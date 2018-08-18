#pragma once
#include "RenderDevice.h"
#include "BufferTool.h"

#include <string>

class Texture
{
public:
	Texture(RenderDevice* pDeviceArg) : pDevice(pDeviceArg) {}

	void destroy() {
		vkDestroyImage(pDevice->vkDevice, image, nullptr);
		vkFreeMemory(pDevice->vkDevice, imageMemory, nullptr);
		pDevice = NULL;
	}

	RenderDevice* pDevice;

	int width, height, channels;
	uint64_t memoryFootprint;

	VkImage image;
	VkDeviceMemory imageMemory;

	VkFormat format;
	VkImageTiling tiling;

	//static Texture loadFromDisk(CommandPoolContext& cmdPoolContext, std::string path);
	static void loadFromDisk(CommandPoolContext& cmdPoolContext, Texture& texture, std::string path);

	void createImage(RenderDevice* pDevice, uint32_t widthArg, uint32_t heightArg, VkFormat formatArg, VkImageTiling tilingArg, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
	void transitionImageLayout(CommandPoolContext& cmdPoolContext, VkImageLayout oldLayout, VkImageLayout newLayout);
	void copyBufferToImage(CommandPoolContext& cmdPoolContext, VkBuffer buffer, uint32_t width, uint32_t height);
private:
	
};

