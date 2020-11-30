/*
* @Author: gpinchon
* @Date:   2020-08-09 22:51:26
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-17 14:00:45
*/
#include "Material/Material.hpp"
#include "Mesh/CubeMesh.hpp"
#include "Mesh/Mesh.hpp"
#include "Assets/AssetsParser.hpp"
#include "Engine.hpp"
#include "Scene/Scene.hpp"

#include "Wall.hpp"

Wall::Wall()
    : GameEntity("Wall")
{
    
}

std::shared_ptr<Wall> Wall::Create()
{
    auto wall(Component::Create<Wall>());
    static auto wallMeshes = AssetsParser::Parse(Engine::ResourcePath() / "models/wall/wall.gltf")->GetComponentsInChildren<Mesh>();
    for (const auto& mesh : wallMeshes)
        wall->AddComponent(mesh);
    //wall->AddChild(std::static_pointer_cast<Node>(wallNode->Clone()));
    wall->SetScale(glm::vec3(0.45));
    //std::shared_ptr<Wall> wall(new Wall);
    //static auto mesh = CubeMesh::Create("WallMesh", glm::vec3(1.f));
    //mesh->GetMaterial(0)->SetDiffuse(glm::vec3(1, 0.2, 0.2));
    //wall->SetComponent(mesh);
    return wall;
}