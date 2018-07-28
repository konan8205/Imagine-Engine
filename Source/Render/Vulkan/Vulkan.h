#pragma once

#include "Core/FileManager.h"
#include "Render/Vulkan/VulkanHeader.h"
#include "Render/Vulkan/VulkanDevice.h"
#include "Render/Vulkan/VulkanCommandBuffer.h"
#include "Render/Vulkan/VulkanSwapChain.h"
#include "Render/Vulkan/VulkanRenderPass.h"
#include "Render/Vulkan/VulkanFrameBuffer.h"
#include "Render/Vulkan/VulkanPipeline.h"
#include "Render/Vulkan/VulkanQueue.h"
#include "Render/Vulkan/VulkanVertexBuffer.h"

#include "Render/Vulkan/Platform/VulkanSurface.h"
#if defined(_WIN32)
#include "Render/Vulkan/Platform/VulkanSurfaceWindows.h"
#elif defined(__linux__)
#include "Render/Vulkan/Platform/VulkanSurfaceLinux.h"
#endif

#include <fstream>

class VulkanDevice;
class VulkanSwapChain;
class VulkanRenderPass;
class VulkanFrameBuffer;
class VulkanGraphicsPipeline;
class VulkanCommandBuffer;
class VulkanQueue;

#if defined(_WIN32)
class VulkanSurfaceWindows;
#elif defined(__linux__)
class VulkanSurfaceWayland;
#endif

struct VulkanCreateInfo
{
	const wchar_t* appName = CoreInfo::GetEngineName();
	uint32_t appVersion = CoreInfo::GetEngineVer();
};

/**
* Main class for Vulkan Renderer
*/
class Vulkan
{
	/* Variables */
public:
	VulkanCreateInfo option;
	vector<const char *> instanceLayerNames;
	vector<const char *> instanceExtensionNames;

	VkInstance instance;
	VulkanDevice* deviceClass;
	VulkanCommandBuffer* cmdClass;
	VulkanSwapChain* swapChainClass;
	VulkanRenderPass* renderPassClass;
	VulkanFrameBuffer* frameBufferClass;
	VulkanGraphicsPipeline* graphicsPipelineClass;
	VulkanVertexBuffer* vertexBufferClass;
	VulkanQueue* queueClass;

#if defined(_WIN32)
	VulkanSurfaceWindows* surfaceClass;
#elif defined(__linux__)
	VulkanSurfaceLinux* surfaceClass;
#endif

	uint32_t currentFrame;
	uint32_t maxFrame;
	
#ifdef _DEBUG
	VkDebugReportCallbackEXT callback;
#endif

private:
	bool bDisableMultiThreadRendering;
	bool bDisableMultiGPURendering;
	
	/* Functions */
public:
	Vulkan();
	~Vulkan();

	bool Initialize(const VulkanCreateInfo _createInfo);
	//bool ReInitialize();
	void DeInitialize();
	bool Update();
	bool Render();

	inline void SetDisableMultiThreadRendering(bool boolTemp) {
		bDisableMultiThreadRendering = boolTemp;
	}

	inline void SetDisableMultiGPURendering(bool boolTemp) {
		bDisableMultiGPURendering = boolTemp;
	}

private:
	VkResult CreateInstance();
	inline void DestroyInstance() {
		vkDestroyInstance(instance, NULL);
	}

#ifdef _DEBUG
	bool IsInstanceLayerSupport();
	VkResult CreateDebugReportCallbackEXT(
		VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback);
	void DestroyDebugReportCallbackEXT(
		VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator);

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
		VkDebugReportFlagsEXT flags,
		VkDebugReportObjectTypeEXT objType,
		uint64_t obj,
		size_t location,
		int32_t code,
		const char* layerPrefix,
		const char* msg,
		void* userData);

#endif
};