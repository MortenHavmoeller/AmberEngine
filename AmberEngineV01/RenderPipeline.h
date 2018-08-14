#pragma once
#include "WindowView.h"
#include "RenderDevice.h"
#include "RenderPass.h"
#include "filetool.h"

#include <string>
#include <fstream>
#include <windows.h>
#include <filesystem>
#include <cstdlib>



const int MAX_FRAMES_IN_FLIGHT = 2;

class RenderPipeline
{
public:
	
	VkCommandPool transferCommandPool;
	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> transferCommandBuffers;
	std::vector<VkCommandBuffer> commandBuffers;

	void create(WindowView* view, RenderDevice* device);
	void cleanup();

	void recreate();

	void drawFrame();

private:
	WindowView* pWindowView;
	RenderDevice* pDevice;

	RenderPass renderPass;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;

	std::vector<VkFramebuffer> swapChainFramebuffers; // for use with createFramebuffers
	size_t currentFrame = 0;
	std::vector<VkSemaphore> imageAvailableSemaphores; // image acquired from swap chain
	std::vector<VkSemaphore> renderFinishedSemaphores; // image ready for presentation to surface
	std::vector<VkFence> inFlightFences; // fences to block CPU command submission from outrunning GPU rendering

	bool hasTransferCommandPool;

	void createGraphicsPipeline();
	void createFramebuffers();
	void createCommandPools();
	void createVertexBuffer();
	void createCommandBuffers();
	void createSyncObjects();

	VkShaderModule createShaderModule(const std::vector<char>& code);
	
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
};

