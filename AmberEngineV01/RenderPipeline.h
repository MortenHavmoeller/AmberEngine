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
	RenderPipeline(WindowView& view) : windowView(view), renderPass(view) {}

	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;

	void create();
	void cleanup();

	void drawFrame();

private:
	WindowView& windowView;

	RenderPass renderPass;
	VkPipelineLayout layout;
	VkPipeline graphicsPipeline;

	void createGraphicsPipeline(RenderPass renderPass);
	VkShaderModule createShaderModule(const std::vector<char>& code);
	
	void createFramebuffers();
	std::vector<VkFramebuffer> swapChainFramebuffers; // for use with createFramebuffers

	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;

	void createCommandPool();
	void createCommandBuffers();
	void createSemaphores();
};

