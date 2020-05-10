#include "Animation/AnimationSampler.hpp"

AnimationSampler::AnimationSampler(std::shared_ptr<BufferAccessor> timings, std::shared_ptr<BufferAccessor> keyFrames)
{
	SetTimings(timings);
	SetKeyFrames(keyFrames);
}

std::shared_ptr<BufferAccessor> AnimationSampler::Timings() const
{
	return _timings;
}

void AnimationSampler::SetTimings(std::shared_ptr<BufferAccessor> input)
{
	_timings = input;
}

std::shared_ptr<BufferAccessor> AnimationSampler::KeyFrames() const
{
	return _keyFrames;
}

void AnimationSampler::SetKeyFrames(std::shared_ptr<BufferAccessor> keyFrames)
{
	_keyFrames = keyFrames;
}

AnimationSampler::AnimationInterpolation AnimationSampler::Interpolation() const
{
	return _interpolation;
}

void AnimationSampler::SetInterpolation(AnimationInterpolation interpolation)
{
	_interpolation = interpolation;
}
