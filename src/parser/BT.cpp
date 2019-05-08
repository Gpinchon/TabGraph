/*
* @Author: gpinchon
* @Date:   2019-05-08 11:07:22
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-05-08 22:22:01
*/

#include "Texture.hpp"
#include "TextureParser.hpp"
#include "parser/InternalTools.hpp"
#include <unistd.h>

std::shared_ptr<Texture> BTParse(const std::string& texture_name, const std::string& path)
{
    BTHeader    header;
    void        *data;
    GLenum      dataFormat;
    GLenum      internalFormat;
    size_t      readSize;

    auto fd = openFile(path);
    if ((readSize = fread(&header, 1, sizeof(BTHeader), fd) != sizeof(BTHeader))) {
        fclose(fd);
        throw std::runtime_error(std::string("[ERROR] ") + path + " : " +
            "Invalid file header, expected size " + std::to_string(sizeof(BTHeader)) + " got " + std::to_string(readSize));
    }
    if (header.dataSize == 2) {
        dataFormat = header.fPoint ? GL_HALF_FLOAT : GL_SHORT;
        internalFormat = header.fPoint ? GL_R16F : GL_R16I;
    }
    else if (header.dataSize == 4) {
        dataFormat = header.fPoint ? GL_FLOAT : GL_INT;
        internalFormat = header.fPoint ? GL_R32F : GL_R32I;
    }
    else {
        fclose(fd);
        throw std::runtime_error(std::string("[ERROR] ") + path + " : " + 
            "Invalid data size " + std::to_string(header.dataSize));
    }
    size_t totalSize = header.dataSize * header.rows * header.columns;
    data = new unsigned char[totalSize];
    if ((readSize = fread(data, 1, totalSize, fd) != totalSize)) {
        fclose(fd);
        throw std::runtime_error(std::string("[ERROR] ") + path + " : " +
            "Invalid map size, expected size " + std::to_string(totalSize) + " got " + std::to_string(readSize));
    }
    fclose(fd);
    return Texture::create(texture_name, new_vec2(header.columns, header.rows), GL_TEXTURE_2D, GL_RED, internalFormat, dataFormat, data);
}

auto __btParser = TextureParser::add("bt", BTParse);
auto __BTParser = TextureParser::add("BT", BTParse);