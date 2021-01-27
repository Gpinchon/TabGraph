/*
* @Author: gpinchon
* @Date:   2021-01-04 09:42:57
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:41:39
*/

#include "Environment.hpp"
#include "Mesh/Geometry.hpp"
#include "Render.hpp"
#include "Shader/Shader.hpp"
#include "Texture/Cubemap.hpp"

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
    GetDiffuse()->Unload();
    GetDiffuse()->Unload();
}