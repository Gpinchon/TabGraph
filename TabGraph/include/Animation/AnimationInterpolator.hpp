#pragma once

#include "Animation/AnimationSampler.hpp"
#include <Buffer/BufferHelper.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class AnimationInterpolator
{
public:
	float PrevTime() const;
	void SetPrevTime(float prevTime);
	long long unsigned NextKey() const;
	void SetNextKey(long long unsigned nextKey);
	long long unsigned PrevKey() const;
	void SetPrevKey(long long unsigned prevKey);
	template <typename T>
	T Interpolate(const AnimationSampler &sampler, float keyDelta, float interpolationValue);

private:
	float _prevTime { 0 };
	long long unsigned _prevKey { 0 };
	long long unsigned _nextKey { 0 };
};

template<typename T, typename U>
static inline T cubicSpline(T previousPoint, T prevTangent, T nextPoint, T nextTangent, U interpolationValue) {
	auto t = interpolationValue;
	auto t2 = t * t;
	auto t3 = t2 * t;
	return (2 * t3 - 3 * t2 + 1) * previousPoint + (t3 - 2 * t2 + t) * prevTangent + (-2 * t3 + 3 * t2) * nextPoint + (t3 - t2) * nextTangent;
}

template <typename T>
inline T AnimationInterpolator::Interpolate(const AnimationSampler &sampler, float delta, float interpolationValue)
{
	if (sampler.Interpolation() == AnimationSampler::CubicSpline)
	{
		T prev				(BufferHelper::Get<T>(sampler.KeyFrames(), PrevKey() * 3 + 1));
		T prevOutputTangent	(BufferHelper::Get<T>(sampler.KeyFrames(), PrevKey() * 3 + 2));
		T nextInputTangent	(BufferHelper::Get<T>(sampler.KeyFrames(), NextKey() * 3 + 0));
		T next				(BufferHelper::Get<T>(sampler.KeyFrames(), NextKey() * 3 + 1));
		T prevTangent = delta * prevOutputTangent;
    	T nextTangent = delta * nextInputTangent;
		return cubicSpline(prev, prevTangent, next, nextTangent, interpolationValue);
	}
	T prev(BufferHelper::Get<T>(sampler.KeyFrames(), PrevKey()));
	T next(BufferHelper::Get<T>(sampler.KeyFrames(), NextKey()));
	switch (sampler.Interpolation())
	{
		case AnimationSampler::Linear: {
			if constexpr (std::is_same_v<T, glm::quat>)
				return glm::slerp(prev, next, interpolationValue); 
			return glm::mix(prev, next, interpolationValue);
		}
		case AnimationSampler::Step:
			return prev;
		default:
			break;
	}
	return next;
}
