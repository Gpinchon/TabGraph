/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-07-13 12:49:34
*/

#include "Config.hpp"
#include "Debug.hpp" // for debugLog
#include <exception>
#include <fstream>
#include <iterator>
#include <sstream>
#include <stdio.h> // for sscanf, fgets, fopen
#include <vector>

Config& Config::_instance()
{
    static Config* instance = nullptr;
    if (instance == nullptr)
        instance = new Config;
    return *instance;
}

void Config::Parse(const std::string& path)
{
    std::ifstream configFile(path);
    for (std::string line; std::getline(configFile, line);) {
        std::istringstream iss(line);
        std::vector<std::string> words(
            std::istream_iterator<std::string> { iss },
            std::istream_iterator<std::string>());
        if (words.size() > 2 && words.at(1) == "=") {
            std::cout << words.at(2) << std::endl;
            switch (words.size() - 2) {
            case 1:
                try {
                    _instance()._configMap[words.at(0)] = std::stof(words.at(2));
                } catch (std::exception&) {
                    _instance()._configMap[words.at(0)] = words.at(2);
                }
                break;
            case 2:
                _instance()._configMap[words.at(0)] = glm::vec2(std::stof(words.at(2)), std::stof(words.at(3)));
                break;
            case 3:
                _instance()._configMap[words.at(0)] = glm::vec3(std::stof(words.at(2)), std::stof(words.at(3)), std::stof(words.at(4)));
                break;
            }
        }
    }
}

void Config::Save(const std::string& path)
{
    std::ofstream configFile;
    configFile.open(path);
    for (const auto& v : _instance()._configMap) {
        configFile << v.first << " = ";
        switch (v.second.index()) {
        case 0:
            configFile << std::get<0>(v.second);
            break;
        case 1:
            configFile << std::get<1>(v.second).x << ' ' << std::get<1>(v.second).y;
            break;
        case 2:
            configFile << std::get<2>(v.second).x << ' ' << std::get<2>(v.second).y << ' ' << std::get<2>(v.second).z;
            break;
        case 3:
            configFile << std::get<3>(v.second);
            break;
        }
        configFile << '\n';
    }
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