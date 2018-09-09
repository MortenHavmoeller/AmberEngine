#include "stdafx.h"
#include "InputManager.h"

enum InputKey {
	key_arrow_up,
	key_arrow_down,
	key_arrow_right,
	key_arrow_left,
	key_a,
	key_b,
	key_c,
	key_d,
	key_e,
	key_f,
	key_g,
	key_h,
	key_i,
	key_j,
	key_k,
	key_l,
	key_m,
	key_n,
	key_o,
	key_p,
	key_q,
	key_r,
	key_s,
	key_t,
	key_u,
	key_v,
	key_w,
	key_x,
	key_y,
	key_z,
	key_space,
	key_plus,
	key_minus,
	key_numpad_plus,
	key_numpad_minus,
	key_number_0,
	key_number_1,
	key_number_2,
	key_number_3,
	key_number_4,
	key_number_5,
	key_number_6,
	key_number_7,
	key_number_8,
	key_number_9,
	key_shift,
	key_control,
	key_alt,
	key_enter,
	key_backspace
};

// singleton
class InputMap {
public:
	static InputCmdType get(InputKey key) {
		return InputMap::_getSingleton().map[key];
	}

	// delete functions in public to ensure better error messages
	InputMap(InputMap const&) = delete;
	void operator=(InputMap const&) = delete;

private:
	InputMap() {}
	
	static InputMap& _getSingleton() {
		static InputMap singleton;
		return singleton;
	}

	std::unordered_map<InputKey, InputCmdType> map{
		{ key_w, move_forward },
		{ key_s, move_backward },
		{ key_a, turn_left },
		{ key_d, turn_right },
		{ key_arrow_up, move_forward },
		{ key_arrow_down, move_backward },
		{ key_arrow_left, turn_left },
		{ key_arrow_right, turn_right }
	};
};

void InputManager::create() {

}

void InputManager::update()
{
	// TODO: guarantee this function is never called async, or async with get(...);

	// put stuff into pWriteQueue...
	InputEvent currentEvt;
	currentEvt.type = InputMap::get(key_w); // pretend we're holding 'w'
	//currentEvt.time = time.now();

	pWriteQueue->push(currentEvt);
}

void InputManager::getNextPtr(std::queue<InputEvent>*& out)
{
	// TODO: guarantee this function is never called async
	// TODO: check if old read queue has been emptied
	// if not; frontload its contents on the new read queue?

	swapQueues();
	out = pReadQueue;

	// from this point on, 'out' is being operated on by the caller...
}

void InputManager::cleanup() {

}

void InputManager::swapQueues() {
	uint16_t newWriteQueue = (writeQueue + 1) % 2;
	pWriteQueue = &inputQueues[newWriteQueue]; // assign new write queue
	pReadQueue = &inputQueues[writeQueue]; // read from old write queue
	writeQueue = newWriteQueue; // record the increment
}
