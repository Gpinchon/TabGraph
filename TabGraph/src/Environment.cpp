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
    SetShader(Component::Create<Shader>(name + "Shader", Shader::Type::LightingShader));
    GetShader()->Stage(GL_FRAGMENT_SHADER)->AddExtension(Component::Create<ShaderCode>(
        "void OutputEnv() { \
        out_0 = texture(Texture.Environment.Diffuse, CubeTexCoord()); \
        out_1 = vec4(0); \
        }",
        "OutputEnv();"
        ));
}

void Environment::Draw()
{
    GetShader()->SetTexture("Texture.Environment.Diffuse", GetDiffuse());
    GetShader()->SetTexture("Texture.Environment.Irradiance", GetIrradiance());
    GetShader()->use();
    Render::DisplayQuad()->Draw();
    GetShader()->use(false);
}

void Environment::unload()
{
    GetDiffuse()->unload();
    GetDiffuse()->unload();
}