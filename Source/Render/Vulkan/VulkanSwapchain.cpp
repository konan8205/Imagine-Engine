
#include "Render/Vulkan/VulkanSwapchain.h"

VulkanSwapChain::VulkanSwapChain(
	VkPhysicalDevice* _pDevice,
	VulkanQueueFamilyStruct* _queueFamilyStruct,
	VkDevice* _device,
	VulkanSurface* _SurfaceClass)
	: pDevice(_pDevice)
	, queueFamilyStruct(_queueFamilyStruct)
	, device(_device)
	, SurfaceClass(_SurfaceClass)
{

}

VulkanSwapChain::~VulkanSwapChain()
{

}

bool VulkanSwapChain::Initialize()
{
	VkResult result;

	result = CreateSwapChain();
	if (result != VK_SUCCESS) {
		return false;
	}
	result = CreateImageView();
	if (result != VK_SUCCESS) {
		return false;
	}
	result = CreateSemaphore_();
	if (result != VK_SUCCESS) {
		return false;
	}

	return true;
}

VkResult VulkanSwapChain::AcquireImage(uint32_t _imageIndex)
{
	VkResult result;
	result = vkAcquireNextImageKHR(
		*device,
		swapChain,
		10,
		renderFinishedSemaphoreList[_imageIndex],
		VK_NULL_HANDLE,
		&_imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw runtime_error("Failed to acquire swap chain image");
	}

	return result;
}

VkResult VulkanSwapChain::CreateSwapChain()
{
	/* Helper functions */
	VulkanSwapChainSupportDetails swapChainSupport = QuerySupportDetails();
	if (swapChainSupport.result != VK_SUCCESS) {
		throw runtime_error("Failed to query surface support details");
		return swapChainSupport.result;
	}

	VkSurfaceFormatKHR surfaceFormat = ChooseSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = ChoosePresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = ChooseExtent(swapChainSupport.capabilities);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 &&
		imageCount > swapChainSupport.capabilities.maxImageCount)
	{
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR swapchainInfo = {};
	swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainInfo.pNext = NULL;
	swapchainInfo.surface = SurfaceClass->surface;
	swapchainInfo.minImageCount = imageCount;
	swapchainInfo.imageFormat = surfaceFormat.format;
	swapchainInfo.imageColorSpace = surfaceFormat.colorSpace;
	swapchainInfo.imageExtent = extent;
	swapchainInfo.imageArrayLayers = 1;
	swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	uint32_t queueFamilyIndices[] = { 
		queueFamilyStruct->graphicsQueueIndex,
		queueFamilyStruct->presentQueueIndex,
	};

	if (queueFamilyIndices[0] != queueFamilyIndices[1]) {
		swapchainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapchainInfo.queueFamilyIndexCount = 2;
		swapchainInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapchainInfo.queueFamilyIndexCount = 0; // Optional
		swapchainInfo.pQueueFamilyIndices = NULL; // Optional
	}

	swapchainInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchainInfo.presentMode = presentMode;
	swapchainInfo.clipped = VK_TRUE;
	swapchainInfo.oldSwapchain = VK_NULL_HANDLE;

	VkResult result;
	result = vkCreateSwapchainKHR(*device, &swapchainInfo, NULL, &swapChain);

	if (result != VK_SUCCESS) {
		throw runtime_error("Failed to create swap chain");
		return result;
	}

	vkGetSwapchainImagesKHR(*device, swapChain, &imageCount, NULL);
	swapChainImageList.resize(imageCount);
	result = vkGetSwapchainImagesKHR(*device, swapChain, &imageCount, swapChainImageList.data());

	if (result != VK_SUCCESS) {
		throw runtime_error("Failed to get swapchain image");
		DestroySwapChain();
		return result;
	}

	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;

	return result;
}

