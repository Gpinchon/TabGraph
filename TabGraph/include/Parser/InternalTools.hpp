/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-27 17:36:55
*/

#pragma once

//#include "Physics/BoundingAABB.hpp"
//#include "Mesh/Mesh.hpp"
//#include "Texture/Texture.hpp"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <errno.h>
#include <fcntl.h>
#include <string>
#include <vector>

std::string access_get_error(const int& access_result);
glm::vec3 parse_vec3(std::vector<std::string>& split);
glm::vec2 parse_vec2(std::vector<std::string>& split);
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
