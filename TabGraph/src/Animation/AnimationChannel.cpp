/*
* @Author: gpinchon
* @Date:   2021-01-04 09:42:57
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:46:33
*/
#include "Animation/AnimationChannel.hpp"

std::shared_ptr<Node> AnimationChannel::Target() const
{
    return _target;
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
