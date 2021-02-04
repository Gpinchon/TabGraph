/*
* @Author: gpinchon
* @Date:   2021-02-01 13:53:07
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-02-01 20:47:00
*/
#include "Assets\Uri.hpp"
#include <array> // for array
#include <codecvt>
#include <ctype.h> // for isalnum
#include <map> // for map, map<>::mapped_type
#include <regex>
#include <sstream> // for basic_istream
#include <string> // for getline

Uri::Uri(const std::string& uri)
{
    if (uri.length() == 0)
        return;
    /**
     * ^\s*                         <---- 0 Start by trimming leading white spaces
     *      (                       <---- 1 Optional scheme
     *          ([^:/?#]+):         <---- 2 "Clean" scheme (everything until we reach a :, a #, a ?)
     *      )?
     *      (\/\/                   <---- 3 Optional authority
     *          ([^/?#\s]*)         <---- 4 "Clean" authority (everything until we reach a #, a ? or a whitespace)
     *      )?
     *      ([^?#\s]*)              <---- 5 Path (everything until we reach a #, a ? or a whitespace)
     *      (\?                     <---- 6 Optional query
     *          ([^#\s]*)           <---- 7 "Clean" query (everything until we reach a # or a whitespace)
     *      )?
     *      (#                      <---- 8 Optional fragment
     *          (\S*)               <---- 9 "Clean" fragment (everything except white spaces)
     *      )?
    */
    std::regex uriRegex { R"(^\s*(([^:/?#]+):)?(\/\/([^/?#\s]*))?([^?#\s]*)(\?([^#\s]*))?(#(\S*))?)", std::regex::ECMAScript };
    auto searchResult = std::sregex_iterator(uri.begin(), uri.end(), uriRegex);
    auto searchEnd = std::sregex_iterator();
    if (searchResult != searchEnd) {
        SetScheme((*searchResult)[2]);
        SetAuthority((*searchResult)[4]);
        SetPath(std::string((*searchResult)[5]));
        SetQuery((*searchResult)[7]);
        SetFragment((*searchResult)[9]);
    }
}

Uri::Uri(const std::filesystem::path& filePath)
{
    SetScheme("file");
    SetPath(filePath);
}

void Uri::SetScheme(const std::string& str)
{
    _scheme = str;
}

std::string Uri::GetScheme() const
{
    return _scheme;
}

void Uri::SetAuthority(const std::string& str)
{
    _authority = str;
}

std::string Uri::GetAuthority() const
{
    return _authority;
}

void Uri::SetPath(const std::filesystem::path& str)
{
    _path = str;
}

std::filesystem::path Uri::GetPath() const
{
    return _path;
}

void Uri::SetQuery(const std::string& str)
{
    _query = str;
}

std::string Uri::GetQuery() const
{
    return _query;
}

void Uri::SetFragment(const std::string& str)
{
    _fragment = str;
}

std::string Uri::GetFragment() const
{
    return _fragment;
}

Uri::operator std::string() const
{
    std::string fullUri;
    if (!GetScheme().empty())
        fullUri += GetScheme() + ":";
    if (!GetAuthority().empty())
        fullUri += "//" + GetAuthority();
    fullUri += GetPath().string();
    if (!GetQuery().empty())
        fullUri += "?" + GetQuery();
    if (!GetFragment().empty())
        fullUri += "#" + GetFragment();
    return fullUri;
}

DataUri::DataUri(const Uri& uri)
{
    /**
     * ^                        <---- 0 No need to trim, Uri already did it
     *  ([-\w]+/[-+\w.]+)?      <---- 1 Mime type
     *  (
     *      (?:;?[\w]+=[-\w]+)* <---- 2 Optional parameters, packed in one string
     *  )
     *  (;base64)?              <---- 3 Optional base64 value
     *  ,(.*)                   <---- 4 The data, we can just take everything there, should be clean
     * $
    */
    std::regex dataRegex { R"(^([-\w]+/[-+\w.]+)?((?:;?[\w]+=[-\w]+)*)(;base64)?,(.*)$)", std::regex::ECMAScript };
    auto data { uri.GetPath().string() };
    auto searchResult = std::sregex_iterator(data.begin(), data.end(), dataRegex);
    auto searchEnd = std::sregex_iterator();
    if (searchResult != searchEnd) {
        SetMime((*searchResult)[1]);
        {
            std::stringstream parameters { (*searchResult)[2] };
            std::string parameter;
            while (std::getline(parameters, parameter, ';')) {
                if (parameter.empty())
                    continue;
                auto separator { parameter.find(L'=') };
                auto attribute { parameter.substr(0, separator) };
                auto value { parameter.substr(separator + 1u) };
                SetParameter(attribute, value);
            }
        }
        SetBase64((*searchResult)[3] == ";base64");
        SetData((*searchResult)[4]);
    }
}

void DataUri::SetMime(const std::string& mime)
{
    _mime = mime;
}

std::string DataUri::GetMime() const
{
    return _mime;
}

void DataUri::SetParameter(const std::string& attribute, const std::string& value)
{
    _parameters[attribute] = value;
}

std::string DataUri::GetParameter(const std::string& attribute) const
{
    auto parameter = _parameters.find(attribute);
    if (parameter != _parameters.end())
        return parameter->second;
    return "";
}

std::map<std::string, std::string> DataUri::GetParameters() const
{
    return _parameters;
}

void DataUri::SetBase64(bool base64)
{
    _base64 = base64;
}

bool DataUri::GetBase64() const
{
    return _base64;
}

void DataUri::SetData(const std::string& mime)
{
    _data = mime;
}

std::string DataUri::GetData() const
{
    return _data;
}

DataUri::operator std::string()
{
    std::string fullUri = "data:";
    if (!GetMime().empty())
        fullUri += GetMime();
    if (!GetParameters().empty())
        for (const auto& parameter : GetParameters())
            fullUri += ';' + parameter.first + '=' + parameter.second;
    if (GetBase64())
        fullUri += ";base64";
    fullUri += ',' + GetData();
    return fullUri;
}

static inline bool is_base64(unsigned char c)
{
    return (isalnum(c) || (c == '+') || (c == '/'));
}

static inline std::string base64_decode(std::string const& encoded_string)
{
    int in_len = static_cast<int>(encoded_string.size());
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    std::string ret;

    const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                     "abcdefghijklmnopqrstuvwxyz"
                                     "0123456789+/";

    while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
        char_array_4[i++] = encoded_string[in_];
        in_++;
        if (i == 4) {
            for (i = 0; i < 4; i++)
                char_array_4[i] = static_cast<unsigned char>(base64_chars.find(char_array_4[i]));

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (i = 0; (i < 3); i++)
                ret += char_array_3[i];
            i = 0;
        }
    }

    if (i) {
        for (j = i; j < 4; j++)
            char_array_4[j] = 0;

        for (j = 0; j < 4; j++)
            char_array_4[j] = static_cast<unsigned char>(base64_chars.find(char_array_4[j]));

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (j = 0; (j < i - 1); j++)
            ret += char_array_3[j];
    }
    return ret;
}

std::string DataUri::Decode() const
{
    if (GetBase64())
        return base64_decode(GetData());
    return GetData();
}

std::ostream& operator<<(std::ostream& os, const Uri& uri)
{
    os << std::string(uri);
    return os;
}
