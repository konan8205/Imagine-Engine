#pragma once

#include "Render/Vulkan/VulkanHeader.h"
#include "Render/Vulkan/Vulkan.h"
#include "Render/Vulkan/VulkanSwapchain.h"
#include "Render/Vulkan/VulkanRenderPass.h"
#include "Render/Vulkan/VulkanFrameBuffer.h"

class VulkanSwapChain;
class VulkanFrameBuffer;
class VulkanRenderPass;
class VulkanGraphicsPipeline;

struct VulkanCommandStruct {
	VkCommandPool cmdPool;
	vector<VkCommandBuffer> cmdList;
};

struct VulkanCommandBufferCreateInfo {
	uint32_t queueFamilyIndex = UINT32_MAX;
	uint32_t cmdPoolCount = UINT32_MAX;
	uint32_t cmdCount = UINT32_MAX;
};

class VulkanCommandBuffer
{
	/* Variables */
public:
	VkDevice* device;
	VulkanSwapChain* SwapChainClass;
	VulkanFrameBuffer* FrameBufferClass;
	VulkanRenderPass* RenderPassClass;
	VulkanGraphicsPipeline* GraphicsPipelineClass;

	uint32_t queueFamilyIndex;
	vector<VulkanCommandStruct> cmdStructList;
private:

	/* Functions */
public:
	VulkanCommandBuffer(
		VkDevice* _device,
		VulkanSwapChain* _SwapChainClass,
		VulkanFrameBuffer* _FrameBufferClass,
		VulkanRenderPass* _RenderPassClass,
		VulkanGraphicsPipeline* _GraphicsPipelineClass);
	~VulkanCommandBuffer();

	bool Initialize(VulkanCommandBufferCreateInfo _createInfo);
	inline void DeInitialize() {
		DestroyAllCommandBuffer();
		DestroyAllCommandPool();
	}
	
	VkResult Update();

private:
	VkResult CreateCommandPool(const uint32_t _cmdPoolIndex, const uint32_t _queueFamilyIndex);
	inline void DestroyCommandPool(const uint32_t _cmdPoolIndex) {
		vkDestroyCommandPool(*device, cmdStructList[_cmdPoolIndex].cmdPool, NULL);
	}
	inline void DestroyAllCommandPool() {
		for (VulkanCommandStruct iter : cmdStructList) {
			vkDestroyCommandPool(*device, iter.cmdPool, NULL);
		}
		cmdStructList.clear();
	}

	VkResult CreateCommandBuffer(const uint32_t _cmdPoolIndex, const uint32_t _count);
	inline void DestroyCommandBuffer(const uint32_t _cmdPoolIndex) {
		vkFreeCommandBuffers(
			*device,
			cmdStructList[_cmdPoolIndex].cmdPool,
			(uint32_t)cmdStructList[_cmdPoolIndex].cmdList.size(),
			cmdStructList[_cmdPoolIndex].cmdList.data());
	}
	inline void DestroyAllCommandBuffer() {
		for (VulkanCommandStruct iter : cmdStructList) {
			vkFreeCommandBuffers(
				*device, iter.cmdPool, (uint32_t)iter.cmdList.size(), iter.cmdList.data());
			iter.cmdList.clear();
		}
	}
};