#pragma once
#include "WindowView.h"
#include "filetool.h"

#include <string>
#include <fstream>
#include <windows.h>
#include <filesystem>

class RenderPipeline
{
public:
	RenderPipeline(WindowView& windowView) : ownedView(windowView) {}
	
	void create();
	void cleanup();

private:
	WindowView& ownedView;

	void createGraphicsPipeline(); // creates the render pipeline with shader loading
	VkShaderModule createShaderModule(const std::vector<char>& code);
};

