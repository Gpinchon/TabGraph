/*
* @Author: gpinchon
* @Date:   2020-06-18 13:31:08
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-19 13:07:33
*/
#include "Animation/Animation.hpp"
#include "Buffer/BufferAccessor.hpp"
#include "Buffer/BufferHelper.hpp"
#include "Callback.hpp"
#include "Debug.hpp"
#include "Mesh/Mesh.hpp"
#include "Node.hpp"
#include "Transform.hpp"

#include <glm/common.hpp>
#include <glm/gtc/quaternion.hpp>

size_t animationNbr = 0;

Animation::Animation()
    : Component("Animation_" + std::to_string(animationNbr))
{
    //_playCallback = Callback<void()>::Create(std::bind(&Animation::Play, this));
    animationNbr++;
}

std::vector<AnimationChannel> &Animation::GetChannels()
{
    return _channels;
}

std::vector<AnimationSampler> &Animation::GetSamplers()
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

void Animation::Reset()
{
    _currentTime = 0;
    for (auto interpolator : _interpolators) {
        interpolator.SetPrevTime(0);
        interpolator.SetNextKey(0);
        interpolator.SetPrevKey(0);
    }
}

void Animation::_FixedUpdateCPU(float delta)
{
    if (!Playing())
        return;
    _currentTime += delta;
    bool animationPlayed(false);
    for (auto index = 0u; index < _channels.size(); ++index) {
        auto channel(_channels.at(index));
        auto interpolator(_interpolators.at(index));
        auto sampler(_samplers.at(channel.SamplerIndex()));
        auto t = _currentTime;
        auto maxT = BufferHelper::Get<float>(sampler.Timings(), sampler.Timings()->Count() - 1);
        auto minT = BufferHelper::Get<float>(sampler.Timings(), 0);
        t = std::clamp(t, minT, maxT);
        interpolator.SetPrevTime(t);
        interpolator.SetNextKey(0u);
        for (auto i = interpolator.PrevKey(); i < sampler.Timings()->Count(); ++i) {
            float timing(BufferHelper::Get<float>(sampler.Timings(), i));
            if (timing > t) {
                interpolator.SetNextKey(std::clamp(size_t(i), size_t(0), sampler.Timings()->Count() - 1));
                break;
            }
        }
        interpolator.SetPrevKey(std::clamp(size_t(interpolator.NextKey() - 1), size_t(0), size_t(interpolator.NextKey())));
        auto prevTime(BufferHelper::Get<float>(sampler.Timings(), interpolator.PrevKey()));
        auto nextTime(BufferHelper::Get<float>(sampler.Timings(), interpolator.NextKey()));
        auto keyDelta(nextTime - prevTime);
        auto interpolationValue(0.f);
        if (keyDelta != 0)
            interpolationValue = (t - prevTime) / keyDelta;
        switch (channel.Path()) {
        case AnimationChannel::Channel::Translation: {
            glm::vec3 current = interpolator.Interpolate<glm::vec3>(sampler, keyDelta, interpolationValue);
            channel.Target()->SetPosition(current);
            break;
        }
        case AnimationChannel::Channel::Rotation: {
            glm::quat current = interpolator.Interpolate<glm::quat>(sampler, keyDelta, interpolationValue);
            channel.Target()->SetRotation(glm::normalize(current));
            break;
        }
        case AnimationChannel::Channel::Scale: {
            glm::vec3 current(interpolator.Interpolate<glm::vec3>(sampler, keyDelta, interpolationValue));
            channel.Target()->SetScale(current);
            break;
        }
        case AnimationChannel::Channel::Weights: {
            /*auto mesh(std::dynamic_pointer_cast<Mesh>(channel.Target()));
                if (mesh == nullptr) {
                    debugLog(channel.Target()->Name() + " is not a Mesh");
                    break;
                }
                auto weights(mesh->Weights());
                if (sampler.Interpolation() == AnimationSampler::CubicSpline)
                {
                }
                else
                {
                    for (auto i = 0u; i < weights->Count(); ++i) {
                        float prev(BufferHelper::Get<float>(sampler.KeyFrames(), interpolator.PrevKey() + i));
                        float next(BufferHelper::Get<float>(sampler.KeyFrames(), nextKey + i));
                        auto current = InterpolateKeyFrame(prev, next, interpolationValue, sampler.Interpolation());
                        BufferHelper::Set(weights, i, current);
                    }
                }*/
            break;
        }
        case AnimationChannel::Channel::None:
            break;
        }
        animationPlayed |= t < maxT;
    }
    if (!animationPlayed) {
        if (Repeat())
            Reset();
        else
            Stop();
    }
}

void Animation::Advance()
{
}

void Animation::Play()
{
    if (!Playing()) {
        for (auto sampler : _samplers) {
            sampler.Timings()->GetBufferView()->GetBuffer()->LoadCPU();
            sampler.KeyFrames()->GetBufferView()->GetBuffer()->LoadCPU();
        }
        _currentTime = 0;
        _playing = true;
        SetNeedsFixedUpdateCPU(true);
    }
}

void Animation::Stop()
{
    Reset();
    _playing = false;
    SetNeedsFixedUpdateCPU(false);
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
