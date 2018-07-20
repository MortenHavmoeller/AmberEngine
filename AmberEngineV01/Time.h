#pragma once
#include <chrono>
#include <GLFW/glfw3.h>

class Time
{
public:
	Time();
	~Time();

	double beginning();
	double now();
	double delta();

	void init(uint32_t targetFramerate);
	void startOfFrame();
	void updateDelta();

	double timeUntilTargetDelta();
	
private:
	double firstRecordedTime;

	double lastRecordedTime;
	double delta_;

	uint32_t targetFramerate_;
	double targetDelta_;
};

