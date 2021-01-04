#pragma once

#include "Component.hpp"
#include "GL/glew.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>

class ShaderCode : public Component {
public:
    ShaderCode(const std::string& code = "", const std::string& technique = "") : _code(code), _technique(technique) {};
    /*ShaderCode(const std::filesystem::path &path, const std::string& technique = "") :  _technique(technique) {
        std::ifstream fileStream(path);
        std::stringstream buffer;
        buffer << fileStream.rdbuf();
        _code = buffer.str();
    };*/
    std::string Code() const;
    void SetCode(const std::string& code);
    std::string Technique() const;
    void SetTechnique(const std::string);

private:
    virtual std::shared_ptr<Component> _Clone() override {
        return Component::Create<ShaderCode>(*this);
    }
    virtual void _LoadCPU() override {};
    virtual void _UnloadCPU() override {};
    virtual void _LoadGPU() override {};
    virtual void _UnloadGPU() override {};
    virtual void _UpdateCPU(float) override {};
    virtual void _FixedUpdateCPU(float) override {};
    std::string _code{ "" };
    std::string _technique{ "" };
};

class ShaderStage : public Component {
public:
    ShaderStage(GLenum stage = 0, const std::shared_ptr<ShaderCode>& code = nullptr);
    void Compile();
    void Recompile();
    /**
    * @arg name : the name of the extension
    * @arg extension : the extension
    */
    void AddExtension(const std::shared_ptr<ShaderCode>& extension);
    void RemoveExtension(const std::shared_ptr<ShaderCode>& extension);
    //void AddExtension(const std::shared_ptr<ShaderStage>& extension);
    void RemoveExtension(const std::string& name);
    auto Extensions() const { return GetComponents<ShaderCode>(); };
    //std::shared_ptr<ShaderStage> Extension(const std::string& name) const;
    std::string FullCode();
    /*std::string Code() const;
    void SetCode(const std::string &code);*/
    GLuint Glid() const;
    GLenum Stage() const;
    bool Compiled() const;
    void SetDefine(const std::string, const std::string = "");
    void RemoveDefine(const std::string);
    /*std::string Technique() const;
    void SetTechnique(const std::string);*/
    void Delete();
    ~ShaderStage();

private:
    virtual std::shared_ptr<Component> _Clone() override {
        return Component::Create<ShaderStage>(*this);
    }
    virtual void _LoadCPU() override {};
    virtual void _UnloadCPU() override {};
    virtual void _LoadGPU() override {};
    virtual void _UnloadGPU() override {};
    virtual void _UpdateCPU(float /*delta*/) override {};
    virtual void _FixedUpdateCPU(float /*delta*/) override {};
    GLuint _glid { 0 };
    GLenum _stage { 0 };
    std::string _fullCode { "" };
    //std::string _code { "" };
    //std::string _technique { "" };
    bool _compiled { false };
    std::unordered_map<std::string, std::string> _defines;
    //std::unordered_map<std::string, std::shared_ptr<ShaderStage>> _extensions;
};
