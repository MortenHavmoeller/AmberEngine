#pragma once
#include "WindowView.h"

class RenderPass
{
public:
	RenderPass() {}

	VkRenderPass vkRenderPass;
	VkPipelineLayout pipelineLayout;

	void create(WindowView* pView);
	void cleanup();
private:
	WindowView* pWindowView;
};

