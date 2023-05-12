#pragma once

#include "cocos2d.h"

class Ticker
{
private:
	timeval timeTemp;
	double before;
	double after;
public:
	void tick();
	double getInterval();
};

inline void Ticker::tick()
{
	cocos2d::gettimeofday(&timeTemp, nullptr);
	before = after;
	after = timeTemp.tv_sec * 1000 + timeTemp.tv_usec * 0.001;
}

inline double Ticker::getInterval()
{
	return after - before;
}