#pragma once

#include "Render/Vulkan/VulkanHeader.h"
#include "Render/Vulkan/VulkanDevice.h"
#ifdef _WIN32
#include "Render/Vulkan/Platform/VulkanSurfaceWindows.h"
#endif

class VulkanDevice;
class VulkanSurface;
struct VulkanPhysicalDevice;

struct VulkanSwapChainSupportDetails {
	bool success = false;
	VkSurfaceCapabilitiesKHR capabilities;
	vector<VkSurfaceFormatKHR> formats;
	vector<VkPresentModeKHR> presentModes;
};

class VulkanSwapChain
{
	/* Variables */
public:
	// Parent class
	const VulkanSurface* VulkanSurfaceClass;
	VulkanPhysicalDevice* pDeviceStruct;
	const VkDevice* device;

	VkSwapchainKHR swapChain;
	vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	vector<VkImageView> swapChainImageViews;

private:

	/* Functions */
public:
	VulkanSwapChain(
		VulkanSurface* _VulkanSurfaceClass,
		VulkanPhysicalDevice* _pDeviceStruct,
		VkDevice* _device);
	~VulkanSwapChain();
	
	bool CreateSwapChain();
	bool RecreateSwapChain();
	inline void DestroySwapChain() {
		vkDestroySwapchainKHR(*device, swapChain, NULL);
	}

	bool CreateImageView();
	inline void DestroyImageView() {
		for (VkImageView iter : swapChainImageViews) {
			vkDestroyImageView(*device, iter, NULL);
		}
	}

private:
	VulkanSwapChainSupportDetails QuerySupportDetails();
	VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormat);
	VkPresentModeKHR ChoosePresentMode(const vector<VkPresentModeKHR> availablePresentMode);
	VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities);
};