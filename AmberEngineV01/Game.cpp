#include "stdafx.h"
#include "Game.h"

#include <thread>
#include <chrono>

int Game::run() {
	WindowView windowView;
	Device device;
	RenderPipeline renderPipeline;

	World world;
	Time time;

	std::cout << "Game application type: " << AmbEnums::getAmbAppTypeName(ambAppType) << std::endl;

	try {
		windowView.create();
		device.create(&windowView);
		renderPipeline.create(&windowView, &device);

		time.init(50.0, true);

		while (!glfwWindowShouldClose(windowView.window)) {
			time.startOfFrame();

			glfwPollEvents(); // check window events
			world.update(); // game state update
			renderPipeline.drawFrame(); // render to screen

			time.frameOperationsDone();

			std::this_thread::sleep_for(
				std::chrono::milliseconds((uint32_t)(1000 * time.timeUntilNextFrame()))
			);
		}

		vkDeviceWaitIdle(device.vkDevice); // wait for Vulkan to finish its last work

		renderPipeline.cleanup();
		device.cleanup();
		windowView.cleanup();
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
