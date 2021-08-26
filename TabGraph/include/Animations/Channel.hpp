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
enum class Interpolation {
    Linear,
    Step,
    CubicSpline
};
template<typename T>
struct Channel {
    struct KeyFrame {
        T inputTangent{};
        T value{};
        T outputTangent{};
        float time{ 0 };
        bool operator<(const KeyFrame& other) {
            return time < other.time;
        }
    };
    Interpolation interpolation{ Interpolation::Linear };
    size_t previousKey{ 0 };
    std::shared_ptr<Nodes::Node> target;
    std::vector<KeyFrame> keyFrames;
    inline void InsertKeyFrame(const KeyFrame& keyFrame) {
        keyFrames.push_back(keyFrame);
        std::sort(keyFrames.begin(), keyFrames.end());
    }
};
}
