/*
* @Author: gpinchon
* @Date:   2021-03-22 20:52:18
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-28 07:54:34
*/

#include "Driver/OpenGL/Renderer/GeometryRenderer.hpp"
#include "Buffer/BufferAccessor.hpp"
#include "Driver/OpenGL/Buffer.hpp"
#include "Driver/OpenGL/VertexArray.hpp"
#include "Mesh/Geometry.hpp"

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
void OnFrameBegin(std::shared_ptr<Geometry> geometry, uint32_t frameNbr, float delta)
{
    geometry->GetRenderer().OnFrameBegin(frameNbr, delta);
}
void Render(std::shared_ptr<Geometry> geometry, bool doubleSided)
{
    geometry->GetRenderer().Render(doubleSided);
}
void OnFrameEnd(std::shared_ptr<Geometry> geometry, uint32_t frameNbr, float delta)
{
    geometry->GetRenderer().OnFrameEnd(frameNbr, delta);
}

GeometryRenderer::Impl::~Impl()
{
}
void GeometryRenderer::Impl::OnFrameBegin(Geometry& geometry, uint32_t frameNbr, float delta)
{
}
void GeometryRenderer::Impl::Render(Geometry& geometry, bool doubleSided)
{
    if (_vao == nullptr) {
        _vao = std::make_unique<VertexArray>();
        _vao->Bind()
            .BindAccessor(geometry.Accessor(Geometry::AccessorKey::Position), 0)
            .BindAccessor(geometry.Accessor(Geometry::AccessorKey::Normal), 1)
            .BindAccessor(geometry.Accessor(Geometry::AccessorKey::Tangent), 2)
            .BindAccessor(geometry.Accessor(Geometry::AccessorKey::TexCoord_0), 3)
            .BindAccessor(geometry.Accessor(Geometry::AccessorKey::TexCoord_1), 4)
            .BindAccessor(geometry.Accessor(Geometry::AccessorKey::Color_0), 5)
            .BindAccessor(geometry.Accessor(Geometry::AccessorKey::Joints_0), 6)
            .BindAccessor(geometry.Accessor(Geometry::AccessorKey::Weights_0), 7)
            .Done();
        if (geometry.Indices() != nullptr)
            geometry.Indices()->GetBufferView()->Load();
    }
    if (doubleSided)
        glDisable(GL_CULL_FACE);
    else
        glEnable(GL_CULL_FACE);
    _vao->Bind();
    if (geometry.Indices() != nullptr) {
        auto drawingMode { GLDrawingMode(geometry.GetDrawingMode()) };
        auto compType { GLComponentType(geometry.Indices()->GetComponentType()) };
        auto bufferView(geometry.Indices()->GetBufferView());
        auto byteOffset(geometry.Indices()->GetByteOffset());
        bufferView->Bind();
        glDrawElements(drawingMode, geometry.Indices()->GetCount(), compType, BUFFER_OFFSET(byteOffset));
        bufferView->Done();
    } else if (auto accessor(geometry.Accessor(Geometry::AccessorKey::Position)); accessor != nullptr) {
        auto drawingMode { GLDrawingMode(geometry.GetDrawingMode()) };
        glDrawArrays(drawingMode, 0, accessor->GetCount());
    }
    VertexArray::BindNone();
}
void GeometryRenderer::Impl::OnFrameEnd(Geometry& geometry, uint32_t frameNbr, float delta)
{
}
};
