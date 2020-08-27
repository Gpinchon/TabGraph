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

#pragma pack(1)
struct BTHeader {
    unsigned char marker[10]; //"binterr1.3"
    int columns; //Width (east-west) dimension of the height grid.
    int rows; //Height (north-south) dimension of the height grid.
    short dataSize; //Bytes per elevation grid point, either 2 or 4.
    short fPoint; //Floating-point flag
        //If 1, the data consists of floating point values (float), otherwise they are signed integers.
    short hUnits; //Horizontal units
        //0: Degrees
        //1: Meters
        //2: Feet (international foot = .3048 meters)
        //3: Feet (U.S. survey foot = 1200/3937 meters)
    short UTMZone; //Indicates the UTM zone (1-60) if the file is in UTM.  Negative zone numbers are used for the southern hemisphere.
    short datum; //Indicates the Datum, see Datum Values below.
    double leftExtent; //The extents are specified in the coordinate space (projection) of the file.  For example, if the file is using UTM, then the extents are in UTM coordinates.
    double rightExtent;
    double bottomExtent;
    double topExtent;
    short projection; //External projection
        //0: Projection is fully described by this header
        //1: Projection is specified in a external .prj file
    float verticalScale; // (vertical units) Vertical units in meters, usually 1.0.  The value 0.0 should be interpreted as 1.0 to allow for backward compatibility.
    unsigned char padding[190]; //unused Bytes of value 0 are used to pad the rest of the header.
};
#pragma pack()

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

auto __btParser = TextureParser::Add(".bt", BTParse);
auto __BTParser = TextureParser::Add(".BT", BTParse);