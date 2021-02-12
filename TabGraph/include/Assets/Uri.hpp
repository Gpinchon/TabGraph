/*
* @Author: gpinchon
* @Date:   2021-02-01 13:53:30
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-02-01 18:26:33
*/
#pragma once
#include <string>  // for string, basic_string, string
#include <array>
#include <map>
#include <filesystem>

/**
 * @brief Parses Uri to make it more understandable to humans
 * This ignores spaces on purpose so make sure to convert them to %20
 * @ref https://www.ietf.org/rfc/rfc2396.txt
*/
class Uri {
public:
    /**
     * @brief Parses uri string into Uri
     * @param uri a uri containing no space
    */
    Uri(const std::string& uri);
    /**
     * @brief Creates a URI from a file path
    */
    Uri(const std::filesystem::path& filePath);
    Uri() = default;
    void SetScheme(const std::string& str);
    std::string GetScheme() const;
    void SetAuthority(const std::string& str);
    std::string GetAuthority() const;
    void SetPath(const std::filesystem::path& str);
    std::filesystem::path GetPath() const;
    void SetQuery(const std::string& str);
    std::string GetQuery() const;
    void SetFragment(const std::string& str);
    std::string GetFragment() const;
    operator std::string() const;
    friend std::ostream& operator<<(std::ostream& os, const Uri& uri);

private:
    std::string _scheme{ "" };
    std::string _authority{ "" };
    std::filesystem::path _path{ "" };
    std::string _query{ "" };
    std::string _fragment{ "" };
    //std::array<std::string, 5> _uriParts { { "" } };
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
    void SetMime(const std::string& mime);
    std::string GetMime() const;
    void SetParameter(const std::string& attribute, const std::string& value);
    std::string GetParameter(const std::string& attribute) const;
    std::map<std::string, std::string> GetParameters() const;
    void SetBase64(bool base64);
    bool GetBase64() const;
    void SetData(const std::string& mime);
    const std::string &GetData() const;
    operator std::string();
    /**
     * @brief Converts data into raw binary string
     * @return the converted data
    */
    template<typename T = uint8_t>
    std::vector<T> Decode() const;

private:
    bool _base64 { false };
    std::string _mime {};
    std::map<std::string, std::string> _parameters {};
    std::string _data {};
};

static inline bool is_base64(unsigned char c)
{
    return (isalnum(c) || (c == '+') || (c == '/'));
}

template<typename T>
static inline std::vector<T> base64_decode(std::string const& encoded_string)
{
    int in_len = static_cast<int>(encoded_string.size());
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    std::vector<T> ret;

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
                ret.push_back(T(char_array_3[i]));
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
            ret.push_back(T(char_array_3[j]));
    }
    return ret;
}

template<typename T>
inline std::vector<T> DataUri::Decode() const
{
    if (GetBase64())
        return base64_decode<T>(GetData());
    std::vector<T> data(GetData().size());
    std::transform(GetData().begin(), GetData().end(), data.begin(),
        [](char c) { return T(c); });
    return data;
}
