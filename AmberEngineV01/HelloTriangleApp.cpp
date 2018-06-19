#include "stdafx.h"
#include "HelloTriangleApp.h"

//#include <vulkan\vulkan.h>
//#include <iostream>
//#include <stdexcept>
//#include <functional>
//#include <cstdlib>

//VkResult CreateDebugReportCallbackExt(
//	VkInstance instance,
//	const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
//	const VkAllocationCallbacks* pAllocator,
//	VkDebugReportCallbackEXT* pCallback) {
//
//	auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
//	if (func != nullptr) {
//		return func(instance, pCreateInfo, pAllocator, pCallback);
//	}
//	else {
//		return VK_ERROR_EXTENSION_NOT_PRESENT;
//	}
//}

void HelloTriangleApp::run() {
	runPriv();
}