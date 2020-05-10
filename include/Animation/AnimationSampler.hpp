#pragma once
#include <memory>

class BufferAccessor;

class AnimationSampler
{
public:
	enum AnimationInterpolation
	{
		Linear,
		Step,
		CubicSpline
	};
	AnimationSampler(std::shared_ptr<BufferAccessor> timings, std::shared_ptr<BufferAccessor> keyFrames);
	std::shared_ptr<BufferAccessor> Timings() const;
	void SetTimings(std::shared_ptr<BufferAccessor>);
	std::shared_ptr<BufferAccessor> KeyFrames() const;
	void SetKeyFrames(std::shared_ptr<BufferAccessor>);
	AnimationInterpolation Interpolation() const;
	void SetInterpolation(AnimationInterpolation);
protected:
	
private:
	std::shared_ptr<BufferAccessor> _timings{nullptr};
	std::shared_ptr<BufferAccessor> _keyFrames{nullptr};
	AnimationInterpolation _interpolation{AnimationInterpolation::Linear};
};