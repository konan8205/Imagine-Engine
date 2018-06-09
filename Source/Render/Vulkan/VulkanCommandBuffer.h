#pragma once

#include "Render/Vulkan/VulkanHeader.h"
#include "Render/Vulkan/Vulkan.h"

class Vulkan;

struct VulkanCommandBufferCreateInfo {
	uint32_t queueFamilyIndex = UINT32_MAX;
	uint32_t cmdCount = UINT32_MAX;
};

class VulkanCommandBuffer
{
	/* Variables */
public:
	VkDevice* device;

	uint32_t queueFamilyIndex;
	VkCommandPool cmdPool;
	vector<VkCommandBuffer> cmd;
private:

	/* Functions */
public:
	VulkanCommandBuffer(VkDevice* _device);
	~VulkanCommandBuffer();

	bool Initialize(VulkanCommandBufferCreateInfo _createInfo);
	inline void DeInitialize() {
		RemoveCommandBuffer();
		DestroyCommandPool();
	};

private:
	VkResult CreateCommandPool(const uint32_t _queueFamilyIndex);
	void DestroyCommandPool();

	VkResult CreateCommandBuffer(const uint32_t _count);
	bool RemoveCommandBuffer();
};