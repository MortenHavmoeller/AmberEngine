#include "stdafx.h"
#include "BufferTool.h"


BufferTool::BufferTool()
{
}


BufferTool::~BufferTool()
{
}

void BufferTool::createBuffer(RenderDevice* pDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer & buffer, VkDeviceMemory & bufferMemory)
{
	/* From text:
	It should be noted that in a real world application, you’re not supposed to actually call vkAllocateMemory for every individual buffer.
	The maximum number of simultaneous memory allocations is limited by the maxMemoryAllocationCount physical device limit,
	which may be as low as 4096 even on high end hardware like an NVIDIA GTX 1080.

	The right way to allocate memory for a large number of objects at the same time is to create a custom allocator
	that splits up a single allocation among many different objects by using the offset parameters that we’ve seen in many functions.

	You can either implement such an allocator yourself, or use the VulkanMemoryAllocator library provided by the GPUOpen initiative.
	(DOWNLOADED AND INCLUDED AS vk_mem_alloc.h IN THIS PROJECT)
	*/

	// data for the creation of the vertex buffer
	// size depends on the vertices vector size multiplied by the size of the Vertex data type
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;

	if (vkCreateBuffer(pDevice->vkDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create a buffer!");
	}

	// get memory requirements for the just-created vertex buffer
	VkMemoryRequirements memReqs;
	vkGetBufferMemoryRequirements(pDevice->vkDevice, buffer, &memReqs);

	// use obtained memory requirements to find the right type of memory
	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memReqs.size;
	allocInfo.memoryTypeIndex = pDevice->findPhysicalMemoryType(memReqs.memoryTypeBits, properties);

	std::cout << "Allocating buffer memory per-object. This is subject to change." << std::endl;

	if (vkAllocateMemory(pDevice->vkDevice, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to create buffer memory!");
	}

	// last parameter is offset within the allocated region of memory
	// this buffer is specifically for this single object, so offset is simply 0
	// if the offset is non-zero, it must be divisible by memReqs.alignment (VkMemoryRequirements)
	vkBindBufferMemory(pDevice->vkDevice, buffer, bufferMemory, 0);
}

void BufferTool::copyBuffer(CommandPoolContext& cmdPoolContext, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
	VkCommandBuffer commandBuffer = beginSingleTimeCommands(cmdPoolContext);

	VkBufferCopy copyRegion = {};
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	endSingleTimeCommands(cmdPoolContext, commandBuffer);
}

VkCommandBuffer BufferTool::beginSingleTimeCommands(CommandPoolContext& cmdPoolContext) {
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	//allocInfo.commandPool = commandPool;
	allocInfo.commandPool = cmdPoolContext.commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(cmdPoolContext.pDevice->vkDevice, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

void BufferTool::endSingleTimeCommands(CommandPoolContext& cmdPoolContext, VkCommandBuffer& commandBuffer) {
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(cmdPoolContext.queue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(cmdPoolContext.queue);

	vkFreeCommandBuffers(cmdPoolContext.pDevice->vkDevice, cmdPoolContext.commandPool, 1, &commandBuffer);
}
