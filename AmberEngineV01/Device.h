#pragma once
#include "WindowView.h"
#include "SwapChain.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class Device
{
public:
	Device();
	~Device();

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice vkDevice;

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

	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
private:
	WindowView* pWindowView;

	void pickPhysicalDevice();
	void createLogicalDevice();

	int rateDeviceSuitability(VkPhysicalDevice device);
	bool checkDeviceExtensionsSupport(VkPhysicalDevice device);

	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentationModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	void createSwapChain();
	void createImageViews();
};

