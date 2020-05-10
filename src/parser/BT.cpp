/*
* @Author: gpinchon
* @Date:   2019-05-08 11:07:22
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-05-10 19:26:25
*/

#include "Texture/Texture2D.hpp" // for Texture2D
#include "Texture/TextureParser.hpp" // for TextureParser
#include "Parser/InternalTools.hpp" // for BTHeader, openFile
#include <GL/glew.h> // for GLenum, GL_FLOAT, GL_HALF_FLOAT
#include <glm/glm.hpp> // for glm::vec2
#include <memory> // for allocator, shared_ptr
#include <stdexcept> // for runtime_error
#include <stdio.h> // for fclose, fread, size_t
#include <string> // for operator+, char_traits, to_string

std::shared_ptr<Texture2D> BTParse(const std::string& texture_name, const std::string& path)
{
    BTHeader header;
    void* data;
    GLenum dataFormat;
    GLenum internalFormat;
    size_t readSize;

    auto fd = openFile(path);
    if ((readSize = fread(&header, 1, sizeof(BTHeader), fd) != sizeof(BTHeader))) {
        fclose(fd);
        throw std::runtime_error(std::string("[ERROR] ") + path + " : " + "Invalid file header, expected size " + std::to_string(sizeof(BTHeader)) + " got " + std::to_string(readSize));
    }
    if (header.dataSize == 2) {
        dataFormat = header.fPoint ? GL_HALF_FLOAT : GL_SHORT;
        internalFormat = header.fPoint ? GL_R16F : GL_R16I;
    } else if (header.dataSize == 4) {
        dataFormat = header.fPoint ? GL_FLOAT : GL_INT;
        internalFormat = header.fPoint ? GL_R32F : GL_R32I;
    } else {
        fclose(fd);
        throw std::runtime_error(std::string("[ERROR] ") + path + " : " + "Invalid data size " + std::to_string(header.dataSize));
    }
    size_t totalSize = header.dataSize * header.rows * header.columns;
    data = new unsigned char[totalSize];
    if ((readSize = fread(data, 1, totalSize, fd) != totalSize)) {
        fclose(fd);
        throw std::runtime_error(std::string("[ERROR] ") + path + " : " + "Invalid map size, expected size " + std::to_string(totalSize) + " got " + std::to_string(readSize));
    }
    fclose(fd);
    return Texture2D::Create(texture_name, glm::vec2(header.columns, header.rows), GL_TEXTURE_2D, GL_RED, internalFormat, dataFormat, data);
}

auto __btParser = TextureParser::Add("bt", BTParse);
auto __BTParser = TextureParser::Add("BT", BTParse);