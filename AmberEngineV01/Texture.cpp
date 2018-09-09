#include "stdafx.h"
#include "Texture.h"

#include "VulkanHeader.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


void Texture::loadFromDisk(CommandPoolContext& cmdPoolContext, Texture& texture, std::string path)
{
	texture.pDevice = cmdPoolContext.pDevice;

	stbi_uc* pixels = stbi_load(path.c_str(), &texture.width, &texture.height, &texture.channels, STBI_rgb_alpha);
	texture.memoryFootprint = texture.width * texture.height * 4;

	if (!pixels) {
		throw new std::runtime_error("failed to load texture image!");
	}

	// assign to device memory and keep a handle that refers to it

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	VkDeviceSize vkSize = VkDeviceSize(texture.memoryFootprint);

	BufferTool::createBuffer(
		cmdPoolContext.pDevice,
		vkSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer,
		stagingBufferMemory
	);

	void* data;
	vkMapMemory(cmdPoolContext.pDevice->vkDevice, stagingBufferMemory, 0, vkSize, 0, &data);
	memcpy(data, pixels, static_cast<uint32_t>(vkSize));
	vkUnmapMemory(cmdPoolContext.pDevice->vkDevice, stagingBufferMemory);

	stbi_image_free(pixels);

	texture.createImage(
		cmdPoolContext.pDevice,
		static_cast<uint32_t>(texture.width),
		static_cast<uint32_t>(texture.height),
		VK_FORMAT_R8G8B8A8_UNORM,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);

	texture.format = VK_FORMAT_R8G8B8A8_UNORM;
	texture.tiling = VK_IMAGE_TILING_OPTIMAL;

	texture.transitionImageLayout(cmdPoolContext, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	texture.copyBufferToImage(cmdPoolContext, stagingBuffer, static_cast<uint32_t>(texture.width), static_cast<uint32_t>(texture.height));

	// prepare for shader access!
	texture.transitionImageLayout(cmdPoolContext, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	vkDestroyBuffer(cmdPoolContext.pDevice->vkDevice, stagingBuffer, nullptr);
	vkFreeMemory(cmdPoolContext.pDevice->vkDevice, stagingBufferMemory, nullptr);
}

void Texture::createImage(RenderDevice* pDevice, uint32_t widthArg, uint32_t heightArg, VkFormat formatArg, VkImageTiling tilingArg, VkImageUsageFlags usage, VkMemoryPropertyFlags properties)
{
	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = widthArg;
	imageInfo.extent.height = heightArg;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = formatArg; // needs to be the same format as the buffer

	// cannot be changed at a later time
	// !! must use VK_IMAGE_TILING_LINEAR if we need direct access to the texels in the image
	// should not be necessary with a stagin buffer (instead of staging image)
	imageInfo.tiling = tilingArg;

	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // only used by one queue family
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT; // related to multisampling - only relevant to images used as attachments
	imageInfo.flags = 0; // optional - can have flags related to sparse images

	if (vkCreateImage(pDevice->vkDevice, &imageInfo, nullptr, &image) != VK_SUCCESS) {
		throw std::runtime_error("failed to create image!");
	}

	VkMemoryRequirements memReqs;
	vkGetImageMemoryRequirements(pDevice->vkDevice, image, &memReqs);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memReqs.size;
	allocInfo.memoryTypeIndex = pDevice->findPhysicalMemoryType(memReqs.memoryTypeBits, properties);

	if (vkAllocateMemory(pDevice->vkDevice, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate image memory!");
	}

	vkBindImageMemory(pDevice->vkDevice, image, imageMemory, 0);
}

void Texture::transitionImageLayout(CommandPoolContext& cmdPoolContext, VkImageLayout oldLayout, VkImageLayout newLayout) {
	VkCommandBuffer commandBuffer = BufferTool::beginSingleTimeCommands(cmdPoolContext);

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED; // ignored because we are not transferring queue family ownership
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.srcAccessMask = 0; // TODO
	barrier.dstAccessMask = 0; // TODO

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else {
		throw std::invalid_argument("unsupported layout transition!");
	}

	vkCmdPipelineBarrier(
		commandBuffer,
		sourceStage,
		destinationStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);

	BufferTool::endSingleTimeCommands(cmdPoolContext, commandBuffer);
}

void Texture::copyBufferToImage(CommandPoolContext& cmdPoolContext, VkBuffer buffer, uint32_t width, uint32_t height) {
	VkCommandBuffer commandBuffer = BufferTool::beginSingleTimeCommands(cmdPoolContext);

	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferRowLength = 0; // tightly packed pixels in memory
	region.bufferImageHeight = 0;

	// copy pixels to this area:
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = { width, height, 1 };

	vkCmdCopyBufferToImage(
		commandBuffer,
		buffer,
		image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, // [dstImageLayout]: assume image is already in a format optimal for destination
		1,
		&region
	);

	BufferTool::endSingleTimeCommands(cmdPoolContext, commandBuffer);
}
