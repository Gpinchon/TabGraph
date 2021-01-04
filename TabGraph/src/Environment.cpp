/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-05-10 20:17:28
*/

#include "Environment.hpp"
#include "Render.hpp"
#include "Mesh/Geometry.hpp"
#include "Texture/Cubemap.hpp"
#include "Shader/Shader.hpp"

Environment::Environment(const std::string& name)
    : Component(name)
{
    auto envShader =
#include "environment.frag"
        ;
    SetShader(Component::Create<Shader>(name + "Shader", Shader::Type::LightingShader));
    GetShader()->SetStage(Component::Create<ShaderStage>(GL_FRAGMENT_SHADER, Component::Create<ShaderCode>(envShader, "OutputEnv();")));
}

void Environment::Draw()
{
    GetShader()->SetTexture("Environment.Diffuse", GetDiffuse());
    GetShader()->SetTexture("Environment.Irradiance", GetIrradiance());
    GetShader()->use();
    Render::DisplayQuad()->Draw();
    GetShader()->use(false);
}

void Environment::unload()
{
    GetDiffuse()->unload();
    GetDiffuse()->unload();
}