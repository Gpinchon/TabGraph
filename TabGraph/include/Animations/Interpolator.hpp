/*
* @Author: gpinchon
* @Date:   2021-01-08 17:02:47
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-21 21:46:36
*/
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Animations/Channel.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Animations::Interpolator {
template <typename T, typename U>
static inline T cubicSpline(T previousPoint, T prevTangent, T nextPoint, T nextTangent, U interpolationValue)
{
    auto t = interpolationValue;
    auto t2 = t * t;
    auto t3 = t2 * t;
    return (2 * t3 - 3 * t2 + 1) * previousPoint + (t3 - 2 * t2 + t) * prevTangent + (-2 * t3 + 3 * t2) * nextPoint + (t3 - t2) * nextTangent;
}

template <typename T>
inline T Interpolate(const Channel<T>& channel, const size_t& prevKey, const size_t& nextKey, float keyDelta, float interpolationValue)
{
    if (channel.interpolation == Channel<T>::Interpolation::CubicSpline) {
        auto prev(channel.keyFrames.at(prevKey * 3 + 1).value);
        auto prevOutputTangent(channel.keyFrames.at(prevKey * 3 + 2).value);
        auto nextInputTangent(channel.keyFrames.at(nextKey * 3 + 0).value);
        auto next(channel.keyFrames.at(nextKey * 3 + 0).value);
        auto prevTangent = keyDelta * prevOutputTangent;
        auto nextTangent = keyDelta * nextInputTangent;
        return cubicSpline(prev, prevTangent, next, nextTangent, interpolationValue);
    }
    auto prev(channel.keyFrames.at(prevKey).value);
    auto next(channel.keyFrames.at(nextKey).value);
    switch (channel.interpolation) {
    case Channel<T>::Interpolation::Linear: {
        if constexpr (std::is_same_v<T, glm::quat>)
            return glm::slerp(prev, next, interpolationValue);
        return glm::mix(prev, next, interpolationValue);
    }
    case Channel<T>::Interpolation::Step:
        return prev;
    default:
        break;
    }
    return next;
}

}