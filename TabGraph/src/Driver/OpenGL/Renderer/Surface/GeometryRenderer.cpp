/*
* @Author: gpinchon
* @Date:   2021-03-22 20:52:18
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-12 18:39:32
*/

#include "Driver/OpenGL/Renderer/Surface/GeometryRenderer.hpp"
#include "Buffer/BufferAccessor.hpp"
#include "Driver/OpenGL/Buffer.hpp"
#include "Driver/OpenGL/VertexArray.hpp"
#include "Surface/Geometry.hpp"

#include <GL/glew.h>

auto GLDrawingMode(Geometry::DrawingMode drawingMode)
{
    static std::array<GLenum, (size_t)Geometry::DrawingMode::MaxValue> s_drawingModeLUT = {
        GL_POINTS, //Points
        GL_LINES, //Lines
        GL_LINE_STRIP, //LineStrip
        GL_LINE_LOOP, //LineLoop
        GL_POLYGON, //Polygon
        GL_TRIANGLES, //Triangles
        GL_TRIANGLE_STRIP, //TriangleStrip
        GL_TRIANGLE_FAN, //TriangleFan
        GL_QUADS, //Quads
        GL_QUAD_STRIP //QuadStrip
    };
    return s_drawingModeLUT.at(int(drawingMode));
}

static inline auto GLComponentType(BufferAccessor::ComponentType type)
{
    static std::array<GLenum, (size_t)BufferAccessor::ComponentType::MaxValue> s_compTypeLUT = {
        GL_BYTE, //Int8
        GL_UNSIGNED_BYTE, //Uint8
        GL_SHORT, //Int16
        GL_UNSIGNED_SHORT, //Uint16
        GL_UNSIGNED_INT, //Uint32
        GL_FLOAT //Float32
    };
    return s_compTypeLUT.at(int(type));
}

namespace Renderer {
GeometryRenderer::GeometryRenderer(Geometry& geometry)
    : _geometry(geometry)
{
}

GeometryRenderer::~GeometryRenderer()
{
}

void GeometryRenderer::OnFrameBegin(uint32_t frameNbr, float delta)
{
}

void GeometryRenderer::Render(bool doubleSided)
{
    if (_vao == nullptr) {
        _vao = std::make_unique<VertexArray>();
        _vao->Bind()
            .BindAccessor(_geometry.Accessor(Geometry::AccessorKey::Position), 0)
            .BindAccessor(_geometry.Accessor(Geometry::AccessorKey::Normal), 1)
            .BindAccessor(_geometry.Accessor(Geometry::AccessorKey::Tangent), 2)
            .BindAccessor(_geometry.Accessor(Geometry::AccessorKey::TexCoord_0), 3)
            .BindAccessor(_geometry.Accessor(Geometry::AccessorKey::TexCoord_1), 4)
            .BindAccessor(_geometry.Accessor(Geometry::AccessorKey::Color_0), 5)
            .BindAccessor(_geometry.Accessor(Geometry::AccessorKey::Joints_0), 6)
            .BindAccessor(_geometry.Accessor(Geometry::AccessorKey::Weights_0), 7)
            .Done();
        if (_geometry.Indices() != nullptr)
            _geometry.Indices()->GetBufferView()->Load();
    }
    if (doubleSided)
        glDisable(GL_CULL_FACE);
    else
        glEnable(GL_CULL_FACE);
    _vao->Bind();
    if (_geometry.Indices() != nullptr) {
        auto drawingMode { GLDrawingMode(_geometry.GetDrawingMode()) };
        auto compType { GLComponentType(_geometry.Indices()->GetComponentType()) };
        auto bufferView(_geometry.Indices()->GetBufferView());
        auto byteOffset(_geometry.Indices()->GetByteOffset());
        bufferView->Bind();
        glDrawElements(drawingMode, _geometry.Indices()->GetCount(), compType, BUFFER_OFFSET(byteOffset));
        bufferView->Done();
    } else if (auto accessor(_geometry.Accessor(Geometry::AccessorKey::Position)); accessor != nullptr) {
        auto drawingMode { GLDrawingMode(_geometry.GetDrawingMode()) };
        glDrawArrays(drawingMode, 0, accessor->GetCount());
    }
    VertexArray::BindNone();
}

void GeometryRenderer::OnFrameEnd(uint32_t frameNbr, float delta)
{
}
void GeometryRenderer::Render(const::Renderer::Options& options, const glm::mat4& parentTransform, const glm::mat4& parentLastTransform)
{
    Render();
}
};
