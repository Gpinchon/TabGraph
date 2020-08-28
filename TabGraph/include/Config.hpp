/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-07-12 16:32:56
*/

#pragma once

#include "glm/glm.hpp" // for glm::vec2
#include <map>
#include <stdint.h> // for uint16_t, int16_t
#include <string> // for allocator, string
#include <variant>
#include <filesystem>

/**
* @brief Loader for config files
* @summary The Config file might contains values formated as such :
* WindowSize = 1024 768
* WindowName = "Window Name"
*/
class ConfigFile
{
public:
    /**
    * Loads the Config from the specified file
    * Invalid values will be ignored (set to default value) but setting keys will still be registered
    */
    void Parse(const std::filesystem::path path);
    /** Saves the Config to the specified file */
    void Save(const std::filesystem::path path);
    /** Tries to get the specified setting, set it to the default value if not found */
    template <typename T>
    T Get(const std::string &name, const T defaultValue);
    /** Sets the specified setting to the specified value */
    template <typename T>
    T Set(const std::string &name, const T value);

private:
    std::map<std::string, std::variant<float, glm::vec2, glm::vec3, std::string>> _configMap;
};

/**
* @brief Loads/Saves a config file
* The Config is shared accross the whole program
* @summary The Config file might contains values formated as such :
* WindowSize = 1024 768
* WindowName = "Window Name"
*/
class Config final
{
public:
    /**
        * Loads the Config from the specified file
        * Invalid values will be ignored (set to default value) but setting keys will still be registered
        */
    static void Parse(const std::filesystem::path path);
    /** Saves the Config to the specified file */
    static void Save(const std::filesystem::path path);
    /** Tries to get the specified setting, set it to the default value if not found */
    template <typename T>
    static T Get(const std::string &name, const T defaultValue);
    /** Sets the specified setting to the specified value */
    template <typename T>
    static T Set(const std::string &name, const T value);

private:
    static ConfigFile &_instance();
    //std::map<std::string, std::variant<float, glm::vec2, glm::vec3, std::string>> _configMap;
};

template <>
inline int ConfigFile::Get<int>(const std::string &name, const int defaultValue)
{
    return Get<float>(name, defaultValue);
}

template <>
inline unsigned ConfigFile::Get<unsigned>(const std::string &name, const unsigned defaultValue)
{
    return Get<float>(name, defaultValue);
}

template <typename T>
inline T ConfigFile::Get(const std::string &name, const T defaultValue)
{
    auto it = _configMap.find(name);
    if (it != _configMap.end())
        return std::get<T>((*it).second);
    else
        return ConfigFile::Set(name, defaultValue);
}

template <typename T>
inline T ConfigFile::Set(const std::string &name, const T value)
{
    return std::get<T>(_configMap[name] = value);
}

template <typename T>
inline T Config::Get(const std::string &name, const T defaultValue)
{
    return _instance().Get<T>(name, defaultValue);
}

template <typename T>
inline T Config::Set(const std::string &name, const T value)
{
    return _instance().Set(name, value);
}
