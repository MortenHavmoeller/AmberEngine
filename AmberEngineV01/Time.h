#pragma once



class Time
{
public:
	Time();
	~Time();

	static double exactNow();

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

#ifdef _DEBUG
	bool frameStartCalled_ = false;
	bool frameOpsDoneCalled_ = false;
#endif
};

