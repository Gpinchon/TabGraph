/*
* @Author: gpinchon
* @Date:   2020-06-18 13:31:08
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-18 18:43:43
*/
#pragma once
#include "Animation/AnimationChannel.hpp"
#include "Animation/AnimationInterpolator.hpp"
#include "Animation/AnimationSampler.hpp"
#include "Component.hpp"
#include "Event/Signal.hpp"

#include <chrono>

template <typename Signature>
class Callback;

class Animation : public Component {
public:
    Animation();
    Animation(const Animation &other);
    AnimationChannel GetChannel(AnimationChannel::Channel channel);

    std::vector<AnimationChannel> &GetChannels();
    std::vector<AnimationSampler> &GetSamplers();
    void AddChannel(AnimationChannel);
    void AddSampler(AnimationSampler);
    /** start playing the animation */
    void Play();
    /** advance the animation */
    void Advance(float delta);
    /** stop the animation */
    void Stop();
    /** true if the animation is currently playing */
    bool Playing() const;
    void SetRepeat(bool);
    bool Repeat() const;
    void Reset();

private:
    virtual std::shared_ptr<Component> _Clone() override {
        return Component::Create<Animation>(*this);
    }
    virtual void _Replace(const std::shared_ptr<Component> oldComponent, const std::shared_ptr<Component> newComponent) {
        for (auto &channel : GetChannels()) {
            if (channel.Target() == oldComponent)
                channel.SetTarget(std::static_pointer_cast<Node>(newComponent));
        }
    };
    void _OnFixedUpdate(float delta);
    std::vector<AnimationInterpolator> _interpolators;
    std::vector<AnimationChannel> _channels;
    std::vector<AnimationSampler> _samplers;
    bool _playing { false };
    bool _repeat { false };
    float _currentTime { 0 };
    float _animationDelta{ 0 };
    Signal<float>::ScoppedSlot _advanceSlot;
};
