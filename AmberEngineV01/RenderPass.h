#pragma once
#include "WindowView.h"

class RenderPass
{
public:
	RenderPass(WindowView& view) : windowView(view) {}
	
	VkRenderPass vkRenderPass;
	VkPipelineLayout pipelineLayout;

	void create();
	void cleanup();
private:
	WindowView& windowView;
};

