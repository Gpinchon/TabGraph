/*
* @Author: gpinchon
* @Date:   2021-01-08 17:02:47
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-19 14:29:40
*/
#pragma once

#include <Animation/AnimationSampler.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class AnimationInterpolator {
public:
    float PrevTime() const;
    void SetPrevTime(float prevTime);
    long long unsigned NextKey() const;
    void SetNextKey(long long unsigned nextKey);
    long long unsigned PrevKey() const;
    void SetPrevKey(long long unsigned prevKey);
    template <typename T>
    T Interpolate(const AnimationSampler& sampler, float keyDelta, float interpolationValue);

private:
    float _prevTime { 0 };
    long long unsigned _prevKey { 0 };
    long long unsigned _nextKey { 0 };
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
inline T AnimationInterpolator::Interpolate(const AnimationSampler& sampler, float delta, float interpolationValue)
{
    if (sampler.Interpolation() == AnimationSampler::AnimationInterpolation::CubicSpline) {
        T prev(sampler.KeyFrames()->Get<T>(PrevKey() * 3 + 1)); // BufferHelper::Get<T>(sampler.KeyFrames(), PrevKey() * 3 + 1));
        T prevOutputTangent(sampler.KeyFrames()->Get<T>(PrevKey() * 3 + 2)); //BufferHelper::Get<T>(sampler.KeyFrames(), PrevKey() * 3 + 2));
        T nextInputTangent(sampler.KeyFrames()->Get<T>(NextKey() * 3 + 0)); //BufferHelper::Get<T>(sampler.KeyFrames(), NextKey() * 3 + 0));
        T next(sampler.KeyFrames()->Get<T>(NextKey() * 3 + 0)); //BufferHelper::Get<T>(sampler.KeyFrames(), NextKey() * 3 + 1));
        T prevTangent = delta * prevOutputTangent;
        T nextTangent = delta * nextInputTangent;
        return cubicSpline(prev, prevTangent, next, nextTangent, interpolationValue);
    }
    T prev(sampler.KeyFrames()->Get<T>(PrevKey())); // BufferHelper::Get<T>(sampler.KeyFrames(), PrevKey()));
    T next(sampler.KeyFrames()->Get<T>(NextKey())); //BufferHelper::Get<T>(sampler.KeyFrames(), NextKey()));
    switch (sampler.Interpolation()) {
    case AnimationSampler::AnimationInterpolation::Linear: {
        if constexpr (std::is_same_v<T, glm::quat>)
            return glm::slerp(prev, next, interpolationValue);
        return glm::mix(prev, next, interpolationValue);
    }
    case AnimationSampler::AnimationInterpolation::Step:
        return prev;
    default:
        break;
    }
    return next;
}
