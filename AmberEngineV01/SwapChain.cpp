#include "stdafx.h"
#include "SwapChain.h"


SwapChain::SwapChain()
{
}


SwapChain::~SwapChain()
{
}

void SwapChain::create(WindowView* view) {
	pWindowView = view;
}

void SwapChain::cleanup() {
	//for (auto imageView : swapChainImageViews) {
	//	vkDestroyImageView(device, imageView, nullptr);
	//}

	//vkDestroySwapchainKHR(device, swapChain, nullptr);
}
