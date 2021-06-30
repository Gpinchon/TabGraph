/*
* @Author: gpinchon
* @Date:   2020-06-18 13:31:08
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-06-30 19:53:18
*/
#pragma once
#include <Animations/Channel.hpp>
#include <Animations/Interpolator.hpp>
#include <Animations/Sampler.hpp>
#include <Core/Inherit.hpp>
#include <Core/Object.hpp>
#include <Event/Signal.hpp>
#include <Property.hpp>

#include <chrono>

namespace TabGraph::Animations {
class Animation : public Core::Inherit<Core::Object, Animation> {
public:
    READONLYPROPERTY(bool, Playing, false);
    PROPERTY(bool, Repeat, false);
public:
    Animation();

    inline auto& GetChannels()
    {
        return _channels;
    }
    inline auto& GetSamplers()
    {
        return _samplers;
    }
    inline void AddChannel(const Channel& channel, const Interpolator& interpolator = {})
    {
        _channels.push_back({ channel, interpolator });
    }
    inline void AddSampler(const Sampler& sampler)
    {
        _samplers.push_back(sampler);
    }

    /** @brief Start playing the animation */
    void Play();
    /** @brief Advance the animation */
    void Advance(float delta);
    /** @brief Stop the animation */
    void Stop();
    /** @brief Resets the animation to the beginning */
    void Reset();

private:
    /*virtual void _Replace(const std::shared_ptr<Component> oldComponent, const std::shared_ptr<Component> newComponent)
    {
        for (auto& channel : GetChannels()) {
            if (channel.Target() == oldComponent)
                channel.SetTarget(std::static_pointer_cast<Node>(newComponent));
        }
    };*/
    std::vector<std::pair<Channel, Interpolator>> _channels;
    std::vector<Sampler> _samplers;
    float _currentTime { 0 };
};

}
