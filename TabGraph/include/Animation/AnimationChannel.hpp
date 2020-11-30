#pragma once
#include <memory>

class Node;

class AnimationChannel
{
public:
	enum class Channel
	{
		None,
		Translation,
		Rotation,
		Scale,
		Weights
	};
	std::shared_ptr<Node> Target() const;
	void SetTarget(std::shared_ptr<Node>);
	size_t SamplerIndex() const;
	void SetSamplerIndex(size_t);
	Channel Path() const;
	void SetPath(Channel);

private:
	Channel _path{Channel::None};
	std::weak_ptr<Node> _target;
	size_t _samplerIndex{0};
};