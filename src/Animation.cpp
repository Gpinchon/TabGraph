#include <SDL2/SDL_timer.h> // for SDL_GetTicks
#include <glm/common.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Animation.hpp"
#include "Node.hpp"
#include "BufferHelper.hpp"
#include "BufferAccessor.hpp"
#include "Callback.hpp"

Animation::Animation() : Object("")
{
	void (__thiscall Animation::* pFunc)() = &Animation::Play;
	_playCallback = Callback::Create(pFunc, this);
}

std::shared_ptr<Animation> Animation::Create()
{
	return std::shared_ptr<Animation>(new Animation);
}

std::vector<AnimationChannel> Animation::GetChannels() const
{
	return _channels;
}

std::vector<AnimationSampler> Animation::GetSamplers() const
{
	return _samplers;
}

void Animation::AddChannel(AnimationChannel channel)
{
	_channels.push_back(channel);
	_interpolators.resize(_channels.size());
}

void Animation::AddSampler(AnimationSampler sampler)
{
	_samplers.push_back(sampler);
}

template<typename T, typename U>
static inline T cubicSpline(T previousPoint, T previousTangent, T nextPoint, T nextTangent, U interpolationValue) {
	auto t = interpolationValue;
	auto t2 = t * t;
	auto t3 = t2 * t;
	return (2 * t3 - 3 * t2 + 1) * previousPoint + (t3 - 2 * t2 + t) * previousTangent + (-2 * t3 + 3 * t2) * nextPoint + (t3 - t2) * nextTangent;
}

template<typename T, typename U>
static inline T slerp(T v0, T v1, U t) {
    // Only unit quaternions are valid rotations.
    // Normalize to avoid undefined behavior.
    v0 = glm::normalize(v0);
    v1 = glm::normalize(v1);

    // Compute the cosine of the angle between the two vectors.
    auto dot = glm::dot(v0, v1);

    // If the dot product is negative, slerp won't take
    // the shorter path. Note that v1 and -v1 are equivalent when
    // the negation is applied to all four components. Fix by 
    // reversing one T.
    if (dot < 0.0f) {
        v1 = -v1;
        dot = -dot;
    }

    const auto DOT_THRESHOLD = 0.9995;
    if (dot > DOT_THRESHOLD) {
        // If the inputs are too close for comfort, linearly interpolate
        // and normalize the result.

        T result = v0 + t*(v1 - v0);
        result.normalize();
        return result;
    }

    // Since dot is in range [0, DOT_THRESHOLD], acos is safe
    auto theta_0 = acos(dot);        // theta_0 = angle between input vectors
    auto theta = theta_0*t;          // theta = angle between v0 and result
    auto sin_theta = sin(theta);     // compute this value only once
    auto sin_theta_0 = sin(theta_0); // compute this value only once

    auto s0 = cos(theta) - dot * sin_theta / sin_theta_0;  // == sin(theta_0 - theta) / sin(theta_0)
    auto s1 = sin_theta / sin_theta_0;

    return (s0 * v0) + (s1 * v1);
}

template<typename T, typename U>
static inline T InterpolateKeyFrame(T prev, T next, U interpolationValue, AnimationSampler::AnimationInterpolation interpolation, T previousTangent = T(), T nextTangent = T())
{
	switch (interpolation) {
		case AnimationSampler::Linear:
			return glm::mix(prev, next, interpolationValue);
		case AnimationSampler::CubicSpline:
			return cubicSpline(prev, previousTangent, next, nextTangent, interpolationValue);
		case AnimationSampler::Step:
			return prev;
	}
	return next;
}

template<typename U>
static inline glm::quat InterpolateKeyFrame(glm::quat prev, glm::quat next, U interpolationValue, AnimationSampler::AnimationInterpolation interpolation, glm::quat previousTangent = glm::quat(), glm::quat nextTangent = glm::quat())
{
	switch (interpolation) {
		case AnimationSampler::Linear:
			return slerp(prev, next, interpolationValue);
		case AnimationSampler::CubicSpline:
			return cubicSpline(prev, previousTangent, next, nextTangent, interpolationValue);
		case AnimationSampler::Step:
			return prev;
	}
	return next;
}

void Animation::Reset()
{
	for (auto interpolator : _interpolators) {
		_startTime = SDL_GetTicks();
		interpolator.SetPrevTime(0);
		interpolator.SetPrevKey(0);
	}
}

