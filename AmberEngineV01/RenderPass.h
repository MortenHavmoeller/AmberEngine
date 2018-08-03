#pragma once
#include "WindowView.h"
#include "RenderDevice.h"

class RenderPass
{
public:
	RenderPass() {}

	VkRenderPass vkRenderPass;

	void create(WindowView* view, RenderDevice* device);
	void cleanup();
private:
	WindowView* pWindowView;
	RenderDevice* pDevice;
};

