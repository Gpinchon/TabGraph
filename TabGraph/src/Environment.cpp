/*
* @Author: gpinchon
* @Date:   2021-01-04 09:42:57
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:41:39
*/

#include "Environment.hpp"
#include "Mesh/Geometry.hpp"
#include "Render.hpp"
#include "Shader/Program.hpp"
#include "Texture/Cubemap.hpp"

auto GetVertexCode() {
    static auto deferred_vert_code =
#include "deferred.vert"
        ;
    static Shader::Stage::Code shaderCode{ deferred_vert_code, "FillVertexData();" };
    return shaderCode;
}

auto GetFragmentCode() {
    static std::string envShader =
#include "environment.frag"
        ;
    static Shader::Stage::Code shaderCode { envShader, "OutputEnv();" };
    return shaderCode;
}

Environment::Environment(const std::string& name)
    : Component(name)
{
    
    SetShader(Component::Create<Shader::Program>(name + "Shader"));
    GetShader()->SetDefine("LIGHTSHADER");
    GetShader()->Attach(Shader::Stage(Shader::Stage::Type::Vertex, GetVertexCode()));
    GetShader()->Attach(Shader::Stage(Shader::Stage::Type::Fragment, GetFragmentCode()));
}

void Environment::Draw()
{
    GetShader()->Use();
    GetShader()->SetTexture("Environment.Diffuse", GetDiffuse());
    GetShader()->SetTexture("Environment.Irradiance", GetIrradiance());
    Render::DisplayQuad()->Draw();
    GetShader()->Done();
}

void Environment::unload()
{
    GetDiffuse()->Unload();
    GetDiffuse()->Unload();
}