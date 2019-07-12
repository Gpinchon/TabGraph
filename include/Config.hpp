/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-07-12 11:21:02
*/

#pragma once

#include "glm/glm.hpp" // for glm::vec2
#include <stdint.h> // for uint16_t, int16_t
#include <string> // for allocator, string
#include <variant>
#include <map>

/**
* @brief Loads/Saves a config file
* The Config is shared accross the whole program
* @summary The Config file might contains values formated as such :
* WindowSize = 1024 768
* WindowName = "Window Name"
*/
class Config final {
public:
    /**
        * Loads the Config from the specified file
        * Invalid values will be ignored (set to default value) but setting keys will still be registered
        */
    static void Load(const std::string& path);
    /** Saves the Config to the specified file */
    static void Save(const std::string& path);
    /** Tries to get the specified setting, set it to the default value if not found */
    template <typename T>
    static T Get(const std::string& name, const T defaultValue);
    /** Sets the specified setting to the specified value */
    template <typename T>
    static T Set(const std::string& name, const T value);
private :
    static Config& _instance();
    std::map<std::string, std::variant<float, glm::vec2, glm::vec3, std::string>> _configMap;
};

template <>
inline int Config::Get<int>(const std::string& name, const int defaultValue)
{
    return Get<float>(name, defaultValue);
}

template <>
inline unsigned Config::Get<unsigned>(const std::string& name, const unsigned defaultValue)
{
    return Get<float>(name, defaultValue);
}

template <typename T>
inline T Config::Get(const std::string& name, const T defaultValue)
{
    auto it = _instance()._configMap.find(name);
    if (it != _instance()._configMap.end())
        return std::get<T>((*it).second);
    else
        return Config::Set(name, defaultValue);
}

template <typename T>
inline T Config::Set(const std::string& name, const T value)
{
    return std::get<T>(_instance()._configMap[name] = value);
}


/*class Config {
public:
    static void Load(const std::string&);
    static glm::ivec2& WindowSize();
    static std::string& WindowName();
    static float& Anisotropy();
    static int16_t& MaxTexRes();
    static uint16_t& ShadowRes();
    static uint16_t& Msaa();
    static uint16_t& BloomPass();
    static uint16_t& LightsPerPass();
    static uint16_t& ShadowsPerPass();
    static uint16_t& ReflexionSteps();
    static uint16_t& ReflexionSamples();
    static uint16_t& ReflexionBorderFactor();

private:
    static Config* _get();
    static Config* _instance;
    glm::ivec2 _windowSize { 1280, 720 };
    std::string _windowName { "" };
    float _anisotropy { 16.f };
    int16_t _maxTexRes { 0 };
    uint16_t _shadowRes { 2048 };
    uint16_t _msaa { 0 };
    uint16_t _bloomPass { 1 };
    uint16_t _lightsPerPass { 32 };
    uint16_t _shadowsPerPass { 16 };
    uint16_t _ReflexionSteps { 10 };
    uint16_t _reflexionSamples { 9 };
    uint16_t _reflexionBorderFactor { 10 };
    Config() = default;
    ~Config() = default;
};*/
