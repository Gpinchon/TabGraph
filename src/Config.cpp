/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-07-11 20:59:41
*/

#include "Config.hpp"
#include "Debug.hpp" // for debugLog
#include <exception>
#include <map>
#include <stdio.h> // for sscanf, fgets, fopen
#include <variant>

namespace Config {
class Private {
public:
    static void Load(const std::string& path);
    static void Save(const std::string& path);
    template <typename T>
    static T Get(const std::string& name, T defaultValue);
    template <typename T>
    static T Set(const std::string& name, T value);

private:
    static Config::Private& _instance();
    std::map<std::string, std::variant<float, glm::vec2, glm::vec3, std::string>> _configMap;
};
}

Config::Private& Config::Private::_instance()
{
    static Config::Private* instance = nullptr;
    if (instance == nullptr)
        instance = new Config::Private;
    return *instance;
}

template <typename T>
T Config::Private::Set(const std::string& name, T value)
{
    return _instance()._configMap[name] = value;
}

template <typename T>
T Config::Private::Get(const std::string& name, T defaultValue)
{
    auto it = _instance()._configMap.find(name);
    if (it != _instance()._configMap.end())
        return *it;
    else
        return Config::Private::Set(name, defaultValue);
}

void Config::Load(const std::string& path)
{
    Config::Private::Load(path);
}

void Config::Save(const std::string& path)
{
    Config::Private::Save(path);
}

template <typename T>
T Config::Get(const std::string& name, const T defaultValue)
{
    return Config::Private::Get<T>(name, defaultValue);
}

template <typename T>
T Config::Set(const std::string& name, const T value)
{
    return Config::Private::Set(name, value);
}

/*
Config* Config::_instance = nullptr;

Config* Config::_get()
{
    if (nullptr == _instance)
        _instance = new Config();
    return (_instance);
}

void Config::Load(const std::string& path)
{
    auto fd = fopen(path.c_str(), "r");
    if (nullptr == fd) {
        debugLog("Config file not found");
        return;
    }
    char buffer[4096];
    while (fgets(buffer, 4096, fd)) {
        unsigned valu, valu1;
        int vali;
        char vals[4096];
        if (sscanf(buffer, "WindowName = %s", vals)) {
            Config::WindowName() = vals;
        } else if (sscanf(buffer, "WindowSize = %u %u", &valu, &valu1)) {
            Config::WindowSize().x = valu;
            Config::WindowSize().y = valu1;
        } else if (sscanf(buffer, "MaxTexRes = %u", &vali)) {
            Config::MaxTexRes() = vali;
        } else if (sscanf(buffer, "ShadowRes = %u", &valu)) {
            Config::ShadowRes() = valu;
        } else if (sscanf(buffer, "Anisotropy = %u", &valu)) {
            Config::Anisotropy() = valu;
        } else if (sscanf(buffer, "MSAA = %u", &valu)) {
            Config::Msaa() = valu;
        } else if (sscanf(buffer, "BloomPass = %u", &valu)) {
            Config::BloomPass() = valu;
        } else if (sscanf(buffer, "LightsPerPass = %u", &valu)) {
            Config::LightsPerPass() = valu;
        } else if (sscanf(buffer, "ShadowsPerPass = %u", &valu)) {
            Config::ShadowsPerPass() = valu;
        } else if (sscanf(buffer, "ReflexionSteps = %u", &valu)) {
            Config::ReflexionSteps() = valu;
        } else if (sscanf(buffer, "ReflexionSamples = %u", &valu)) {
            Config::ReflexionSamples() = valu;
        } else if (sscanf(buffer, "ReflexionBorderFactor = %u", &valu)) {
            Config::ReflexionBorderFactor() = valu;
        }
    }
}

glm::ivec2& Config::WindowSize()
{
    return (_get()->_windowSize);
}

std::string& Config::WindowName()
{
    return (_get()->_windowName);
}

float& Config::Anisotropy()
{
    return (_get()->_anisotropy);
}

int16_t& Config::MaxTexRes()
{
    return (_get()->_maxTexRes);
}

uint16_t& Config::ShadowRes()
{
    return (_get()->_shadowRes);
}

uint16_t& Config::Msaa()
{
    return (_get()->_msaa);
}

uint16_t& Config::BloomPass()
{
    return (_get()->_bloomPass);
}

uint16_t& Config::LightsPerPass()
{
    return (_get()->_lightsPerPass);
}

uint16_t& Config::ShadowsPerPass()
{
    return (_get()->_shadowsPerPass);
}

uint16_t& Config::ReflexionSteps()
{
    return (_get()->_ReflexionSteps);
}

uint16_t& Config::ReflexionSamples()
{
    return (_get()->_reflexionSamples);
}

uint16_t& Config::ReflexionBorderFactor()
{
    return (_get()->_reflexionBorderFactor);
}
*/