#pragma once
#include "WindowView.h"
#include "VulkanStructs.h"
#include "Vertex.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

class RenderDevice
{
public:
	RenderDevice();
	~RenderDevice();

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice vkDevice;

	VkQueue transferQueue;
	VkQueue graphicsQueue;
	VkQueue presentationQueue;

	// for SwapChain
	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImageView> swapChainImageViews;
	// /for SwapChain

	void create(WindowView* view);
	void cleanup();

	void recreateSwapChain();

	QueueFamilyIndices findPhysicalDeviceQueueFamilies();

	uint32_t findPhysicalMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
private:
	WindowView* pWindowView;

	void createLogicalDevice();
	void createSwapChain();
	void createImageViews();

	void pickPhysicalDevice();
	int rateDeviceSuitability(VkPhysicalDevice device);
	bool checkDeviceExtensionsSupport(VkPhysicalDevice device);
	
	void DisplayPhysicalDeviceFamilies(VkPhysicalDevice device);

	QueueFamilyIndices findPhysicalDeviceQueueFamilies(VkPhysicalDevice device);

	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentationModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

};

