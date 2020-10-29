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
#include <chrono>

template <typename Signature>
class Callback;

class Animation : public Component {
public:
    Animation();
    static std::shared_ptr<Animation> Create();
    AnimationChannel GetChannel(AnimationChannel::Channel channel);

    std::vector<AnimationChannel> GetChannels() const;
    std::vector<AnimationSampler> GetSamplers() const;
    void AddChannel(AnimationChannel);
    void AddSampler(AnimationSampler);
    /** start playing the animation */
    void Play();
    /** advance the animation */
    void Advance();
    /** stop the animation */
    void Stop();
    /** true if the animation is currently playing */
    bool Playing() const;
    void SetRepeat(bool);
    bool Repeat() const;
    void Reset();

private:
    virtual std::shared_ptr<Component> _Clone() const override {
        return tools::make_shared<Animation>(*this);
    }
    virtual void _LoadCPU() override {};
    virtual void _UnloadCPU() override {};
    virtual void _LoadGPU() override {};
    virtual void _UnloadGPU() override {};
    virtual void _UpdateCPU(float /*delta*/) override {};
    virtual void _UpdateGPU(float /*delta*/) override {};
    virtual void _FixedUpdateCPU(float /*delta*/) override;
    virtual void _FixedUpdateGPU(float /*delta*/) override {};
    std::vector<AnimationInterpolator> _interpolators;
    std::vector<AnimationChannel> _channels;
    std::vector<AnimationSampler> _samplers;
    bool _playing { false };
    bool _repeat { false };
    float _currentTime { 0 };
    std::shared_ptr<Callback<void()>> _playCallback { nullptr };
};
