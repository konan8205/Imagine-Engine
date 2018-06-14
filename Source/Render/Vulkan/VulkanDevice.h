#pragma once

#include "Render/Vulkan/VulkanHeader.h"
#include "Render/Vulkan/Vulkan.h"

class Vulkan;
class VulkanSurface;

struct VulkanQueueFamilyStruct
{
	vector<VkQueueFamilyProperties> queueFamilyProps;

	uint32_t graphicsQueueIndex = UINT32_MAX;
	uint32_t computeQueueIndex = UINT32_MAX;
	uint32_t transferQueueIndex = UINT32_MAX;

	uint32_t presentQueueIndex = UINT32_MAX;
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
	const VulkanSurface* SurfaceClass;

	vector<const char *> deviceLayerNames;
	vector<const char *> deviceExtensionNames;
	
	vector<VkPhysicalDevice> pDeviceList;
	vector<VkPhysicalDeviceGroupProperties> pDeviceGroupList;
	VulkanQueueFamilyStruct queueFamilyStruct;

	// physical device of logical device
	VkPhysicalDevice* pDevice;
	VkDevice device;

	/* Functions */
public:
	VulkanDevice(Vulkan* _VulkanClass, VulkanSurface* _SurfaceClass);
	~VulkanDevice();

	bool Initialize(const VulkanDeviceCreateInfo _createInfo);
	inline void DeInitialize() {
		DestroyDevice();
	};

private:
	VkResult EnumeratePhysicalDevice();
	VkResult EnumeratePhysicalDeviceGroup();
	VulkanQueueFamilyStruct GetQueueFamilyProperties(VkPhysicalDevice* _pDevice);
	VkResult CreateDevice(
		VkPhysicalDevice* _pDevice,
		VulkanQueueFamilyStruct _queueFamilyStruct,
		VulkanDeviceCreateInfo _createInfo);
	inline void DestroyDevice() {
		pDevice = NULL;
		vkDestroyDevice(device, NULL);
	}
};