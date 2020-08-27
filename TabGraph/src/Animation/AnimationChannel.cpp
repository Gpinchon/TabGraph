#include "Animation/AnimationChannel.hpp"

std::shared_ptr<Node> AnimationChannel::Target() const
{
	return _target.lock();
}

void AnimationChannel::SetTarget(std::shared_ptr<Node> target)
{
	_target = target;
}

size_t AnimationChannel::SamplerIndex() const
{
	return _samplerIndex;
}

void AnimationChannel::SetSamplerIndex(size_t index)
{
	_samplerIndex = index;
}

AnimationChannel::Channel AnimationChannel::Path() const
{
	return _path;
}

void AnimationChannel::SetPath(Channel path)
{
	_path = path;
}
