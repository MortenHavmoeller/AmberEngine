#pragma once

#include "Time.h"

#include <cstdlib>
#include <unordered_map>
#include <queue>
#include <iostream>

enum InputCmdType {
	move_forward,
	move_backward,
	turn_right,
	turn_left
};

class InputEvent {
public:
	InputCmdType type;
	double time;
};

class InputManager
{
public:
	InputManager() {
		writeQueue = 0;
		pWriteQueue = &inputQueues[0];
	}
	~InputManager() {}

	void create();
	void update();
	void getNextPtr(std::queue<InputEvent>*& out);
	void cleanup();

private:
	uint16_t writeQueue;
	std::queue<InputEvent>* pWriteQueue;
	std::queue<InputEvent>* pReadQueue;
	std::queue<InputEvent> inputQueues[2]; // double buffered

	void swapQueues();
};

