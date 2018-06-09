
#include "Render/Vulkan/VulkanCommandBuffer.h"

VulkanCommandBuffer::VulkanCommandBuffer(VkDevice* _device)
	: device(_device)
	, queueFamilyIndex(UINT32_MAX)
{

}

VulkanCommandBuffer::~VulkanCommandBuffer()
{

}

bool VulkanCommandBuffer::Initialize(VulkanCommandBufferCreateInfo _createInfo)
{
	VkResult result;

	assert(_createInfo.queueFamilyIndex != UINT32_MAX
		&& _createInfo.cmdCount != UINT32_MAX);

	result = CreateCommandPool(_createInfo.queueFamilyIndex);
	if (result != VK_SUCCESS) {
		throw runtime_error("Failed to create the command pool");
		return false;
	}
	
	result = CreateCommandBuffer(_createInfo.cmdCount);
	if (result != VK_SUCCESS) {
		throw runtime_error("Failed to allocate the command buffer");
		vkDestroyCommandPool(*device, cmdPool, NULL);
		return false;
	}
	queueFamilyIndex = _createInfo.queueFamilyIndex;

	return true;
}

VkResult VulkanCommandBuffer::CreateCommandPool(const uint32_t _queueFamilyIndex)
{
	VkCommandPoolCreateInfo cmdPoolInfo = {};
	cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmdPoolInfo.pNext = NULL;
	cmdPoolInfo.queueFamilyIndex = _queueFamilyIndex;
	cmdPoolInfo.flags = 0;

	VkResult result;
	result = vkCreateCommandPool(*device, &cmdPoolInfo, NULL, &cmdPool);

	return result;
}

void VulkanCommandBuffer::DestroyCommandPool()
{
	vkDestroyCommandPool(*device, cmdPool, NULL);
}

VkResult VulkanCommandBuffer::CreateCommandBuffer(const uint32_t _count)
{
	VkCommandBufferAllocateInfo cmdInfo = {};
	cmdInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdInfo.pNext = NULL;
	cmdInfo.commandPool = cmdPool;
	cmdInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cmdInfo.commandBufferCount = _count;

	cmd.resize(_count);

	VkResult result;
	result = vkAllocateCommandBuffers(*device, &cmdInfo, cmd.data());
	
	return result;
}


bool VulkanCommandBuffer::RemoveCommandBuffer()
{
	bool cmdFound = false;
	vector<VkCommandBuffer>::iterator iter = cmd.begin();
	while (iter != cmd.end()) {
		vkFreeCommandBuffers(*device, cmdPool, 1, &*iter);
		cmdFound = true;
		++iter;
	}
	cmd.clear();

	return cmdFound;
}