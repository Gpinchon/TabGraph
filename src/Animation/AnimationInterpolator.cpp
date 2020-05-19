#include "Animation/AnimationInterpolator.hpp"

float AnimationInterpolator::PrevTime() const
{
	return _prevTime;
}

void AnimationInterpolator::SetPrevTime(float prevTime)
{
	_prevTime = prevTime;
}

long long unsigned AnimationInterpolator::NextKey() const
{
	return _nextKey;
}

void AnimationInterpolator::SetNextKey(long long unsigned nextKey)
{
	_nextKey = nextKey;
}

long long unsigned AnimationInterpolator::PrevKey() const
{
	return _prevKey;
}

void AnimationInterpolator::SetPrevKey(long long unsigned prevKey)
{
	_prevKey = prevKey;
}
