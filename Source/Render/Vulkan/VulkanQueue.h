#pragma once

#include "Render/Vulkan/VulkanHeader.h"
#include "Render/Vulkan/VulkanDevice.h"
#include "Render/Vulkan/VulkanSwapChain.h"
#include "Render/Vulkan/VulkanCommandBuffer.h"

class VulkanSwapChain;
class VulkanCommandBuffer;

struct VulkanQueueFamilyStruct;

class VulkanQueue
{
	/* Variables */
public:
	VulkanQueueFamilyStruct* queueFamilyStruct;
	VkDevice* device;
	VulkanSwapChain* SwapChainClass;
	VulkanCommandBuffer* CmdClass;
	VkQueue graphicsQueue;
	VkQueue presentQueue;

	/* Functions */
public:
	VulkanQueue(
		VulkanQueueFamilyStruct* _queueFamilyStruct,
		VkDevice* _device,
		VulkanSwapChain* _SwapChainClass,
		VulkanCommandBuffer* _CmdClass);
	~VulkanQueue();

	void GetDeviceQueue();
	VkResult SubmitQueue(const uint32_t _imageIndex);
	VkResult PresentQueue(const uint32_t _imageIndex);
};