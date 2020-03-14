#pragma once

#include <vector>
#include <memory>
#include <filesystem>
#include <GL/glew.h>
#include "Object.hpp"

/** A buffer points to binary geometry, animation, or skins. */
class Buffer : public Object
{
public:
	static std::shared_ptr<Buffer> Create(size_t byteLength);
	/** Calls LoadToCPU() and LoadToGPU() */
	void Load();
	/** Reads the data from the uri into rawData */
	void LoadToCPU();
	/**
	 * Loads the data of rawData into the VRAM
	 * If the buffer is not loaded to cpu, LoadToCPU is called
	 */
	void LoadToGPU();
	/** Calls UnloadFromCPU() and UnloadFromGPU() */
	void Unload();
	void UnloadFromCPU();
	void UnloadFromGPU();
	bool LoadedToCPU();
	bool LoadedToGPU();
	/** The total byte length of the buffer. */
	size_t ByteLength() const;
	/** Sets the buffer's byte length and RESIZE RAW DATA !!! */
	void SetByteLength(size_t);
	/** The buffer's raw data */
	std::vector<std::byte> &RawData();
	/** The uri of the buffer. */
	std::filesystem::path Uri() const;
	/** Sets the URI */
	void SetUri(std::string);
	GLuint Glid() const;
	GLenum Usage() const;
	void SetUsage(GLenum);
	~Buffer();

protected:
	Buffer(size_t byteLength);
	Buffer() = delete;
	
private:
	std::filesystem::path _uri {""};
	std::vector<std::byte> _rawData {};
	bool _loadedToCPU {false};
	bool _loadedToGPU {false};
	GLuint _glid{0};
	GLenum _usage {GL_STATIC_DRAW};
	size_t _byteLength{0};
};