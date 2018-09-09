#include "stdafx.h"
#include "Game.h"

#include <thread>
#include <chrono>

void inputUpdateRoutine(InputManager* inputManager, bool* pKeepRunning) {
	Time inputUpdateTimer;
	inputUpdateTimer.init(200, true);

	while (*pKeepRunning) {
		inputUpdateTimer.startOfFrame();
		inputManager->update();

		inputUpdateTimer.frameOperationsDone();

		std::this_thread::sleep_for(
			std::chrono::milliseconds((uint32_t)(1000 * inputUpdateTimer.timeUntilNextFrame()))
		);
	}
}

int Game::run() {
	std::cout << "Game application type: " << AmbEnums::getAmbAppTypeName(ambAppType) << std::endl;

	keepRunning = true;

	Camera mainCamera;
	mainCamera.pos = glm::vec3(2.0f, -2.0f, 2.0f);

	try {
		inputManager.create();

		std::thread inputThread(inputUpdateRoutine, &inputManager, &keepRunning);

		windowView.create();
		renderDevice.create(&windowView);
		renderPipeline.create(&windowView, &renderDevice);

		GameContext gameContext(*this, time);

		time.init(60, true);

		while (!glfwWindowShouldClose(windowView.window)) {
			time.startOfFrame();
			glfwPollEvents(); // check window events

			std::queue<InputEvent>* pInput = NULL;
			inputManager.getNextPtr(pInput); // get pointer to the input queue

			if (pInput->size() > 0) {
				if (pInput->front().type == move_forward) {
					// move forward...
					mainCamera.pos += vec3_forward * (float)time.delta();
				}

				std::cout << "input queue size " << pInput->size() << std::endl;
			}

			// empty input queue
			while (pInput->size() > 0) {
				pInput->pop();
			}

			world.update(); // game state update; currently empty

			renderPipeline.drawFrame(mainCamera, gameContext); // render to screen

			time.frameOperationsDone();

			std::this_thread::sleep_for(
				std::chrono::milliseconds((uint32_t)(1000 * time.timeUntilNextFrame()))
			);
		}

		keepRunning = false;

		vkDeviceWaitIdle(renderDevice.vkDevice); // wait for Vulkan to finish its last work

		renderPipeline.cleanup();
		renderDevice.cleanup();
		windowView.cleanup();

		inputManager.cleanup();
		inputThread.join();
	}
	catch (const std::runtime_error& e) {
		std::cerr << "error: " << e.what() << std::endl;

		std::cout << std::endl << "press enter to exit..." << std::endl;
		std::cin.get();

		return EXIT_FAILURE;
	}

#ifdef _DEBUG
	// allow developer to see printout before closing console
	std::cout << std::endl << "press enter to exit..." << std::endl;
	std::cin.get();
#endif

	return EXIT_SUCCESS;
}
