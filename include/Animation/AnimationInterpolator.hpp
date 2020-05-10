#pragma once

class AnimationInterpolator
{
public:
	float PrevTime() const;
	void SetPrevTime(float prevTime);
	long long unsigned PrevKey() const;
	void SetPrevKey(long long unsigned prevKey);

private:
	float _prevTime { 0 };
	long long unsigned _prevKey { 0 };
};