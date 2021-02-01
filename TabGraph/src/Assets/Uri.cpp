/*
* @Author: gpinchon
* @Date:   2021-02-01 13:53:07
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-02-01 18:20:08
*/
#include "Assets\Uri.hpp"
#include <regex>
#include <sstream>
#include <locale>
#include <codecvt>

Uri::Uri(const std::wstring& uri)
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
    std::wregex uriRegex { LR"(^\s*(([^:/?#]+):)?(\/\/([^/?#\s]*))?([^?#\s]*)(\?([^#\s]*))?(#(\S*))?)", std::regex::ECMAScript };
    auto searchResult = std::wsregex_iterator(uri.begin(), uri.end(), uriRegex);
    auto searchEnd = std::wsregex_iterator();
    if (searchResult != searchEnd) {
        SetScheme((*searchResult)[2]);
        SetAuthority((*searchResult)[4]);
        SetPath((*searchResult)[5]);
        SetQuery((*searchResult)[7]);
        SetFragment((*searchResult)[9]);
    }
}

void Uri::SetScheme(const std::wstring& str)
{
    _uriParts.at(0) = str;
}

std::wstring Uri::GetScheme() const
{
    return _uriParts.at(0);
}

void Uri::SetAuthority(const std::wstring& str)
{
    _uriParts.at(1) = str;
}

std::wstring Uri::GetAuthority() const
{
    return _uriParts.at(1);
}

void Uri::SetPath(const std::wstring& str)
{
    _uriParts.at(2) = str;
}

std::wstring Uri::GetPath() const
{
    return _uriParts.at(2);
}

void Uri::SetQuery(const std::wstring& str)
{
    _uriParts.at(3) = str;
}

std::wstring Uri::GetQuery() const
{
    return _uriParts.at(3);
}

void Uri::SetFragment(const std::wstring& str)
{
    _uriParts.at(4) = str;
}

std::wstring Uri::GetFragment() const
{
    return _uriParts.at(4);
}

Uri::operator std::wstring()
{
    std::wstring fullUri;
    if (!GetScheme().empty())
        fullUri += GetScheme() + L":";
    if (!GetAuthority().empty())
        fullUri += L"//" + GetAuthority();
    fullUri += GetPath();
    if (!GetQuery().empty())
        fullUri += L"?" + GetQuery();
    if (!GetFragment().empty())
        fullUri += L"#" + GetFragment();
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
    std::wregex dataRegex { LR"(^([-\w]+/[-+\w.]+)?((?:;?[\w]+=[-\w]+)*)(;base64)?,(.*)$)", std::regex::ECMAScript };
    auto data { uri.GetPath() };
    auto searchResult = std::wsregex_iterator(data.begin(), data.end(), dataRegex);
    auto searchEnd = std::wsregex_iterator();
    if (searchResult != searchEnd) {
        SetMime((*searchResult)[1]);
        {
            std::wstringstream parameters { (*searchResult)[2] };
            std::wstring parameter;
            while (std::getline(parameters, parameter, L';')) {
                if (parameter.empty())
                    continue;
                auto separator { parameter.find(L'=') };
                auto attribute { parameter.substr(0, separator) };
                auto value { parameter.substr(separator + 1u) };
                SetParameter(attribute, value);
            }
        }
        SetBase64((*searchResult)[3] == L";base64");
        SetData((*searchResult)[4]);
    }
}

void DataUri::SetMime(const std::wstring& mime)
{
    _mime = mime;
}

std::wstring DataUri::GetMime() const
{
    return _mime;
}

void DataUri::SetParameter(const std::wstring& attribute, const std::wstring& value)
{
    _parameters[attribute] = value;
}

std::wstring DataUri::GetParameter(const std::wstring& attribute) const
{
    auto parameter = _parameters.find(attribute);
    if (parameter != _parameters.end())
        return parameter->second;
    return L"";
}

std::map<std::wstring, std::wstring> DataUri::GetParameters() const
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

void DataUri::SetData(const std::wstring& mime)
{
    _data = mime;
}

std::wstring DataUri::GetData() const
{
    return _data;
}

DataUri::operator std::wstring()
{
    std::wstring fullUri = L"data:";
    if (!GetMime().empty())
        fullUri += GetMime();
    if (!GetParameters().empty())
        for (const auto& parameter : GetParameters())
            fullUri += L';' + parameter.first + L'=' + parameter.second;
    if (GetBase64())
        fullUri += L";base64";
    fullUri += L',' + GetData();
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
    using convert_type = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_type, wchar_t> converter;
    auto convertedData{ converter.to_bytes(GetData()) };
    if (GetBase64())
        return base64_decode(convertedData);
    return convertedData;
}
