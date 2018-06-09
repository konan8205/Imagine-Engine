#pragma once

#include "Render/Vulkan/VulkanHeader.h"
#include "Render/Vulkan/Vulkan.h"

class Vulkan;

struct VulkanPhysicalDevice
{
	VkPhysicalDevice* pDevice = NULL;

	vector<VkQueueFamilyProperties> queueFamilyProps;

	uint32_t graphicsQueueIndex = UINT32_MAX;
	uint32_t computeQueueIndex = UINT32_MAX;
	uint32_t transferQueueIndex = UINT32_MAX;
};

struct VulkanDeviceCreateInfo
{
	bool bDisableMultiGPURendering = false;
};

class VulkanDevice
{
	/* Variables */
public:
	// Parent class
	const Vulkan* VulkanClass;

	vector<const char *> deviceLayerNames;
	vector<const char *> deviceExtensionNames;
	
	// Chosen physical device
	VulkanPhysicalDevice pDeviceStruct;
	vector<VkPhysicalDevice> pDeviceList;
	vector<VkPhysicalDeviceGroupProperties> pDeviceGroupList;
	VkDevice device;

	/* Functions */
public:
	VulkanDevice(Vulkan* _VulkanClass);
	~VulkanDevice();

	bool Initialize(const VulkanDeviceCreateInfo _createInfo);
	inline void DeInitialize() {
		DestroyDevice();
	};

private:
	VkResult EnumeratePhysicalDevice();
	VkResult EnumeratePhysicalDeviceGroup();
	bool SetupQueueFamilyProperties(VulkanPhysicalDevice& _pDeviceStruct);
	VkResult CreateDevice(
		VkPhysicalDevice _pDevice,
		const uint32_t _queueFamilyIndex,
		VulkanDeviceCreateInfo _createInfo);
	inline void DestroyDevice() {
		vkDestroyDevice(device, NULL);
	}
};