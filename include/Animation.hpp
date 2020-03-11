#pragma once
#include <chrono>
#include "Object.hpp"
#include "AnimationChannel.hpp"
#include "AnimationInterpolator.hpp"
#include "AnimationSampler.hpp"

class Callback;

class Animation : public Object
{
public:
	static std::shared_ptr<Animation> Create();
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
	float _currentTime{ 0 };
	std::shared_ptr<Callback> _playCallback { nullptr };
};
