#include "stdafx.h"
#include "RenderPipeline.h"



const std::vector<Vertex> vertices = {
	{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
	{{0.5f, -0.5f },{ 0.0f, 1.0f, 0.0f}},
	{{0.5f, 0.5f },{ 0.0f, 0.0f, 1.0f}},
	{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
};

const std::vector<uint16_t> vertexIndices = {
	0, 1, 2, 2, 3, 0
};

void RenderPipeline::create(WindowView* view, RenderDevice* device) {
	pWindowView = view;
	pDevice = device;

	renderPass.create(pWindowView, pDevice);

	createDescriptorSetLayout();
	createGraphicsPipeline();
	createFramebuffers();
	createCommandPools();

	CommandPoolContext cmdPoolContext(pDevice, pDevice->graphicsQueue, commandPool);
	Texture::loadFromDisk(cmdPoolContext, texture, "../textures/dungeon_floor_tile.jpg");

	createVertexBuffer();
	createIndexBuffer();
	createUniformBuffers();
	createDescriptorPool();
	createDescriptorSets();
	createCommandBuffers(VK_INDEX_TYPE_UINT16);
	createSyncObjects();
};

void RenderPipeline::cleanup() {
	texture.destroy();

	vkDestroyBuffer(pDevice->vkDevice, indexBuffer, nullptr);
	vkFreeMemory(pDevice->vkDevice, indexBufferMemory, nullptr);

	vkDestroyBuffer(pDevice->vkDevice, vertexBuffer, nullptr);
	vkFreeMemory(pDevice->vkDevice, vertexBufferMemory, nullptr);

	vkDestroyCommandPool(pDevice->vkDevice, transferCommandPool, nullptr);
	vkDestroyCommandPool(pDevice->vkDevice, commandPool, nullptr);

	for (auto framebuffer : swapChainFramebuffers) {
		vkDestroyFramebuffer(pDevice->vkDevice, framebuffer, nullptr);
	}

	vkDestroyPipeline(pDevice->vkDevice, graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(pDevice->vkDevice, pipelineLayout, nullptr);
	renderPass.cleanup();

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(pDevice->vkDevice, renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(pDevice->vkDevice, imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(pDevice->vkDevice, inFlightFences[i], nullptr);
	}

	vkDestroyDescriptorPool(pDevice->vkDevice, descriptorPool, nullptr);

	vkDestroyDescriptorSetLayout(pDevice->vkDevice, descriptorSetLayout, nullptr);
	for (size_t i = 0; i < uniformBuffers.size(); i++) {
		vkDestroyBuffer(pDevice->vkDevice, uniformBuffers[i], nullptr);
		vkFreeMemory(pDevice->vkDevice, uniformBuffersMemory[i], nullptr);
	}
};

void RenderPipeline::recreate() {
	// wait for previous operations to finish
	vkDeviceWaitIdle(pDevice->vkDevice);

	// CLEANUP
	for (auto framebuffer : swapChainFramebuffers) {
		vkDestroyFramebuffer(pDevice->vkDevice, framebuffer, nullptr);
	}

	// allows re-use of the existing command pool
	vkFreeCommandBuffers(pDevice->vkDevice, transferCommandPool, static_cast<uint32_t>(transferCommandBuffers.size()), transferCommandBuffers.data());
	vkFreeCommandBuffers(pDevice->vkDevice, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());

	vkDestroyPipeline(pDevice->vkDevice, graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(pDevice->vkDevice, pipelineLayout, nullptr);

	renderPass.cleanup();

	// RECREATE

	// swap chain and image views
	pDevice->recreateSwapChain();

	// render pass
	renderPass.create(pWindowView, pDevice);

	createGraphicsPipeline();
	createFramebuffers();
	createCommandBuffers(VK_INDEX_TYPE_UINT16);
}

void RenderPipeline::drawFrame(Camera& camera, GameContext gameContext) {
	uint64_t uint64max = (std::numeric_limits<uint64_t>::max)();

	// wait for a fence to become signaled; fences are signaled as drawing operations finish
	vkWaitForFences(pDevice->vkDevice, 1, &inFlightFences[currentFrame], VK_TRUE, uint64max);

	// un-signal these fences so they can be signaled once this drawing operation has been finished
	vkResetFences(pDevice->vkDevice, 1, &inFlightFences[currentFrame]);

	// get an image from the swap chain
	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(pDevice->vkDevice, pDevice->swapChain, uint64max, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

	// if the window size has changed, recreate pipeline
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
		recreate();
		return;
	}
	else if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	updateUniformBuffer(camera, imageIndex, gameContext);

	// submitting the command buffer
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] }; // array of 1
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	// the command buffers to submit for execution...
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

	VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] }; // array of 1
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	// submit queue with fences
	if (vkQueueSubmit(pDevice->graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentationInfo = {};
	presentationInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentationInfo.waitSemaphoreCount = 1;
	presentationInfo.pWaitSemaphores = signalSemaphores; // render finished semaphore

	VkSwapchainKHR swapChains[] = { pDevice->swapChain };
	presentationInfo.swapchainCount = 1;
	presentationInfo.pSwapchains = swapChains;
	presentationInfo.pImageIndices = &imageIndex;
	presentationInfo.pResults = nullptr; // optional; handle to receive an array of VkResult if an array of swap chains is used

	// request presentation of an image to the swap chain
	result = vkQueuePresentKHR(pDevice->presentationQueue, &presentationInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
		recreate();
	}
	else if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image!");
	}

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void RenderPipeline::createDescriptorSetLayout() {
	VkDescriptorSetLayoutBinding layoutBinding = {};
	layoutBinding.binding = 0;
	layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	layoutBinding.descriptorCount = 1; // higher counts used for bones in a skeleton
	layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT; // only used by vertex shader
	layoutBinding.pImmutableSamplers = nullptr; // optional

	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &layoutBinding;

	if (vkCreateDescriptorSetLayout(pDevice->vkDevice, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

void RenderPipeline::createGraphicsPipeline() {
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

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	auto bindingDescription = Vertex::getBindingDescription();
	auto attributeDescriptions = Vertex::getAttributeDescriptions();

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());

	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport = {};
	viewport.x = 0;
	viewport.y = 0;
	viewport.width = (float)pDevice->swapChainExtent.width;
	viewport.height = (float)pDevice->swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = { 0,0 };
	scissor.extent = pDevice->swapChainExtent;

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
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; // define triangles in counter-clockwise fashion

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
	pipelineLayoutInfo.setLayoutCount = 1; // for descriptor sets
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
	pipelineLayoutInfo.pushConstantRangeCount = 0; // optional
	pipelineLayoutInfo.pPushConstantRanges = nullptr; // optional

	if (vkCreatePipelineLayout(pDevice->vkDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
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

	pipelineInfo.layout = pipelineLayout;
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

	if (vkCreateGraphicsPipelines(pDevice->vkDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline!");
	}

	vkDestroyShaderModule(pDevice->vkDevice, vertShaderModule, nullptr);
	vkDestroyShaderModule(pDevice->vkDevice, fragShaderModule, nullptr);
}

VkShaderModule RenderPipeline::createShaderModule(const std::vector<char>& code) {
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	// Vulkan expects uint32_t code, but the code comes as char...
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(pDevice->vkDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module!");
	}

	return shaderModule;
}

void RenderPipeline::createFramebuffers() {
	swapChainFramebuffers.resize(pDevice->swapChainImageViews.size());

	for (size_t i = 0; i < pDevice->swapChainImageViews.size(); i++) {
		VkImageView attachments[] = { pDevice->swapChainImageViews[i] };
		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass.vkRenderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = pDevice->swapChainExtent.width;
		framebufferInfo.height = pDevice->swapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(pDevice->vkDevice, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

void RenderPipeline::createCommandPools() {
	QueueFamilyIndices queueFamilyIndices = pDevice->findPhysicalDeviceQueueFamilies();


	// create a transfer command pool if possible
	if (queueFamilyIndices.hasTransferOnly()) {
		hasTransferCommandPool = true;
		VkCommandPoolCreateInfo transferPoolInfo = {};
		transferPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		transferPoolInfo.queueFamilyIndex = queueFamilyIndices.transferOnlyFamily;
		transferPoolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

		if (vkCreateCommandPool(pDevice->vkDevice, &transferPoolInfo, nullptr, &transferCommandPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create transfer command pool!");
		}
	}
	else {
		hasTransferCommandPool = false;
	}

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
	poolInfo.flags = 0; // optional

	if (vkCreateCommandPool(pDevice->vkDevice, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool!");
	}
}

void RenderPipeline::createVertexBuffer() {
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	BufferTool::createBuffer(pDevice, bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	// offset still hard-coded to 0; bufferInfo.size is the amount of data read from the buffer when drawing
	// it is possible to supply a special handle VK_WHOLE_SIZE to map all of the memory
	// flags also hard-coded to 0; none available in current API
	void* data; // raw memory pointer (undefined type)
	vkMapMemory(pDevice->vkDevice, stagingBufferMemory, 0, bufferSize, 0, &data);

	// copy the vertex data into the mapped memory
	memcpy(data, vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(pDevice->vkDevice, stagingBufferMemory);

	// destination buffer
	BufferTool::createBuffer(pDevice, bufferSize, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);
	// copy from staging buffer into vertex buffer
	CommandPoolContext cmdPoolContext(pDevice, pDevice->transferQueue, transferCommandPool);
	BufferTool::copyBuffer(cmdPoolContext, stagingBuffer, vertexBuffer, bufferSize);

	vkDestroyBuffer(pDevice->vkDevice, stagingBuffer, nullptr);
	vkFreeMemory(pDevice->vkDevice, stagingBufferMemory, nullptr);
}

void RenderPipeline::createIndexBuffer() {
	VkDeviceSize bufferSize = sizeof(vertexIndices[0]) * vertexIndices.size();

	// staging buffer
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	BufferTool::createBuffer(pDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	// copy vertex indices into 'data' and unmap the used memory
	void* data;
	vkMapMemory(pDevice->vkDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertexIndices.data(), (size_t)bufferSize);
	vkUnmapMemory(pDevice->vkDevice, stagingBufferMemory);

	// destination buffer
	BufferTool::createBuffer(pDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);
	// copy from staging buffer into index buffer
	CommandPoolContext cmdPoolContext(pDevice, pDevice->transferQueue, transferCommandPool);
	BufferTool::copyBuffer(cmdPoolContext, stagingBuffer, indexBuffer, bufferSize);

	// clean up staging buffer
	vkDestroyBuffer(pDevice->vkDevice, stagingBuffer, nullptr);
	vkFreeMemory(pDevice->vkDevice, stagingBufferMemory, nullptr);
}

//void RenderPipeline::copyBuffer_UsingOwnQueue(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
//	VkCommandBufferAllocateInfo allocInfo = {};
//	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
//	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
//	allocInfo.commandPool = transferCommandPool;
//	allocInfo.commandBufferCount = 1;
//
//	VkCommandBuffer transferBuffer;
//	vkAllocateCommandBuffers(pDevice->vkDevice, &allocInfo, &transferBuffer);
//
//	VkCommandBufferBeginInfo beginInfo = {};
//	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
//
//	vkBeginCommandBuffer(transferBuffer, &beginInfo);
//
//	VkBufferCopy copyRegion = {};
//	copyRegion.srcOffset = 0; // optional - used for buffers with many parts
//	copyRegion.dstOffset = 0; // optional - used for buffers with many parts
//	copyRegion.size = size;
//
//	vkCmdCopyBuffer(transferBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
//
//	vkEndCommandBuffer(transferBuffer);
//
//	VkSubmitInfo submitInfo = {};
//	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
//	submitInfo.commandBufferCount = 1;
//	submitInfo.pCommandBuffers = &transferBuffer;
//
//	vkQueueSubmit(pDevice->transferQueue, 1, &submitInfo, VK_NULL_HANDLE);
//
//	vkQueueWaitIdle(pDevice->transferQueue);
//	vkFreeCommandBuffers(pDevice->vkDevice, transferCommandPool, 1, &transferBuffer);
//}

void RenderPipeline::createUniformBuffers() {
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	uniformBuffers.resize(pDevice->swapChainImages.size());
	uniformBuffersMemory.resize(pDevice->swapChainImages.size());

	for (size_t i = 0; i < pDevice->swapChainImages.size(); i++) {
		BufferTool::createBuffer(
			pDevice,
			bufferSize,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			uniformBuffers[i],
			uniformBuffersMemory[i]
		);
	}
}

void RenderPipeline::updateUniformBuffer(Camera& camera, uint32_t currentImage, GameContext gameContext) {
	float time = (float)gameContext.time.elapsed();
	float aspectRatio = pDevice->swapChainExtent.width / (float)pDevice->swapChainExtent.height;
	UniformBufferObject ubo = {};
	// rotate unit matrix by time*radians, around the up vector
	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	
	ubo.view = glm::lookAt(camera.pos, glm::vec3(0.0f, 0.0f, 0.0f), camera.up());

	// perspective matrix with y-axis FOV, aspect ratio, near plane and far plane
	ubo.proj = glm::perspective(glm::radians(45.0f), pDevice->swapChainExtent.width / (float)pDevice->swapChainExtent.height, 0.1f, 10.0f);
	// GLM is designed for OpenGL, where y-coordinate for clip plane was inverted
	ubo.proj[1][1] *= -1;

	void* data;
	vkMapMemory(pDevice->vkDevice, uniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(pDevice->vkDevice, uniformBuffersMemory[currentImage]);
}

void RenderPipeline::createDescriptorPool() {
	VkDescriptorPoolSize poolSize = {};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = static_cast<uint32_t>(pDevice->swapChainImages.size());

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1; // one descriptor each frame
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = static_cast<uint32_t>(pDevice->swapChainImages.size());

	if (vkCreateDescriptorPool(pDevice->vkDevice, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

void RenderPipeline::createDescriptorSets() {
	std::vector<VkDescriptorSetLayout> layouts(pDevice->swapChainImages.size(), descriptorSetLayout);

	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(pDevice->swapChainImages.size());
	allocInfo.pSetLayouts = layouts.data();

	descriptorSets.resize(pDevice->swapChainImages.size());

#ifdef _DEBUG
	std::cout << "DescriptorSets size: " << descriptorSets.size() << std::endl;
	std::cout << "allocInfo.descriptorSetCount: " << allocInfo.descriptorSetCount << std::endl;
#endif

	if (vkAllocateDescriptorSets(pDevice->vkDevice, &allocInfo, &descriptorSets[0]) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor set!");
	}

	for (size_t i = 0; i < pDevice->swapChainImages.size(); i++) {
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = uniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		VkWriteDescriptorSet descriptorWrite = {};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = descriptorSets[i];
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;
		descriptorWrite.pImageInfo = nullptr; // optional
		descriptorWrite.pTexelBufferView = nullptr; // optional

		vkUpdateDescriptorSets(pDevice->vkDevice, 1, &descriptorWrite, 0, nullptr);
	}
}

void RenderPipeline::createCommandBuffers(VkIndexType indexType) {
#ifdef _DEBUG
	std::cout << "swap chain frame buffers size: " << swapChainFramebuffers.size() << std::endl;
#endif
	if (hasTransferCommandPool) {
		transferCommandBuffers.resize(swapChainFramebuffers.size());

		VkCommandBufferAllocateInfo transferCmdBufferAllocateInfo = {};
		transferCmdBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		transferCmdBufferAllocateInfo.commandPool = transferCommandPool;
		transferCmdBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		transferCmdBufferAllocateInfo.commandBufferCount = (uint32_t)transferCommandBuffers.size();

		if (vkAllocateCommandBuffers(pDevice->vkDevice, &transferCmdBufferAllocateInfo, transferCommandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate transfer command buffers!");
		}
	}

	commandBuffers.resize(swapChainFramebuffers.size());

	VkCommandBufferAllocateInfo cmdBufferAllocateInfo = {};
	cmdBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdBufferAllocateInfo.commandPool = commandPool;
	cmdBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cmdBufferAllocateInfo.commandBufferCount = (uint32_t)commandBuffers.size();

	if (vkAllocateCommandBuffers(pDevice->vkDevice, &cmdBufferAllocateInfo, commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}

	for (size_t i = 0; i < commandBuffers.size(); i++) {
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT; // allows simultaneous submittal to different rendering lines
		beginInfo.pInheritanceInfo = nullptr; // optional; only relevant for secondary buffers

		if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass.vkRenderPass;
		renderPassInfo.framebuffer = swapChainFramebuffers[i];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = pDevice->swapChainExtent;

		// used by VK_ATTACHMENT_LOAD_OP_CLEAR
		VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f }; // black
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		// VK_SUBPASS_CONTENTS_INLINE means the render pass resides in the primary command buffer; no secondaries
		vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

		VkBuffer vertexBuffers[] = { vertexBuffer };
		VkDeviceSize offsets[] = { 0 };

		// bind to the bindings specified in the Vertex struct
		// current command buffer, offset, number of bindings, array of vBuffers, array of mem offsets
		vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);

		// bind the index buffer
		vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer, 0, indexType);

		// descriptor sets - i.e. mvp matrix. Bind explicitly to the the graphics pipeline.
		vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[i], 0, nullptr);

		// draw vertices based on the vertex indices
		vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(vertexIndices.size()), 1, 0, 0, 0);

		vkCmdEndRenderPass(commandBuffers[i]);

		if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
	}
}

void RenderPipeline::createSyncObjects() {
	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // start fence in signaled state

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(pDevice->vkDevice, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(pDevice->vkDevice, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(pDevice->vkDevice, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {

			throw std::runtime_error("failed to create synchronization objects for CPU-GPU sync!");
		}
	}
}