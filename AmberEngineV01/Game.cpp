#include "stdafx.h"
#include "Game.h"

#include <thread>
#include <chrono>

Game::Game()
{
}


Game::~Game()
{
}

int Game::run()
{
	WindowView windowView;
	RenderPipeline renderPipeline;
	Time time;
	
	try {
		windowView.create();
		renderPipeline.create(&windowView);
		time.init(50);

		while (!glfwWindowShouldClose(windowView.window)) {
			time.startOfFrame();

			glfwPollEvents();
			renderPipeline.drawFrame();

			time.updateDelta();
			double timeLeft = time.timeUntilTargetDelta();
			std::this_thread::sleep_for(std::chrono::milliseconds((uint32_t)(1000 * timeLeft)));
			time.updateDelta();
		}

		vkDeviceWaitIdle(windowView.device);

		renderPipeline.cleanup();
		windowView.cleanup();
	}
	catch (const std::runtime_error& e) {
		std::cerr << "error: " << e.what() << std::endl;
		windowView.cleanup();

		std::cout << std::endl << "press enter to exit..." << std::endl;
		std::cin.get();

		return EXIT_FAILURE;
	}

	std::cout << std::endl << "press enter to exit..." << std::endl;
	std::cin.get();

	return EXIT_SUCCESS;
}
