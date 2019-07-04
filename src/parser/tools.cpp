/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-25 15:43:20
*/

#include <algorithm> // for max, count, replace
#include <errno.h> // for ENOENT, EACCES, ELOOP, ENAMETOOLONG
#include <ext/alloc_traits.h> // for __alloc_traits<>::value_type
#include <fstream> // for ifstream
#include <io.h> // for access, R_OK
#include <iostream> // for basic_ostream::operator<<, char_traits
#include <iterator> // for istreambuf_iterator, operator!=
#include <stdexcept> // for runtime_error
#include <stdio.h> // for fseek, ftell, size_t, fread, FILE, SEE...
#include <stdlib.h> // for errno
#include <string.h> // for strerror
#include <string> // for string, operator+, basic_string<>::con...
#include <vector> // for allocator, vector

std::string access_get_error(const int& access_result)
{
    std::string errorMessage;

    std::cout << ENOENT << " " << access_result << std::endl;
    switch (access_result) {
    case EACCES:
        errorMessage = "Permission Denied";
        break;
    case ELOOP:
        errorMessage = "Symbolic Loop";
        break;
    case ENAMETOOLONG:
        errorMessage = "Name Too Long";
        break;
    case ENOENT:
        errorMessage = "No Such File";
        break;
    case ENOTDIR:
        errorMessage = "Wrong Path";
        break;
    case EROFS:
        errorMessage = "File is Read Only";
        break;
    }
    return (errorMessage);
}

std::string file_to_str(const std::string& path)
{
    std::ifstream ifs(path.c_str());
    std::string fileBuffer((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
    return (fileBuffer);
}

std::string stream_to_str(FILE* stream)
{
    size_t size;
    size_t cur;

    cur = ftell(stream);
    fseek(stream, 0L, SEEK_END);
    size = ftell(stream);
    rewind(stream);
    auto ret = std::vector<char>(size + 1, 0);
    fread(&ret[0], sizeof(char), size, stream);
    fseek(stream, cur, SEEK_SET);
    auto retString = std::string(&ret[0]);
    return (retString);
}

std::string stream_to_str(FILE* stream, size_t offset)
{
    size_t size;
    size_t cur;

    cur = ftell(stream);
    fseek(stream, 0L, SEEK_END);
    size = ftell(stream) - offset;
    fseek(stream, offset, SEEK_SET);
    auto ret = new char[size + 1];
    ret[size] = '\0';
    fread(ret, sizeof(ret[0]), size, stream);
    fseek(stream, cur, SEEK_SET);
    auto retString = std::string(ret);
    delete[] ret;
    return (retString);
}

std::string convert_backslash(std::string str)
{
    std::replace(str.begin(), str.end(), '\\', '/');
    return (str);
}

std::vector<std::string> split_path(const std::string& path)
{
    std::vector<std::string> final_path(2);
    auto localPath = convert_backslash(path);
    auto slashpos = localPath.find_last_of('/');

    final_path[0] = std::string(localPath.substr(0, slashpos)) + "/";
    final_path[1] = std::string(localPath.substr(slashpos));
    return (final_path);
}

std::vector<std::string> strsplit(const std::string& s, char c)
{
    size_t pos = 0;
    size_t needle = 0;
    std::vector<std::string> ret;
    while (s[needle] == c) {
        needle++;
    }
    while ((pos = s.find(c, needle)) != std::string::npos) {
        ret.push_back(s.substr(needle, pos - needle));
        while (s[pos] == c) {
            pos++;
        }
        needle = pos;
    }
    if (needle < s.length()) {
        ret.push_back(s.substr(needle));
    }
    return (ret);
}

std::vector<std::string> strsplitwspace(const std::string& s)
{
    size_t pos = 0;
    size_t needle = 0;
    std::vector<std::string> ret;
    while (pos < s.length()) {
        if (s[pos] != ' ' && s[pos] != '\t' && s[pos] != '\n') {
            while (pos < s.length() && s[pos] != ' ' && s[pos] != '\t' && s[pos] != '\n') {
                pos++;
            }
            ret.push_back(s.substr(needle, pos - needle));
        }
        while (s[pos] == ' ' || s[pos] == '\t' || s[pos] == '\n') {
            pos++;
        }
        needle = pos;
    }
    if (needle < s.length()) {
        ret.push_back(s.substr(needle));
    }
    return (ret);
}

unsigned count_char(const std::string& str, char c)
{
    return (std::count(str.begin(), str.end(), c));
}

std::string fileFormat(const std::string& path)
{
    return path.substr(path.find_last_of(".") + 1);
}

FILE* openFile(const std::string& path, const std::string& mod)
{
    FILE* fd;
    if (access(path.c_str(), R_OK) != 0) {
        throw std::runtime_error(std::string("Can't access ") + path + " : " + strerror(errno));
    }
    if ((fd = fopen(path.c_str(), mod.c_str())) == nullptr) {
        throw std::runtime_error(std::string("Can't open ") + path + " : " + strerror(errno));
    }
    return fd;
}

/*unsigned long long	hash(GLubyte *str)
{
	unsigned long long	hash;
	int					c;

	hash = 5381;
	while ((c = *str++))
		hash = ((hash << 5) + hash) + c;
	return (hash);
}*/
