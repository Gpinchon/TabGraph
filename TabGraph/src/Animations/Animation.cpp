/*
* @Author: gpinchon
* @Date:   2020-06-18 13:31:08
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-06-30 19:51:36
*/
#include <Animations/Animation.hpp>
#include <Animations/Interpolator.hpp>
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
    _scales.previousKey = 0;
    _positions.previousKey = 0;
    _rotations.previousKey = 0;
}

template<typename T>
auto InterpolateChannel(Channel<T>& channel, float t, bool& animationPlayed)
{
    auto& minKey{ *channel.keyFrames.begin() };
    auto& maxKey{ *channel.keyFrames.end() };
    t = std::clamp(t, minKey.time, maxKey.time);
    size_t nextKey = 0;
    //TODO use range based iterations
    Channel<T>::KeyFrame nextKeyFrame;
    Channel<T>::KeyFrame prevKeyFrame;
    for (auto i = channel.previousKey; i < channel.keyFrames.size(); ++i) {
        auto& keyFrame(channel.keyFrames.at(i));
        if (keyFrame.time > t) {
            nextKey = std::clamp(size_t(i), size_t(0), channel.keyFrames.size() - 1);
            channel.previousKey = std::clamp(size_t(nextKey - 1), size_t(0), size_t(nextKey));
            nextKeyFrame = keyFrame;
            break;
        }
        prevKeyFrame = keyFrame;
    }
    auto keyDelta(nextKeyFrame.time - prevKeyFrame.time);
    auto interpolationValue(0.f);
    if (keyDelta != 0)
        interpolationValue = (t - prevKeyFrame.time) / keyDelta;
    animationPlayed |= t < maxKey.time;
    return Interpolator::Interpolate(channel, channel.previousKey, nextKey, keyDelta, interpolationValue);
}

void Animation::Advance(float delta)
{
    if (!GetPlaying()) return;
    _currentTime += delta * GetSpeed();
    bool animationPlayed(false);
    _scales.target->SetLocalScale(InterpolateChannel(_scales, _currentTime, animationPlayed));
    _positions.target->SetLocalPosition(InterpolateChannel(_positions, _currentTime, animationPlayed));
    _rotations.target->SetLocalRotation(InterpolateChannel(_rotations, _currentTime, animationPlayed));
    if (!animationPlayed) {
        if (GetLoop()) {
            if (GetLoopMode() == LoopMode::Repeat)
                Reset();
            else
                SetSpeed(-GetSpeed());
        }
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
