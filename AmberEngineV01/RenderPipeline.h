#pragma once
#include "WindowView.h"
#include "RenderPass.h"
#include "filetool.h"

#include <string>
#include <fstream>
#include <windows.h>
#include <filesystem>

class RenderPipeline
{
public:
	RenderPipeline(WindowView& view) : windowView(view), renderPass(view) {}

	void create();
	void cleanup();

private:
	WindowView& windowView;

	RenderPass renderPass;
	VkPipelineLayout layout;
	VkPipeline graphicsPipeline;

	void createGraphicsPipeline(RenderPass renderPass);
	VkShaderModule createShaderModule(const std::vector<char>& code);
	
	void createFramebuffers();
	std::vector<VkFramebuffer> swapChainFramebuffers; // for use with createFramebuffers

};

