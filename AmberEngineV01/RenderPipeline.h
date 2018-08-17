#pragma once
#include "WindowView.h"
#include "RenderDevice.h"
#include "RenderPass.h"
#include "filetool.h"
#include "UniformBufferObject.h"
#include "Contexts.h"

#include <string>
#include <fstream>
#include <windows.h>
#include <filesystem>
#include <cstdlib>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>



const int MAX_FRAMES_IN_FLIGHT = 2;

class RenderPipeline
{
public:
	
	VkCommandPool transferCommandPool;
	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> transferCommandBuffers;
	std::vector<VkCommandBuffer> commandBuffers;

	void create(WindowView* view, RenderDevice* device);
	void cleanup();

	void recreate();

	void drawFrame(GameContext gameContext);

private:
	WindowView* pWindowView;
	RenderDevice* pDevice;

	RenderPass renderPass;
	VkDescriptorSetLayout descriptorSetLayout;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;
	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;

	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;

	std::vector<VkFramebuffer> swapChainFramebuffers; // for use with createFramebuffers
	size_t currentFrame = 0;
	std::vector<VkSemaphore> imageAvailableSemaphores; // image acquired from swap chain
	std::vector<VkSemaphore> renderFinishedSemaphores; // image ready for presentation to surface
	std::vector<VkFence> inFlightFences; // fences to block CPU command submission from outrunning GPU rendering

	bool hasTransferCommandPool;

	void createDescriptorSetLayout();
	void createGraphicsPipeline();
	void createFramebuffers();
	void createCommandPools();
	void createVertexBuffer();
	void createIndexBuffer();
	void createUniformBuffers();
	void createDescriptorPool();
	void createDescriptorSets();
	void createCommandBuffers(VkIndexType indexType);
	void createSyncObjects();

	VkShaderModule createShaderModule(const std::vector<char>& code);
	
	void createBuffer_WithOwnMemory(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void copyBuffer_UsingOwnQueue(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	void updateUniformBuffer(uint32_t currentImage, GameContext gameContext);
};