VkResult VulkanSwapChain::CreateImageView()
{
	VkResult result;

	swapChainImageViewList.resize(swapChainImageList.size());
	for (uint32_t i = 0; i < swapChainImageList.size(); ++i) {
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = swapChainImageList[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = swapChainImageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		result = vkCreateImageView(*device, &createInfo, NULL, &swapChainImageViewList[i]);

		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to create image views");
			--i;
			do {
				vkDestroyImageView(*device, swapChainImageViewList[i], NULL);
				--i;
			} while (i != 0);
			swapChainImageList.clear();
			return result;
		}
	}
	return result;
}

VkResult VulkanSwapChain::CreateSemaphore_()
{
	VkResult result;
	imageAvailableSemaphoreList.resize(swapChainImageList.size());
	renderFinishedSemaphoreList.resize(swapChainImageList.size());

	for (uint32_t i = 0; i < imageAvailableSemaphoreList.size(); ++i) {
		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		result = vkCreateSemaphore(*device, &semaphoreInfo, nullptr, &imageAvailableSemaphoreList[i]);
		if (result != VK_SUCCESS) {
			--i;
			do {
				vkDestroySemaphore(*device, imageAvailableSemaphoreList[i], NULL);
				--i;
			} while (i != 0);
			return result;
		}
	}

	for (uint32_t i = 0; i < renderFinishedSemaphoreList.size(); ++i) {
		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		result = vkCreateSemaphore(*device, &semaphoreInfo, nullptr, &renderFinishedSemaphoreList[i]);
		if (result != VK_SUCCESS) {
			--i;
			do {
				vkDestroySemaphore(*device, renderFinishedSemaphoreList[i], NULL);
				--i;
			} while (i != 0);
			return result;
		}
	}
	
	return result;
}

VulkanSwapChainSupportDetails VulkanSwapChain::QuerySupportDetails()
{
	VkResult result;
	VulkanSwapChainSupportDetails details;

	const VkSurfaceKHR* surface = &SurfaceClass->surface;

	// Capabilities
	result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*pDevice, *surface, &details.capabilities);
	if (result != VK_SUCCESS) {
		details.result = result;
		return details;
	}

	// Formats
	uint32_t formatCount;
	result = vkGetPhysicalDeviceSurfaceFormatsKHR(*pDevice, *surface, &formatCount, NULL);
	if (result != VK_SUCCESS) {
		details.result = result;
		return details;
	}

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		result = vkGetPhysicalDeviceSurfaceFormatsKHR(*pDevice, *surface, &formatCount, details.formats.data());
		if (result != VK_SUCCESS) {
			details.result = result;
			return details;
		}
	}

	// Present Modes
	uint32_t presentModeCount;
	result = vkGetPhysicalDeviceSurfacePresentModesKHR(*pDevice, *surface, &presentModeCount, NULL);
	if (result != VK_SUCCESS) {
		details.result = result;
		return details;
	}

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		result = vkGetPhysicalDeviceSurfacePresentModesKHR(*pDevice, *surface, &presentModeCount, details.presentModes.data());
		if (result != VK_SUCCESS) {
			details.result = result;
			return details;
		}
	}

	details.result = result;
	return details;
}

VkSurfaceFormatKHR VulkanSwapChain::ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormat)
{
	if (availableFormat.size() == 1 && availableFormat[0].format == VK_FORMAT_UNDEFINED) {
		return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}

	for (const auto& availableFormat : availableFormat) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}

	return availableFormat[0];
}

VkPresentModeKHR VulkanSwapChain::ChoosePresentMode(const vector<VkPresentModeKHR> availablePresentMode)
{
	VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

	for (const VkPresentModeKHR availablePresentMode : availablePresentMode) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
		else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
			bestMode = availablePresentMode;
		}
	}

	return bestMode;
}

VkExtent2D VulkanSwapChain::ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != UINT32_MAX) {
		return capabilities.currentExtent;
	}
	else {
		VkExtent2D actualExtent = { (uint32_t)SurfaceClass->width, (uint32_t)SurfaceClass->height };

		actualExtent.width = max(capabilities.minImageExtent.width, min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = max(capabilities.minImageExtent.height, min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}