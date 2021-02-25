/*
* @Author: gpinchon
* @Date:   2021-01-04 09:42:56
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-02-22 18:30:45
*/
#pragma once
#include "Component.hpp"
#include "Shader/Stage.hpp"

#include <glm/glm.hpp>
#include <map>
#include <memory>
#include <string>

class Texture2D;
namespace Shader {
    class Program;
};

class MaterialExtension : public Component {
public:
    using Colors = std::map<std::string, glm::vec3>;
    using Defines = std::map<std::string, std::string>;
    using Textures = std::map<std::string, std::shared_ptr<Texture2D>>;
    using Values = std::map<std::string, float>;
    /**
     * @brief returns the extension's fragment shader's code
     * @return the shader's Fragment stage extension code
    */
    virtual void SetCode(const Shader::Stage::Code& code) final;
    virtual void SetColor(const std::string& name, const glm::vec3& value) final;
    virtual void SetValue(const std::string& name, const float value) final;
    virtual void SetDefine(const std::string& name, const std::string& value = "") final;
    virtual void RemoveDefine(const std::string& name) final;
    virtual void SetTexture(const std::string& name, const std::shared_ptr<Texture2D>& value) final;
    virtual const Shader::Stage::Code& GetCode() const final;
    virtual const glm::vec3 GetColor(const std::string& name) const final;
    virtual const float GetValue(const std::string& name) const final;
    virtual const std::string GetDefine(const std::string& name) const final;
    virtual const std::shared_ptr<Texture2D> GetTexture(const std::string& name) const final;

    /**
     * @brief returns the extension's colors
     * @return the extension's colors map
    */
    virtual const Colors& GetColors() const final;
    virtual const Values& GetValues() const final;
    virtual const Defines& GetDefines() const final;
    virtual const Textures& GetTextures() const final;

protected:
    MaterialExtension(const std::string& name);
private:
    virtual std::shared_ptr<Component> _Clone() override
    {
        return Component::Create<MaterialExtension>(*this);
    }
    Shader::Stage::Code _code;
    Colors _colors;
    Values _values;
    Defines _defines;
    Textures _textures;
};