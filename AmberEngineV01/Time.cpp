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
	if (fixedTime_) {
		return targetDelta_;
	}

	return delta_;
}

void Time::init(double targetFramerate, bool fixedTime) {
	targetFramerate_ = targetFramerate;
	fixedTime_ = fixedTime;

	if (targetFramerate > 0) {
		targetDelta_ = 1.0 / (double)targetFramerate_;
		delta_ = targetDelta_;
	}
	else {
		if (fixedTime) {
			throw std::runtime_error("cannot create Time object with fixed time without a target framerate");
		}
		targetDelta_ = -1;
		delta_ = 0.1;
	}

	lastRecordedTime = glfwGetTime();
	firstRecordedTime = lastRecordedTime;
}

void Time::startOfFrame() {
	if (fixedTime_) {
		lastRecordedTime += targetDelta_;
	}
	else {
		double currentTime = glfwGetTime();
		delta_ = currentTime - lastRecordedTime;
		lastRecordedTime = currentTime;
	}
}

void Time::frameOperationsDone() {
	delta_ = glfwGetTime() - lastRecordedTime;
}

double Time::timeUntilNextFrame() {
	return std::max(targetDelta_ - delta_, 0.0);
}
