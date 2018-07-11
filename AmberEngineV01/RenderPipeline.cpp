#include "stdafx.h"
#include "RenderPipeline.h"
#include "WindowView.h"

void RenderPipeline::create() {
	createGraphicsPipeline();
};

void RenderPipeline::cleanup() {
	// TODO: make sure we clean up everything
};

void RenderPipeline::createGraphicsPipeline() {
	auto vertShaderByteCode = filetool::readFile("../shaders/vert.spv"); // relative path starts in $(SolutionDir)/(ProjectName)
	std::cout << "Vertex shader byte size: " << vertShaderByteCode.size() << std::endl;

	auto fragShaderByteCode = filetool::readFile("../shaders/frag.spv");
	std::cout << "Fragment shader byte size: " << fragShaderByteCode.size() << std::endl;

	VkShaderModule vertShaderModule;
	VkShaderModule fragShaderModule;
}

VkShaderModule RenderPipeline::createShaderModule(const std::vector<char>& code) {
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	// Vulkan expects uint32_t code, but the code comes as char...
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
	
	VkShaderModule shaderModule;
	if (vkCreateShaderModule(ownedView.device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module!");
	}
	
	return shaderModule;
}