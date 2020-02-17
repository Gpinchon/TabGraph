#include "Buffer.hpp"
#include "Debug.hpp"
#include <cstddef>
#include <stdio.h>
#include <wchar.h>

Buffer::Buffer(size_t byteLength) : Object(""), _rawData(byteLength, std::byte(0)) {
}

Buffer::~Buffer()
{
	Unload();
}

std::shared_ptr<Buffer> Buffer::Create(size_t byteLength)
{
	return std::shared_ptr<Buffer>(new Buffer(byteLength));
}

void Buffer::Load()
{
	if (!Loaded() && Uri() != "") {
		debugLog(Uri());
		auto file(_wfopen(Uri().c_str(), L"rb"));
		debugLog(file);
		fread(&RawData().at(0), sizeof(std::byte), ByteLength(), file);
		fclose(file);
	}
	glCreateBuffers(1, &_glid);
	glCheckError();
	glNamedBufferData(
		Glid(),
 		ByteLength(),
 		&RawData().at(0),
 		Usage()
 	);
 	glCheckError();
	_loaded = true;
}

void Buffer::Unload()
{
	glDeleteBuffers(1, &_glid);
}

bool Buffer::Loaded()
{
	return _loaded;
}

std::vector<std::byte> &Buffer::RawData()
{
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
}

void Buffer::SetByteLength(size_t byteLength)
{
	_byteLength = byteLength;
	_rawData.resize(byteLength);
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
