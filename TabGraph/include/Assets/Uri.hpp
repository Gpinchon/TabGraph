/*
* @Author: gpinchon
* @Date:   2021-02-01 13:53:30
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-02-01 18:26:33
*/
#pragma once
#include <array>
#include <map>
#include <regex>
#include <sstream>

/**
 * @brief Parses Uri to make it more understandable to humans
 * This ignores spaces on purpose so make sure to convert them to %20
 * @ref https://www.ietf.org/rfc/rfc2396.txt
*/
class Uri {
public:
    /**
     * @brief Parses uri wstring into Uri
     * @param uri a uri containing no space
    */
    Uri(const std::wstring& uri);
    void SetScheme(const std::wstring& str);
    std::wstring GetScheme() const;
    void SetAuthority(const std::wstring& str);
    std::wstring GetAuthority() const;
    void SetPath(const std::wstring& str);
    std::wstring GetPath() const;
    void SetQuery(const std::wstring& str);
    std::wstring GetQuery() const;
    void SetFragment(const std::wstring& str);
    std::wstring GetFragment() const;
    operator std::wstring();

private:
    std::array<std::wstring, 5> _uriParts { { L"" } };
};

/**
 * @brief Converts a Uri into a data uri.
*/
class DataUri {
public:
    /**
     * @brief Silently creates an empty data uri (data:,) if Uri is not a data scheme
     * @param uri a data Uri
    */
    DataUri(const Uri& uri);
    void SetMime(const std::wstring& mime);
    std::wstring GetMime() const;
    void SetParameter(const std::wstring& attribute, const std::wstring& value);
    std::wstring GetParameter(const std::wstring& attribute) const;
    std::map<std::wstring, std::wstring> GetParameters() const;
    void SetBase64(bool base64);
    bool GetBase64() const;
    void SetData(const std::wstring& mime);
    std::wstring GetData() const;
    operator std::wstring();
    /**
     * @brief Converts data into raw binary string
     * @return the converted data
    */
    std::string Decode() const;

private:
    bool _base64 { false };
    std::wstring _mime {};
    std::map<std::wstring, std::wstring> _parameters {};
    std::wstring _data {};
};
