
#include "Render/Vulkan/VulkanVertexBuffer.h"

VulkanVertexBuffer::VulkanVertexBuffer(VkPhysicalDevice* _pDevice, VkDevice* _device)
	: pDevice(_pDevice)
	, device(_device)
{

}

VulkanVertexBuffer::~VulkanVertexBuffer()
{

}

VkResult VulkanVertexBuffer::CreateVertexBuffer(uint64_t _size)
{
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = _size;
	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VkResult result = vkCreateBuffer(*device, &bufferInfo, nullptr, &vertexBuffer);
	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to create vertex buffer");
	}

	vertexBufferSize = _size;

	return result;
}

VkResult VulkanVertexBuffer::AllocateMemory(const void* _vertices, const uint64_t _size)
{
	if (_size != vertexBufferSize) {
		throw std::runtime_error("vertices size is differ from vertex buffer size");
		return VK_ERROR_MEMORY_MAP_FAILED;
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(*device, vertexBuffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(
		memRequirements.memoryTypeBits,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	VkResult result = vkAllocateMemory(*device, &allocInfo, nullptr, &vertexBufferMemory);
	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate vertex buffer memory");
		return result;
	}

	result = vkBindBufferMemory(*device, vertexBuffer, vertexBufferMemory, 0);
	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to bind vertex buffer memory");
		return result;
	}

	void* data;
	result = vkMapMemory(*device, vertexBufferMemory, 0, vertexBufferSize, 0, &data);
	memcpy(data, _vertices, (size_t)vertexBufferSize);
	vkUnmapMemory(*device, vertexBufferMemory);

	return result;
}

uint32_t VulkanVertexBuffer::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(*pDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && 
			(memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}
	
	return UINT32_MAX;
}