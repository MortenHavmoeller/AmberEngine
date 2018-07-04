#include "stdafx.h"
#include "WindowView.h"
#include "RenderPipeline.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <iostream>
#include <cstdlib>

int main() {
	WindowView windowView;
	RenderPipeline renderPipeline;

	try {
		windowView.create();
		renderPipeline.create(windowView);

		while (!glfwWindowShouldClose(windowView.window)) {
			glfwPollEvents();
		}

		//renderPipeline.cleanup();
		windowView.cleanup();

	}
	catch (const std::runtime_error& e) {
		std::cerr << e.what() << std::endl;
		
		std::cout << std::endl << "press enter to exit..." << std::endl;
		std::cin.get();

		return EXIT_FAILURE;
	}

	std::cout << std::endl << "press enter to exit..." << std::endl;
	std::cin.get();

	return EXIT_SUCCESS;
}