#pragma once
#include "WindowView.h"

class Renderer
{
public:
	Renderer(WindowView& view) : windowView(view) {}

	VkCommandPool commandPool;

	void create();
	void cleanup();

private:
	WindowView& windowView;

	void createCommandPool();
};

