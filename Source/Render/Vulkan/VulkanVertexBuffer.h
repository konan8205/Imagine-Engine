#pragma once

#include "Render/Vulkan/VulkanHeader.h"
#include "Render/Vulkan/VulkanDevice.h"

class VulkanDevice;

struct VulkanVertex {
	glm::vec2 pos;
	glm::vec3 color;

	static inline VkVertexInputBindingDescription GetBindingDescription() {
		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(VulkanVertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}
	
	static inline array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions()
	{
		array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(VulkanVertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(VulkanVertex, color);

		return attributeDescriptions;
	}
};

const vector<VulkanVertex> vertices = {
	{ { 0.0f, -0.5f },{ 1.0f, 0.0f, 0.0f } },
	{ { 0.5f, 0.5f },{ 0.0f, 1.0f, 0.0f } },
	{ { -0.5f, 0.5f },{ 0.0f, 0.0f, 1.0f } }
};

class VulkanVertexBuffer
{
	/* Variables */
public:
	VkPhysicalDevice* pDevice;
	VkDevice* device;

	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	size_t vertexBufferSize;

	/* Functions */
public:
	VulkanVertexBuffer(VkPhysicalDevice* _pDevice, VkDevice* _device);
	~VulkanVertexBuffer();
	
	VkResult CreateVertexBuffer(uint64_t _size);
	void DestroyVertexBuffer() {
		vkDestroyBuffer(*device, vertexBuffer, nullptr);
	}
	
	VkResult AllocateMemory(const void* _vertices, const uint64_t _size);
	inline void FreeMemory() {
		vkFreeMemory(*device, vertexBufferMemory, nullptr);
	}

private:
	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
};