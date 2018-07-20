#pragma once
#include "WindowView.h"
#include "RenderPass.h"
#include "filetool.h"

#include <string>
#include <fstream>
#include <windows.h>
#include <filesystem>
#include <cstdlib>

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

	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;

	void createCommandPool();
	void createCommandBuffers();
	void createSemaphores();
};

