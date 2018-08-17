#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <array>

struct Vertex {
	glm::vec2 pos;
	glm::vec3 col;

	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription description = {};
		description.binding = 0;
		description.stride = sizeof(Vertex);
		
		description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		// non-instanced rendering
		// alternative is: VK_VERTEX_INPUT_RATE_INSTANCE
		// which tells Vulkan to only read the next data entry after the instance

		return description;
	}

	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 2> description;

		description[0].binding = 0;
		description[0].location = 0; // matches shader input definition
		description[0].format = VK_FORMAT_R32G32_SFLOAT;
		description[0].offset = offsetof(Vertex, pos);

		description[1].binding = 0;
		description[1].location = 1; // matches shader input definition
		description[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		description[1].offset = offsetof(Vertex, col);

		return description;
	}
};