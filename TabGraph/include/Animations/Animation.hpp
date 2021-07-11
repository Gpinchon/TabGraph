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
#include <Core/Inherit.hpp>
#include <Core/Object.hpp>
#include <Core/Property.hpp>

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Animations {
class Animation : public Core::Inherit<Core::Object, Animation> {
public:
    enum class LoopMode {
        Swing, Repeat
    };
    READONLYPROPERTY(bool, Playing, false);
    PROPERTY(bool, Loop, false);
    PROPERTY(LoopMode, LoopMode, LoopMode::Swing);
    PROPERTY(float, Speed, 1);

public:
    Animation();
    auto& GetChannelPosition() {
        return _positions;
    }
    auto& GetChannelScale() {
        return _scales;
    }
    auto& GetChannelRotation() {
        return _rotations;
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
    Channel<glm::vec3> _positions;
    Channel<glm::vec3> _scales;
    Channel<glm::quat> _rotations;
    float _currentTime { 0 };
};

}