void Animation::Advance()
{
	_currentTime = (SDL_GetTicks() - _startTime) / 1000.0;
	bool animationPlayed(false);
	for (auto index = 0u; index < _channels.size(); ++index) {
		auto channel(_channels.at(index));
		auto interpolator(_interpolators.at(index));
		auto sampler(_samplers.at(channel.SamplerIndex()));
		auto t = _currentTime;
		auto max(BufferHelper::Get<float>(sampler.Timings(), sampler.Timings()->Count() - 1));
		auto min(BufferHelper::Get<float>(sampler.Timings(), 0));
		t = std::clamp(t, min, max);
        interpolator.SetPrevTime(t);
		size_t nextKey(0u);
		for (auto i = interpolator.PrevKey(); i < sampler.Timings()->Count(); ++i)
		{
			float timing(BufferHelper::Get<float>(sampler.Timings(), i));
			if (timing > t)
            {
                nextKey = std::clamp(size_t(i), size_t(0), sampler.Timings()->Count() - 1);
                break;
            }
		}
		interpolator.SetPrevKey(std::clamp(size_t(nextKey - 1), size_t(0), size_t(nextKey)));
		auto prevTime(BufferHelper::Get<float>(sampler.Timings(), interpolator.PrevKey()));
		auto nextTime(BufferHelper::Get<float>(sampler.Timings(), nextKey));
		auto keyDelta(nextTime - prevTime);
		auto interpolationValue(0.f);
		if (keyDelta != 0)
			interpolationValue = (t - prevTime) / keyDelta;
		switch (channel.Path()) {
			case AnimationChannel::Translation:
			{
				glm::vec3 current;
				if (sampler.Interpolation() == AnimationSampler::CubicSpline)
				{
					glm::vec3 prev				(BufferHelper::Get<glm::vec3>(sampler.KeyFrames(), interpolator.PrevKey() * 3 + 1));
					glm::vec3 prevOutputTangent	(BufferHelper::Get<glm::vec3>(sampler.KeyFrames(), interpolator.PrevKey() * 3 + 2));
					glm::vec3 nextInputTangent	(BufferHelper::Get<glm::vec3>(sampler.KeyFrames(), nextKey * 3 + 0));
					glm::vec3 next				(BufferHelper::Get<glm::vec3>(sampler.KeyFrames(), nextKey * 3 + 1));
					glm::vec3 prevTangent = keyDelta * prevOutputTangent;
    				glm::vec3 nextTangent = keyDelta * nextInputTangent;
    				current = InterpolateKeyFrame(prev, next, interpolationValue, sampler.Interpolation(), prevTangent, nextTangent);
				}
				else
				{
					glm::vec3 prev(BufferHelper::Get<glm::vec3>(sampler.KeyFrames(), interpolator.PrevKey()));
					glm::vec3 next(BufferHelper::Get<glm::vec3>(sampler.KeyFrames(), nextKey));
					current = InterpolateKeyFrame(prev, next, interpolationValue, sampler.Interpolation());
				}
				channel.Target()->SetPosition(current);
				break;
			}
			case AnimationChannel::Rotation:
			{
				glm::quat current;
				if (sampler.Interpolation() == AnimationSampler::CubicSpline)
				{
					glm::quat prev				(BufferHelper::Get<glm::quat>(sampler.KeyFrames(), interpolator.PrevKey() * 3 + 1));
					glm::quat prevOutputTangent	(BufferHelper::Get<glm::quat>(sampler.KeyFrames(), interpolator.PrevKey() * 3 + 2));
					glm::quat nextInputTangent	(BufferHelper::Get<glm::quat>(sampler.KeyFrames(), nextKey * 3 + 0));
					glm::quat next				(BufferHelper::Get<glm::quat>(sampler.KeyFrames(), nextKey * 3 + 1));
					glm::quat prevTangent = keyDelta * prevOutputTangent;
    				glm::quat nextTangent = keyDelta * nextInputTangent;
    				current = InterpolateKeyFrame(prev, next, interpolationValue, sampler.Interpolation(), prevTangent, nextTangent);
				}
				else
				{
					glm::quat prev(BufferHelper::Get<glm::quat>(sampler.KeyFrames(), interpolator.PrevKey()));
					glm::quat next(BufferHelper::Get<glm::quat>(sampler.KeyFrames(), nextKey));
					current = InterpolateKeyFrame(prev, next, interpolationValue, sampler.Interpolation());
				}
				channel.Target()->SetRotation(glm::normalize(current));
				break;
			}
			case AnimationChannel::Scale:
			{
				glm::vec3 current(channel.Target()->Scale());
				if (sampler.Interpolation() == AnimationSampler::CubicSpline)
				{
					glm::vec3 prev				(BufferHelper::Get<glm::vec3>(sampler.KeyFrames(), interpolator.PrevKey() * 3 + 1));
					glm::vec3 prevOutputTangent	(BufferHelper::Get<glm::vec3>(sampler.KeyFrames(), interpolator.PrevKey() * 3 + 2));
					glm::vec3 nextInputTangent	(BufferHelper::Get<glm::vec3>(sampler.KeyFrames(), nextKey * 3 + 0));
					glm::vec3 next				(BufferHelper::Get<glm::vec3>(sampler.KeyFrames(), nextKey * 3 + 1));
					glm::vec3 prevTangent = keyDelta * prevOutputTangent;
    				glm::vec3 nextTangent = keyDelta * nextInputTangent;
    				current = InterpolateKeyFrame(prev, next, interpolationValue, sampler.Interpolation(), prevTangent, nextTangent);
				}
				else
				{
					glm::vec3 prev(BufferHelper::Get<glm::vec3>(sampler.KeyFrames(), interpolator.PrevKey()));
					glm::vec3 next(BufferHelper::Get<glm::vec3>(sampler.KeyFrames(), nextKey));
					current = InterpolateKeyFrame(prev, next, interpolationValue, sampler.Interpolation());
				}
				channel.Target()->SetScale(current);
				break;
			}
			case AnimationChannel::Weights:
			{
				break;
			}
			case AnimationChannel::None:
				break;
		}

		animationPlayed = t < max;
	}
	if (!animationPlayed) {
		if (Repeat())
			Reset();
		else
			Stop();
	}
}

void Animation::Play()
{
	if (!Playing()) {
		for (auto sampler : _samplers) {
			sampler.Timings()->Load(false);
			sampler.KeyFrames()->Load(false);
		}
		_startTime = SDL_GetTicks();
		_playing = true;
	}
}

void Animation::Stop()
{
	Reset();
	_playing = false;
}

bool Animation::Playing() const
{
	return _playing;
}

void Animation::SetRepeat(bool repeat)
{
	_repeat = repeat;
}

bool Animation::Repeat() const
{
	return _repeat;
}