
#include "Render/Vulkan/VulkanSwapchain.h"

VulkanSwapChain::VulkanSwapChain(
	VulkanSurface* _VulkanSurfaceClass,
	VulkanPhysicalDevice* _pDeviceStruct,
	VkDevice* _device)
	: VulkanSurfaceClass(_VulkanSurfaceClass)
	, pDeviceStruct(_pDeviceStruct)
	, device(_device)
	
{

}

VulkanSwapChain::~VulkanSwapChain()
{

}

bool VulkanSwapChain::CreateSwapChain()
{
	/* Helper functions */
	VulkanSwapChainSupportDetails swapChainSupport = QuerySupportDetails();
	if (swapChainSupport.success != true) {
		throw runtime_error("Failed to query surface support details");
		return false;
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
	swapchainInfo.surface = VulkanSurfaceClass->surface;
	swapchainInfo.minImageCount = imageCount;
	swapchainInfo.imageFormat = surfaceFormat.format;
	swapchainInfo.imageColorSpace = surfaceFormat.colorSpace;
	swapchainInfo.imageExtent = extent;
	swapchainInfo.imageArrayLayers = 1;
	swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	uint32_t queueFamilyIndices[] = { pDeviceStruct->graphicsQueueIndex, pDeviceStruct->computeQueueIndex };
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
		return false;
	}

	vkGetSwapchainImagesKHR(*device, swapChain, &imageCount, NULL);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(*device, swapChain, &imageCount, swapChainImages.data());

	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;

	return true;
}

bool VulkanSwapChain::RecreateSwapChain()
{
	return false;
}

bool VulkanSwapChain::CreateImageView()
{
	VkResult result;

	swapChainImageViews.resize(swapChainImages.size());
	for (uint32_t i = 0; i < swapChainImages.size(); ++i) {
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = swapChainImages[i];
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

		result = vkCreateImageView(*device, &createInfo, NULL, &swapChainImageViews[i]);

		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to create image views");
			return false;
		}
	}
	return true;
}

VulkanSwapChainSupportDetails VulkanSwapChain::QuerySupportDetails()
{
	VkResult result;
	VulkanSwapChainSupportDetails details;

	const VkPhysicalDevice* pDevice = pDeviceStruct->pDevice;
	const VkSurfaceKHR* surface = &VulkanSurfaceClass->surface;

	// Capabilities
	result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*pDevice, *surface, &details.capabilities);
	if (result != VK_SUCCESS) {
		details.success = false;
		return details;
	}

	// Formats
	uint32_t formatCount;
	result = vkGetPhysicalDeviceSurfaceFormatsKHR(*pDevice, *surface, &formatCount, NULL);
	if (result != VK_SUCCESS) {
		details.success = false;
		return details;
	}

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		result = vkGetPhysicalDeviceSurfaceFormatsKHR(*pDevice, *surface, &formatCount, details.formats.data());
		if (result != VK_SUCCESS) {
			details.success = false;
			return details;
		}
	}

	// Present Modes
	uint32_t presentModeCount;
	result = vkGetPhysicalDeviceSurfacePresentModesKHR(*pDevice, *surface, &presentModeCount, NULL);
	if (result != VK_SUCCESS) {
		details.success = false;
		return details;
	}

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		result = vkGetPhysicalDeviceSurfacePresentModesKHR(*pDevice, *surface, &presentModeCount, details.presentModes.data());
		if (result != VK_SUCCESS) {
			details.success = false;
			return details;
		}
	}

	details.success = true;
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
		VkExtent2D actualExtent = { (uint32_t)VulkanSurfaceClass->width, (uint32_t)VulkanSurfaceClass->height };

		actualExtent.width = max(capabilities.minImageExtent.width, min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = max(capabilities.minImageExtent.height, min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}