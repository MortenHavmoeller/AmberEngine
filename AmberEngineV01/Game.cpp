#include "stdafx.h"
#include "Game.h"

#include <thread>
#include <chrono>

int Game::run() {
	WindowView windowView;
	RenderPipeline renderPipeline;
	World world;
	Time time;

	std::cout << "Game is type: " << applicationType << std::endl;

	try {
		windowView.create();
		renderPipeline.create(&windowView);
		time.init(1.5, false);

		while (!glfwWindowShouldClose(windowView.window)) {
			time.startOfFrame();
			//std::cout << "since last frame (delta): " << time.delta() << " --- ";

			glfwPollEvents();

			world.update(); // game state update

			renderPipeline.drawFrame();

			time.frameOperationsDone();
			//std::cout << "frame ops took: " << time.delta() << std::endl;

			std::this_thread::sleep_for(
				std::chrono::milliseconds((uint32_t)(1000 * time.timeUntilNextFrame()))
			);
		}

		vkDeviceWaitIdle(windowView.device);

		renderPipeline.cleanup();
		windowView.cleanup();
	}
	catch (const std::runtime_error& e) {
		std::cerr << "error: " << e.what() << std::endl;

		std::cout << std::endl << "press enter to exit..." << std::endl;
		std::cin.get();

		return EXIT_FAILURE;
	}

	std::cout << std::endl << "press enter to exit..." << std::endl;
	std::cin.get();

	return EXIT_SUCCESS;
}
