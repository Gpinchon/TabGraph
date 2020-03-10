#include <SDL2/SDL_timer.h> // for SDL_GetTicks
#include <glm/common.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Animation.hpp"
#include "Events.hpp"
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
T cubicSpline(T previousPoint, T previousTangent, T nextPoint, T nextTangent, U interpolationValue) {
	auto t = interpolationValue;
	auto t2 = t * t;
	auto t3 = t2 * t;
	return (2 * t3 - 3 * t2 + 1) * previousPoint + (t3 - 2 * t2 + t) * previousTangent + (-2 * t3 + 3 * t2) * nextPoint + (t3 - t2) * nextTangent;
}

/*cubicSpline(prevKey, nextKey, output, keyDelta, t, )
    {
        // : Count of components (4 in a quaternion).
        // Scale by 3, because each output entry consist of two tangents and one data-point.
        const prevIndex = prevKey *  * 3;
        const nextIndex = nextKey *  * 3;
        const A = 0;
        const V = 1 * ;
        const B = 2 * ;

        const result = new glMatrix.ARRAY_TYPE();
        const tSq = t ** 2;
        const tCub = t ** 3;

        // We assume that the components in output are laid out like this: in-tangent, point, out-tangent.
        // https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#appendix-c-spline-interpolation
        for(let i = 0; i < ; ++i)
        {
            const v0 = output[prevIndex + i + V];
            const a = keyDelta * output[nextIndex + i + A];
            const b = keyDelta * output[prevIndex + i + B];
            const v1 = output[nextIndex + i + V];

            result[i] = ((2*tCub - 3*tSq + 1) * v0) + ((tCub - 2*tSq + t) * b) + ((-2*tCub + 3*tSq) * v1) + ((tCub - tSq) * a);
        }

        return result;
    }*/

template<typename T, typename U>
T InterpolateKeyFrame(T prev, T next, U interpolationValue, AnimationSampler::AnimationInterpolation interpolation, T previousTangent = T(), T nextTangent = T())
{
	switch (interpolation) {
		case AnimationSampler::Linear:
			return glm::mix(prev, next, interpolationValue);
		case AnimationSampler::CubicSpline:
			return cubicSpline(prev, previousTangent, next, nextTangent, interpolationValue);
		case AnimationSampler::Step:
			return interpolationValue < 0.5 ? prev : next;
	}
	return next;
}

//#include <glm/gtx/string_cast.hpp>
//#include <iostream>
/*
template<typename U>
glm::quat InterpolateKeyFrame(glm::quat prev, glm::quat next, U interpolationValue, AnimationSampler::AnimationInterpolation interpolation, glm::quat previousTangent = glm::quat(), glm::quat nextTangent = glm::quat())
{
	//std::cout << glm::to_string(prev) << " " << glm::to_string(next) << std::endl;
	glm::quat result = prev;
	switch (interpolation) {
		case AnimationSampler::Linear:
			result = glm::mix(prev, next, interpolationValue);
			break;
		case AnimationSampler::CubicSpline:
			result = cubicSpline(prev, previousTangent, next, nextTangent, interpolationValue);
			break;
		case AnimationSampler::Step:
			result = interpolationValue < 0.5 ? prev : next;
			break;
	}
	result = glm::normalize(result);
	return result;
}
*/
void Animation::Reset()
{
	for (auto interpolator : _interpolators) {
		_startTime = SDL_GetTicks();
		interpolator.SetPrevTime(0);
		interpolator.SetPrevKey(0);
	}
}

void Animation::Play()
{
	if (!Playing()) {
		for (auto sampler : _samplers) {
			sampler.Timings()->Load(false);
			sampler.KeyFrames()->Load(false);
		}
		Events::AddRefreshCallback(_playCallback);
		_startTime = SDL_GetTicks();
		_playing = true;
		return;
	}
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
			if (timing >= t)
            {
                nextKey = std::clamp(size_t(i), size_t(0), sampler.Timings()->Count() - 1);
                break;
            }
		}
		interpolator.SetPrevKey(std::clamp(size_t(nextKey - 1), size_t(0), size_t(nextKey)));
		auto prevTime(BufferHelper::Get<float>(sampler.Timings(), interpolator.PrevKey()));
		auto nextTime(BufferHelper::Get<float>(sampler.Timings(), nextKey));
		auto keyDelta(nextTime - prevTime);
		auto interpolationValue((t - prevTime) / keyDelta);
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

void Animation::Stop()
{
	if (Playing())
		Events::RemoveRefreshCallback(_playCallback);
	_playing = 0;
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
