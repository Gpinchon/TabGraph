#include "Buffer/Buffer.hpp"
#include "Debug.hpp"
#include <cstring>
#include <cstddef>
#include <stdio.h>
#include <wchar.h>

Buffer::Buffer(size_t byteLength) : Object(""), _byteLength(byteLength) //_rawData(byteLength, std::byte(0))
{
}

Buffer::~Buffer()
{
	Unload();
}

std::shared_ptr<Buffer> Buffer::Create(size_t byteLength)
{
	return std::shared_ptr<Buffer>(new Buffer(byteLength));
}

void Buffer::UpdateGPU()
{
	std::memcpy(Map(BufferAccess::Write), _rawData.data(), ByteLength());
	Unmap();
}

void Buffer::Allocate()
{
	if (Glid() > 0)
		glDeleteBuffers(1, &_glid);
	glCreateBuffers(1, &_glid);
	glCheckError();
	glNamedBufferData(
		Glid(),
 		ByteLength(),
 		nullptr,
 		Usage()
 	);
 	glCheckError();
}

void *Buffer::Map(GLenum access)
{
	if (Glid() == 0)
		Allocate();
	auto ptr(glMapNamedBuffer(Glid(), access));
	glCheckError();
	return ptr;
}

void *Buffer::MapRange(size_t offset, size_t length, GLbitfield access)
{
	if (Glid() == 0)
		Allocate();
	auto ptr(glMapNamedBufferRange(Glid(), offset, length, access));
	glCheckError();
	return ptr;
}

void Buffer::Unmap()
{
	glUnmapNamedBuffer(Glid());
	glCheckError();
}

void Buffer::Load()
{
	LoadToCPU();
	LoadToGPU();
}

void Buffer::LoadToCPU()
{
	if (LoadedToCPU())
		return;
	_rawData.resize(ByteLength(), std::byte(0));
	if (Uri() != "") {
		debugLog(Uri());
		auto file(_wfopen(Uri().c_str(), L"rb"));
		debugLog(file);
		fread(&_rawData.at(0), sizeof(std::byte), ByteLength(), file);
		if (ferror(file)) {
			perror ("The following error occurred");
		}
		fclose(file);
	}
	_loadedToCPU = true;
}

void Buffer::LoadToGPU()
{
	if (LoadedToGPU())
		return;
	Allocate();
	if (Uri() != "") {
		debugLog(Uri());
		auto file(_wfopen(Uri().c_str(), L"rb"));
		debugLog(file);
		fread(Map(BufferAccess::Write), sizeof(std::byte), ByteLength(), file);
		Unmap();
		if (ferror(file)) {
			perror ("The following error occurred");
		}
		fclose(file);
	}
	else if (_rawData.size()) {
		UpdateGPU();
	}
 	_loadedToGPU = true;
}

void Buffer::Unload()
{
	UnloadFromCPU();
	UnloadFromGPU();
}

void Buffer::UnloadFromCPU()
{
	_rawData.resize(0);
	_rawData.shrink_to_fit();
	_loadedToCPU = false;
}

void Buffer::UnloadFromGPU()
{
	glDeleteBuffers(1, &_glid);
	_glid = 0;
	_loadedToGPU = false;
}

bool Buffer::LoadedToCPU()
{
	return _loadedToCPU;
}

bool Buffer::LoadedToGPU()
{
	return _loadedToGPU;
}

std::vector<std::byte> &Buffer::RawData()
{
	LoadToCPU();
	return _rawData;
}

std::filesystem::path Buffer::Uri() const
{
	return _uri;
}

void Buffer::SetUri(std::string uri)
{
	_uri = uri;
}

size_t Buffer::ByteLength() const
{
	return _byteLength;
	//return _rawData.size();
}

void Buffer::SetByteLength(size_t byteLength)
{
	_byteLength = byteLength;
	//_rawData.resize(byteLength);
}

GLenum Buffer::Usage() const
{
	return _usage;
}

void Buffer::SetUsage(GLenum usage)
{
	_usage= usage;
}

GLuint Buffer::Glid() const
{
	return _glid;
}
