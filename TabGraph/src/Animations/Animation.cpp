/*
* @Author: gpinchon
* @Date:   2020-06-18 13:31:08
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-06-30 19:51:36
*/
#include <Animations/Animation.hpp>
#include <Buffer/Accessor.hpp>
#include <Buffer/View.hpp>
#include <Debug.hpp>
#include <Nodes/Node.hpp>
#include <Shapes/Mesh/Mesh.hpp>

#include <glm/common.hpp>
#include <glm/gtc/quaternion.hpp>
#include <algorithm>


namespace TabGraph::Animations {
Animation::Animation()
    : Inherit()
{
    static size_t s_animationNbr = 0;
    SetName("Animation_" + std::to_string(++s_animationNbr));
}

void Animation::Reset()
{
    _currentTime = 0;
    for (auto & channelPair : _channels) {
        auto& interpolator{ channelPair.second };
        interpolator.prevTime = 0;
        interpolator.nextKey = 0;
        interpolator.prevKey = 0;
    }
}

void Animation::Advance(float delta)
{
    if (!GetPlaying()) return;
    _currentTime += delta;
    bool animationPlayed(false);
    for (auto& channelPair : _channels) {
        auto& channel{ channelPair.first };
        auto& interpolator{ channelPair.second };
        auto sampler(_samplers.at(channel.samplerIndex));
        auto t = _currentTime;
        auto maxT = sampler.timings->Get<float>(sampler.timings->GetCount() - 1);
        auto minT = sampler.timings->Get<float>(0);
        t = std::clamp(t, minT, maxT);
        interpolator.prevTime = t;
        interpolator.nextKey = 0;
        for (auto i = interpolator.prevKey; i < sampler.timings->GetCount(); ++i) {
            float timing(sampler.timings->Get<float>(i));
            if (timing > t) {
                interpolator.nextKey = std::clamp(size_t(i), size_t(0), sampler.timings->GetCount() - 1);
                break;
            }
        }
        interpolator.prevKey = std::clamp(size_t(interpolator.nextKey - 1), size_t(0), size_t(interpolator.nextKey));
        auto prevTime(sampler.timings->Get<float>(interpolator.prevKey));
        auto nextTime(sampler.timings->Get<float>(interpolator.nextKey));
        auto keyDelta(nextTime - prevTime);
        auto interpolationValue(0.f);
        if (keyDelta != 0)
            interpolationValue = (t - prevTime) / keyDelta;
        switch (channel.path) {
        case Channel::Path::Translation: {
            glm::vec3 current = interpolator.Interpolate<glm::vec3>(sampler, keyDelta, interpolationValue);
            channel.target->SetLocalPosition(current);
            break;
        }
        case Channel::Path::Rotation: {
            glm::quat current = interpolator.Interpolate<glm::quat>(sampler, keyDelta, interpolationValue);
            channel.target->SetLocalRotation(glm::normalize(current));
            break;
        }
        case Channel::Path::Scale: {
            glm::vec3 current(interpolator.Interpolate<glm::vec3>(sampler, keyDelta, interpolationValue));
            channel.target->SetLocalScale(current);
            break;
        }
        case Channel::Path::Weights: {
            /*auto mesh(std::dynamic_pointer_cast<Mesh>(channel.Target()));
                    if (mesh == nullptr) {
                        debugLog(channel.Target()->Name() + " is not a Mesh");
                        break;
                    }
                    auto weights(mesh->Weights());
                    if (sampler.Interpolation() == Sampler::CubicSpline)
                    {
                    }
                    else
                    {
                        for (auto i = 0u; i < weights->Count(); ++i) {
                            float prev(BufferHelper::Get<float>(sampler.KeyFrames(), interpolator.prevKey + i));
                            float next(BufferHelper::Get<float>(sampler.KeyFrames(), nextKey + i));
                            auto current = InterpolateKeyFrame(prev, next, interpolationValue, sampler.Interpolation());
                            BufferHelper::Set(weights, i, current);
                        }
                    }*/
            break;
        }
        case Channel::Path::None:
            break;
        }
        animationPlayed |= t < maxT;
    }
    if (!animationPlayed) {
        if (GetRepeat())
            Reset();
        else
            Stop();
    }
}

void Animation::Play()
{
    if (!GetPlaying()) {
        _currentTime = 0;
        _SetPlaying(true);
    }
}

void Animation::Stop()
{
    Reset();
    _SetPlaying(false);
}
}
