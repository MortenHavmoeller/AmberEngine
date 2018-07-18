#include "stdafx.h"
#include "RenderPipeline.h"
#include "WindowView.h"

void RenderPipeline::create() {
	renderPass.create();
	createGraphicsPipeline(renderPass);
	createFramebuffers();
};

void RenderPipeline::cleanup() {
	for (auto framebuffer : swapChainFramebuffers) {
		vkDestroyFramebuffer(windowView.device, framebuffer, nullptr);
	}

	renderPass.cleanup();
	vkDestroyPipeline(windowView.device, graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(windowView.device, layout, nullptr);
};

void RenderPipeline::createGraphicsPipeline(RenderPass renderPass) {
	std::vector<char> vertShaderByteCode = filetool::readFile("../shaders/vert.spv"); // relative path starts in $(SolutionDir)/(ProjectName)
	std::vector<char> fragShaderByteCode = filetool::readFile("../shaders/frag.spv");

	VkShaderModule vertShaderModule = createShaderModule(vertShaderByteCode);
	VkShaderModule fragShaderModule = createShaderModule(fragShaderByteCode);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main"; // can point to other parts of shader code, if we want
	
	// optional; this handle allows initialization of shader constants
	// allowing the compiler to remove if-statements that depend on these
	vertShaderStageInfo.pSpecializationInfo = nullptr;

	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main"; // can point to other parts of shader code

	VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 0;
	vertexInputInfo.pVertexBindingDescriptions = nullptr; // optional; an array of structs that detail input data
	vertexInputInfo.vertexAttributeDescriptionCount = 0;
	vertexInputInfo.pVertexAttributeDescriptions = nullptr; // optional; an array of structs that detail input data

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport = {};
	viewport.x = 0;
	viewport.y = 0;
	viewport.width = (float) windowView.swapChainExtent.width;
	viewport.height = (float)windowView.swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = {0,0};
	scissor.extent = windowView.swapChainExtent;

	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport; // pointer to the previously created viewport
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor; // pointer to the previously created scissor

	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE; // discard fragments outside near/far planes
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT; // cull backfaces
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE; // define triangles in clockwise fashion
	
	// often used for shadow mapping
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // optional
	rasterizer.depthBiasClamp = 0.0f; // optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // optional - depth bias based on fragment slope

	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT; // no multisampling
	multisampling.minSampleShading = 1.0f; // optional
	multisampling.pSampleMask = nullptr; // optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // optional
	multisampling.alphaToOneEnable = VK_FALSE; // optional

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	// use all 4 color channels
	colorBlendAttachment.colorWriteMask =
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // optional
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // optional
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // optional
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // optional
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // optional

	// example alpha blend parameters
	/*colorBlendAttachment.blendEnable = VK_TRUE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;*/

	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // optional
	colorBlending.blendConstants[1] = 0.0f; // optional
	colorBlending.blendConstants[2] = 0.0f; // optional
	colorBlending.blendConstants[3] = 0.0f; // optional

	// example dynamic pipeline change
	/*VkDynamicState dynamicStates[] = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_LINE_WIDTH
	};
	VkPipelineDynamicStateCreateInfo dynamicState = {};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = 2;
	dynamicState.pDynamicStates = dynamicStates;*/

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0; // optional
	pipelineLayoutInfo.pSetLayouts = nullptr; // optional
	pipelineLayoutInfo.pushConstantRangeCount = 0; // optional
	pipelineLayoutInfo.pPushConstantRanges = nullptr; // optional

	if (vkCreatePipelineLayout(windowView.device, &pipelineLayoutInfo, nullptr, &layout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout");
	}

	// ---- CREATE THE ACTUAL PIPELINE ----
	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;

	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = nullptr; // optional
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = nullptr; // optional

	pipelineInfo.layout = layout;
	pipelineInfo.renderPass = renderPass.vkRenderPass;
	pipelineInfo.subpass = 0;

	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // optional
	pipelineInfo.basePipelineIndex = -1; // optional

	// there are also:
	// pipelineInfo.basePipelineHandle
	// pipelineInfo.basePipelineIndex
	// which allows for deriving from existing pipelines

	// seconds argument 'VK_NULL_HANDLE' is a VkPipelineCache object handle
	// that argument can be used to re-use data from pipeline creation between calls
	// significantly speeding up generation of new pipelines
	
	if (vkCreateGraphicsPipelines(windowView.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline!");
	}

	vkDestroyShaderModule(windowView.device, vertShaderModule, nullptr);
	vkDestroyShaderModule(windowView.device, fragShaderModule, nullptr);
}

VkShaderModule RenderPipeline::createShaderModule(const std::vector<char>& code) {
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	// Vulkan expects uint32_t code, but the code comes as char...
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
	
	VkShaderModule shaderModule;
	if (vkCreateShaderModule(windowView.device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module!");
	}
	
	return shaderModule;
}

void RenderPipeline::createFramebuffers() {
	swapChainFramebuffers.resize(windowView.swapChainImageViews.size());

	for (size_t i = 0; i < windowView.swapChainImageViews.size(); i++) {
		VkImageView attachments[] = { windowView.swapChainImageViews[i] };
		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass.vkRenderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = windowView.swapChainExtent.width;
		framebufferInfo.height = windowView.swapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(windowView.device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}