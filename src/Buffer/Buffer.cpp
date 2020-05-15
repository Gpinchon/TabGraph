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

static inline bool is_base64(unsigned char c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_decode(std::string const &encoded_string) {
  int in_len = static_cast<int>(encoded_string.size());
  int i = 0;
  int j = 0;
  int in_ = 0;
  unsigned char char_array_4[4], char_array_3[3];
  std::string ret;

  const std::string base64_chars =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz"
      "0123456789+/";

  while (in_len-- && (encoded_string[in_] != '=') &&
         is_base64(encoded_string[in_])) {
    char_array_4[i++] = encoded_string[in_];
    in_++;
    if (i == 4) {
      for (i = 0; i < 4; i++)
        char_array_4[i] =
            static_cast<unsigned char>(base64_chars.find(char_array_4[i]));

      char_array_3[0] =
          (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] =
          ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

      for (i = 0; (i < 3); i++) ret += char_array_3[i];
      i = 0;
    }
  }

  if (i) {
    for (j = i; j < 4; j++) char_array_4[j] = 0;

    for (j = 0; j < 4; j++)
      char_array_4[j] =
          static_cast<unsigned char>(base64_chars.find(char_array_4[j]));

    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] =
        ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

    for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
  }

  return ret;
}

auto ParseData(const std::string &uri)
{
	std::vector<std::byte> ret;
	std::string header("data:application/octet-stream;base64,");
	if (uri.find(header) == 0) {
		auto data(base64_decode(uri.substr(header.size())));
		std::transform(data.begin(), data.end(), ret.begin(),
                   [] (char c) { return std::byte(c); });
	}
	return ret;
}

void Buffer::LoadToCPU()
{
	if (LoadedToCPU())
		return;
	_rawData.resize(ByteLength(), std::byte(0));
	if (Uri() != "") {
		auto data(ParseData(Uri().string()));
		if (data.empty()) {
			debugLog(Uri());
			auto file(_wfopen(Uri().c_str(), L"rb"));
			debugLog(file);
			fread(&_rawData.at(0), sizeof(std::byte), ByteLength(), file);
			if (ferror(file)) {
				perror ("The following error occurred");
			}
			fclose(file);
		}
		else
			_rawData = data;
	}
	_loadedToCPU = true;
}

void Buffer::LoadToGPU()
{
	if (LoadedToGPU())
		return;
	Allocate();
	if (Uri() != "") {
		auto data(ParseData(Uri().string()));
		if (data.empty()) {
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
		else {
			std::memcpy(Map(BufferAccess::Write), data.data(), ByteLength());
		}
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
