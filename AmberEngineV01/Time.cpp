#include "stdafx.h"
#include "Time.h"

#include <algorithm> // std::max


Time::Time()
{
}


Time::~Time()
{
}

double Time::beginning() {
	return firstRecordedTime;
}

double Time::now() {
	return lastRecordedTime;
}

double Time::delta() {
	return delta_;
}

void Time::init(uint32_t targetFramerate)
{
	targetFramerate_ = targetFramerate;
	targetDelta_ = 1.0 / (double)targetFramerate_;

	lastRecordedTime = glfwGetTime();
	firstRecordedTime = lastRecordedTime;

	delta_ = 0.05;
}

void Time::startOfFrame()
{
	lastRecordedTime = glfwGetTime();
}

void Time::updateDelta()
{
	delta_ = glfwGetTime() - lastRecordedTime;
}

double Time::timeUntilTargetDelta()
{
	return std::max(targetDelta_ - delta_, 0.0);
}
