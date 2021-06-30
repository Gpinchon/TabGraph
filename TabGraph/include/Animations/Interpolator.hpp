/*
* @Author: gpinchon
* @Date:   2021-01-08 17:02:47
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-06-30 19:43:39
*/
#pragma once

#include <Animations/Sampler.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace TabGraph::Animations {
struct Interpolator {
    float prevTime { 0 };
    long long unsigned prevKey { 0 };
    long long unsigned nextKey { 0 };
    template <typename T>
    T Interpolate(const Sampler& sampler, float keyDelta, float interpolationValue);
};

template <typename T, typename U>
static inline T cubicSpline(T previousPoint, T prevTangent, T nextPoint, T nextTangent, U interpolationValue)
{
    auto t = interpolationValue;
    auto t2 = t * t;
    auto t3 = t2 * t;
    return (2 * t3 - 3 * t2 + 1) * previousPoint + (t3 - 2 * t2 + t) * prevTangent + (-2 * t3 + 3 * t2) * nextPoint + (t3 - t2) * nextTangent;
}

template <typename T>
inline T Interpolator::Interpolate(const Sampler& sampler, float delta, float interpolationValue)
{
    if (sampler.interpolation == Sampler::Interpolation::CubicSpline) {
        T prev(sampler.keyFrames->Get<T>(prevKey * 3 + 1));
        T prevOutputTangent(sampler.keyFrames->Get<T>(prevKey * 3 + 2));
        T nextInputTangent(sampler.keyFrames->Get<T>(nextKey * 3 + 0));
        T next(sampler.keyFrames->Get<T>(nextKey * 3 + 0));
        T prevTangent = delta * prevOutputTangent;
        T nextTangent = delta * nextInputTangent;
        return cubicSpline(prev, prevTangent, next, nextTangent, interpolationValue);
    }
    T prev(sampler.keyFrames->Get<T>(prevKey));
    T next(sampler.keyFrames->Get<T>(nextKey));
    switch (sampler.interpolation) {
    case Sampler::Interpolation::Linear: {
        if constexpr (std::is_same_v<T, glm::quat>)
            return glm::slerp(prev, next, interpolationValue);
        return glm::mix(prev, next, interpolationValue);
    }
    case Sampler::Interpolation::Step:
        return prev;
    default:
        break;
    }
    return next;
}
}