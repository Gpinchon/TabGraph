#pragma once
#include "Animation/AnimationChannel.hpp"
#include "Animation/AnimationInterpolator.hpp"
#include "Animation/AnimationSampler.hpp"
#include "Object.hpp"
#include <chrono>

template <typename Signature>
class Callback;

class Animation : public Object {
public:
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

protected:
    Animation();

private:
    std::vector<AnimationInterpolator> _interpolators;
    std::vector<AnimationChannel> _channels;
    std::vector<AnimationSampler> _samplers;
    bool _playing { false };
    bool _repeat { false };
    float _startTime { 0 };
    float _currentTime { 0 };
    std::shared_ptr<Callback<void()>> _playCallback { nullptr };
};
