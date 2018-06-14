#pragma once

#include "Render/Vulkan/VulkanHeader.h"
#include "Render/Vulkan/VulkanDevice.h"
#ifdef _WIN32
#include "Render/Vulkan/Platform/VulkanSurfaceWindows.h"
#endif

class VulkanSurface;

struct VulkanQueueFamilyStruct;

struct VulkanSwapChainSupportDetails {
	VkResult result;
	VkSurfaceCapabilitiesKHR capabilities;
	vector<VkSurfaceFormatKHR> formats;
	vector<VkPresentModeKHR> presentModes;
};

class VulkanSwapChain
{
	/* Variables */
public:
	VkPhysicalDevice* pDevice;
	VulkanQueueFamilyStruct * queueFamilyStruct;
	VkDevice* device;
	VulkanSurface* SurfaceClass;
	

	VkSwapchainKHR swapChain;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	vector<VkImage> swapChainImageList;
	vector<VkImageView> swapChainImageViewList;
	vector<VkSemaphore> imageAvailableSemaphoreList;
	vector<VkSemaphore> renderFinishedSemaphoreList;
	
	/* Functions */
public:
	VulkanSwapChain(
		VkPhysicalDevice* _pDevice,
		VulkanQueueFamilyStruct* _queueFamilyStruct,
		VkDevice* _device,
		VulkanSurface* _SurfaceClass);
	~VulkanSwapChain();

	bool Initialize();
	inline void DeInialize() {
		DestroySemaphore_();
		DestroyImageView();
		DestroySwapChain();
	}
	VkResult AcquireImage(uint32_t _imageIndex);

private:
	VkResult CreateSwapChain();
	inline void DestroySwapChain() {
		vkDestroySwapchainKHR(*device, swapChain, NULL);
	}

	VkResult CreateImageView();
	inline void DestroyImageView() {
		for (VkImageView iter : swapChainImageViewList) {
			vkDestroyImageView(*device, iter, NULL);
		}
	}

	VkResult CreateSemaphore_();
	inline void DestroySemaphore_() {
		for (VkSemaphore iter : renderFinishedSemaphoreList) {
			vkDestroySemaphore(*device, iter, nullptr);
		}
		for (VkSemaphore iter : imageAvailableSemaphoreList) {
			vkDestroySemaphore(*device, iter, nullptr);
		}
	}
	
	VulkanSwapChainSupportDetails QuerySupportDetails();
	VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormat);
	VkPresentModeKHR ChoosePresentMode(const vector<VkPresentModeKHR> availablePresentMode);
	VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities);
};