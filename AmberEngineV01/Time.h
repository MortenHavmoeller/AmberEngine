#pragma once
#include <chrono>
#include <GLFW/glfw3.h>
#include <stdexcept>

class Time
{
public:
	Time();
	~Time();

	double beginning();
	double now();
	double elapsed();
	double delta();

	void init(double targetFramerate, bool fixed);
	void startOfFrame();
	void frameOperationsDone();

	double timeUntilNextFrame();
	
private:
	bool fixedTime_;
	double firstRecordedTime;

	double lastRecordedTime;
	double delta_;

	double targetFramerate_;
	double targetDelta_;
};

