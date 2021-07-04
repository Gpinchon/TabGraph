/*
* @Author: gpinchon
* @Date:   2020-06-18 13:31:08
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-01 22:30:41
*/
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Animations/Channel.hpp>
#include <Animations/Interpolator.hpp>
#include <Animations/Sampler.hpp>
#include <Core/Inherit.hpp>
#include <Core/Object.hpp>
#include <Core/Property.hpp>

#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
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
    std::vector<std::pair<Channel, Interpolator>> _channels;
    std::vector<Sampler> _samplers;
    float _currentTime { 0 };
};

}
