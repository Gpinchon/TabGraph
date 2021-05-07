/*
* @Author: gpinchon
* @Date:   2021-04-29 18:56:46
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-01 20:10:58
*/
#pragma once

#include "Property.hpp"

#include <glm/vec4.hpp>
#include <memory>

struct TextureSampler {
    class Handle;
    enum class Wrap {
        Repeat,
        ClampToBorder,
        ClampToEdge,
        MirroredRepeat,
        MirroredClampToEdge,
        MaxValue
    };
    enum class Filter {
        Nearest,
        Linear,
        NearestMipmapLinear,
        NearestMipmapNearest,
        LinearMipmapLinear,
        LinearMipmapNearest,
        MaxValue
    };
    enum class CompareMode {
        None,
        CompareRefToTexture,
        MaxValue
    };
    enum class CompareFunc {
        LessEqual,
        GreaterEqual,
        Less,
        Greater,
        Equal,
        NotEqual,
        Always,
        Never,
        MaxValue
    };
private:
    class Impl;
    friend Impl;
    std::unique_ptr<Impl> _impl;

public:
    TextureSampler();
    ~TextureSampler();

    const Handle& GetHandle() const;

    Filter GetMagFilter() const;
    Filter GetMinFilter() const;
    float GetMinLOD() const;
    float GetMaxLOD() const;
    float GetLODBias() const;
    Wrap GetWrapS() const;
    Wrap GetWrapT() const;
    Wrap GetWrapR() const;
    CompareMode GetCompareMode() const;
    CompareFunc GetCompareFunc() const;
    float GetMaxAnisotropy() const;
    glm::vec4 GetBorderColor() const;

    void SetMagFilter(Filter value);
    void SetMinFilter(Filter value);
    void SetMinLOD(float value);
    void SetMaxLOD(float value);
    void SetLODBias(float value);
    void SetWrapS(Wrap value);
    void SetWrapT(Wrap value);
    void SetWrapR(Wrap value);
    void SetCompareMode(CompareMode value);
    void SetCompareFunc(CompareFunc value);
    void SetMaxAnisotropy(float value);
    void SetBorderColor(glm::vec4 value);
};
