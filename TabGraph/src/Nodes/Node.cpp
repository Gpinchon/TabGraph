/*
* @Author: gpinchon
* @Date:   2021-06-19 15:01:38
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-06-26 23:39:40
*/

#include <Nodes/Node.hpp>

#include <glm/gtx/transform.hpp>

namespace TabGraph::Nodes {
Node::Node()
    : Inherit()
{
    static auto s_nodeNbr { 0u };
    SetName("Node_" + std::to_string(++s_nodeNbr));
}
glm::mat4 Node::GetLocalTranslationMatrix()
{
    if (_positionChanged) {
        _localTranslationMatrix = glm::translate(GetLocalPosition());
        _positionChanged = false;
    }
    return _localTranslationMatrix;
}
glm::mat4 Node::GetLocalRotationMatrix()
{
    if (_rotationChanged) {
        _localRotationMatrix = glm::mat4_cast(GetLocalRotation());
        _rotationChanged = false;
    }
    return _localRotationMatrix;
}
glm::mat4 Node::GetLocalScaleMatrix()
{
    if (_scaleChanged) {
        _localScaleMatrix = glm::scale(GetLocalScale());
        _scaleChanged = false;
    }
    return _localScaleMatrix;
}
}
