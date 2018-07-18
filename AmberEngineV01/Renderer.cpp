#include "stdafx.h"
#include "Renderer.h"
#include "WindowView.h"

void Renderer::create() {
	createCommandPool();
}

void Renderer::cleanup() {
	vkDestroyCommandPool(windowView.device, commandPool, nullptr);
}

void Renderer::createCommandPool()
{
	QueueFamilyIndices queueFamilyIndices = windowView.findQueueFamilies(windowView.physicalDevice);

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
	poolInfo.flags = 0; // optional

	if (vkCreateCommandPool(windowView.device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool!");
	}
}