#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <algorithm>
#include <memory>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Nodes {
class Node;
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Animations {
template<typename T>
struct Channel {
    enum class Interpolation {
        Linear,
        Step,
        CubicSpline
    };
    struct KeyFrame {
        T value{};
        float time{ 0 };
        bool operator<(const KeyFrame& other) {
            return time < other.time;
        }
    };
    Interpolation interpolation{ Interpolation::Linear };
    size_t previousKey{ 0 };
    std::shared_ptr<Nodes::Node> target;
    std::vector<KeyFrame> keyFrames;
    inline void InsertKeyFrame(const T& value, float time) {
        keyFrames.push_back({ value, time });
        std::sort(keyFrames.begin(), keyFrames.end());
    }
};
}
