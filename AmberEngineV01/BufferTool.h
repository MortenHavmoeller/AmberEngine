#pragma once
#include "VulkanHeader.h"
#include "RenderDevice.h"
#include "Contexts.h"

#include <iostream>
#include <cstdlib>
#include <fstream>

class BufferTool {
public:
	BufferTool();
	~BufferTool();

	static void createBuffer(RenderDevice* device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	static void copyBuffer(CommandPoolContext& cmdPoolContext, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	static VkCommandBuffer beginSingleTimeCommands(CommandPoolContext& cmdPoolContext);
	static void endSingleTimeCommands(CommandPoolContext& cmdPoolContext, VkCommandBuffer& commandBuffer);
};