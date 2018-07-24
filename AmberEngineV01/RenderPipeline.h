#pragma once
#include "WindowView.h"
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
	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;

	void create(WindowView* view);
	void cleanup();

	void drawFrame();

private:
	WindowView* pWindowView;

	RenderPass renderPass;
	VkPipelineLayout layout;
	VkPipeline graphicsPipeline;

	void createGraphicsPipeline();
	VkShaderModule createShaderModule(const std::vector<char>& code);
	
	void createFramebuffers();
	std::vector<VkFramebuffer> swapChainFramebuffers; // for use with createFramebuffers

	size_t currentFrame = 0;

	std::vector<VkSemaphore> imageAvailableSemaphores; // image acquired from swap chain
	std::vector<VkSemaphore> renderFinishedSemaphores; // image ready for presentation to surface
	std::vector<VkFence> inFlightFences; // fences to block CPU command submissiong from outrunning GPU rendering

	void createCommandPool();
	void createCommandBuffers();
	void createSyncObjects();
};

