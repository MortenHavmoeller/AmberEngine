#include "stdafx.h"
#include "RenderPass.h"


void RenderPass::create() {
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = windowView.swapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT; // multisampling possible
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // clear screen to black before drawing
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // store so it can be displayed
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // don't care for stencils
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // ditto
	
	// other common values are: 
	// VK_IMAGE_LAYOUT_PRESENT_SRC_KHR - images to be presented in the swap chain
	// VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL - images used as color attachments(?)
	// VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL - image is destination for memory copy op.
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // we're gonna clear old image anyway
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // ready to present to swap chain...

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0; // index number
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	// Vulkan will automatically transition the attachment when subpass is started...

	// a single subpass - more are possible, Vulkan will optimize their calling order
	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS; // this is not a compute subpass
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	// TEMP: frag shader refers to layout(location = 0) to refer to the above...

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;

	if (vkCreateRenderPass(windowView.device, &renderPassInfo, nullptr, &vkRenderPass) != VK_SUCCESS) {
		throw std::runtime_error("failed to create render pass!");
	}
}

void RenderPass::cleanup() {
	vkDestroyPipelineLayout(windowView.device, pipelineLayout, nullptr);
	vkDestroyRenderPass(windowView.device, vkRenderPass, nullptr);
}