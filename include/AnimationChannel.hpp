#pragma once
#include <memory>

class Node;

class AnimationChannel
{
public:
	enum AnimationPath
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
	AnimationPath Path() const;
	void SetPath(AnimationPath);

private:
	AnimationPath _path{AnimationPath::None};
	std::weak_ptr<Node> _target;
	size_t _samplerIndex{0};
};