/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-27 17:36:55
*/

#pragma once

#include "AABB.hpp"
#include "Mesh.hpp"
#include "Texture.hpp"
#include <errno.h>
#include <fcntl.h>
#include <string>
#include <unistd.h>
#include <vector>

#ifndef O_BINARY
#define O_BINARY 0x0004
#endif

struct t_obj_parser {
    std::vector<std::string> path_split;
    FILE* fd { nullptr };
    std::vector<glm::vec3> v;
    std::vector<glm::vec3> vn;
    std::vector<glm::vec2> vt;
    std::shared_ptr<Mesh> parent;
    std::shared_ptr<Vgroup> vg;
    AABB bbox;
};

#pragma pack(1)
struct t_bmp_header {
    short type;
    unsigned size;
    short reserved1;
    short reserved2;
    unsigned data_offset;
};

struct t_bmp_info {
    unsigned header_size;
    int width;
    int height;
    short color_planes;
    short bpp;
    unsigned compression_method;
    unsigned size;
    int horizontal_resolution;
    int vertical_resolution;
    int colors;
    int important_colors;
};

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

struct t_bmp_parser {
    FILE* fd;
    t_bmp_info info;
    t_bmp_header header;
    GLubyte* data { nullptr };
    unsigned size_read;
};

std::string access_get_error(const int& access_result);
glm::vec3 parse_vec3(std::vector<std::string>& split);
glm::vec2 parse_vec2(std::vector<std::string>& split);
void parse_vg(t_obj_parser* p, const std::string& name = "");
void parse_v(t_obj_parser* p, std::vector<std::string>& split, glm::vec2* in_vt);
void parse_vtn(t_obj_parser* p, std::vector<std::string>& split);
void correct_vt(glm::vec2* vt);
glm::vec2 generate_vt(glm::vec3 v, glm::vec3 center);
glm::vec3 generate_vn(glm::vec3* v);
std::string strjoin(char const* s1, char const* s2);
std::vector<std::string> split_path(const std::string& path);
std::vector<std::string> strsplit(const std::string& s, char c);
std::vector<std::string> strsplitwspace(const std::string& s);
unsigned count_char(const std::string& str, char c);
std::string file_to_str(const std::string& path);
std::string stream_to_str(FILE* stream);
std::string stream_to_str(FILE* stream, size_t offset);
std::string convert_backslash(std::string str);
std::string fileFormat(const std::string& path);

//Attempts to access file then open it, throws if it can't
FILE* openFile(const std::string& path, const std::string& mod = "rb");

template <typename T>
std::vector<T> stream_to_vector(FILE* stream, size_t offset)
{
    size_t size;
    size_t cur;

    cur = ftell(stream);
    fseek(stream, 0L, SEEK_END);
    size = ftell(stream) - offset;
    fseek(stream, offset, SEEK_SET);
    auto ret = std::vector<T>(size);
    fread(&ret[0], sizeof(ret[0]), size, stream);
    fseek(stream, cur, SEEK_SET);
    return (ret);
}
