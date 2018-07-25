#pragma once
#include "WindowView.h"
#include "Device.h"

class RenderPass
{
public:
	RenderPass() {}

	VkRenderPass vkRenderPass;

	void create(WindowView* view, Device* device);
	void cleanup();
private:
	WindowView* pWindowView;
	Device* pDevice;
};

