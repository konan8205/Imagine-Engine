
#include "Render/Vulkan/Vulkan.h"

Vulkan::Vulkan(const VulkanCreateInfo _option)
	: option(_option)
	, deviceClass(nullptr)
	, cmdClass(nullptr)
	, surfaceClass(nullptr)
	, swapChainClass(nullptr)
	, renderPassClass(nullptr)
	, frameBufferClass(nullptr)
	, graphicsPipelineClass(nullptr)
	, queueClass(nullptr)
	, currentFrame(UINT32_MAX)
	, maxFrame(UINT32_MAX)
	, bDisableMultiThreadRendering(false)
	, bDisableMultiGPURendering(false)
{
	
}

Vulkan::~Vulkan()
{

}

bool Vulkan::Initialize()
{
#ifdef _DEBUG
	/* Debug options of vulkan instance */
	if (!IsInstanceLayerSupport()) {
		throw runtime_error("Validation layers requested, but not available");
		return false;
	}

	instanceLayerNames = {
		"VK_LAYER_LUNARG_standard_validation"
	};
#endif

	instanceExtensionNames = {
#ifdef _DEBUG
		VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
#endif
		VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef _WIN32
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#endif
	};

	/* Create the vulkan instance */
	if (CreateInstance() != VK_SUCCESS) {
		throw runtime_error("Failed to create vulkan instance");
		return false;
	}

	/* Create surface */
	VulkanSurfaceCreateInfo surfaceCreateInfo;
	surfaceCreateInfo.width = 1280;
	surfaceCreateInfo.height = 720;
	surfaceClass = new VulkanSurface(this);
	if (!surfaceClass->Initialize(surfaceCreateInfo)) {
		return false;
	}

	/* Create the logical device */
	VulkanDeviceCreateInfo deviceCreateInfo;
	deviceCreateInfo.bDisableMultiGPURendering = bDisableMultiGPURendering;
	deviceClass = new VulkanDevice(this, surfaceClass);
	if (!deviceClass->Initialize(deviceCreateInfo)) {
		return false;
	}

	/* Create swap Chain */
	swapChainClass = new VulkanSwapChain(
		deviceClass->pDevice,
		&deviceClass->queueFamilyStruct,
		&deviceClass->device,
		surfaceClass);

	if (!swapChainClass->Initialize()) {
		return false;
	}
	maxFrame = (uint32_t)swapChainClass->swapChainImageList.size();

	/* Create render pass */
	renderPassClass = new VulkanRenderPass(&deviceClass->device, swapChainClass);
	if (renderPassClass->CreateRenderPass() != VK_SUCCESS) {
		return false;
	}

	/* Create frame buffers */
	frameBufferClass = new VulkanFrameBuffer(&deviceClass->device, swapChainClass, renderPassClass);
	if (frameBufferClass->CreateFrameBuffer() != VK_SUCCESS) {
		return false;
	}

	/* Create graphics pipelines */
	graphicsPipelineClass = new VulkanGraphicsPipeline(&deviceClass->device, swapChainClass, renderPassClass);
	if (graphicsPipelineClass->CreateGraphicsPipeline() != VK_SUCCESS) {
		return false;
	}
	
	/* Allocate command buffers */
	VulkanCommandBufferCreateInfo cmdCreateInfo;
	cmdCreateInfo.queueFamilyIndex = deviceClass->queueFamilyStruct.graphicsQueueIndex;
	cmdCreateInfo.cmdPoolCount = (uint32_t)thread::hardware_concurrency();
	cmdCreateInfo.cmdCount = (uint32_t)frameBufferClass->frameBufferList.size();
	cmdClass = new VulkanCommandBuffer(
		&deviceClass->device,
		swapChainClass,
		frameBufferClass,
		renderPassClass,
		graphicsPipelineClass);

	if (!cmdClass->Initialize(cmdCreateInfo)) {
		return false;
	}

	queueClass = new VulkanQueue(&deviceClass->queueFamilyStruct, &deviceClass->device, swapChainClass, cmdClass);
	queueClass->GetDeviceQueue();
	currentFrame = 0;

	return true;
}

void Vulkan::DeInitialize()
{
	vkDeviceWaitIdle(deviceClass->device);

	delete queueClass;

	/* Free command buffers */
	cmdClass->DeInitialize();
	delete cmdClass;

	/* Clear graphics pipelines */
	graphicsPipelineClass->DestroyGraphicsPipeline();
	delete graphicsPipelineClass;

	/* Clear frame buffers */
	frameBufferClass->DestroyFrameBuffer();
	delete frameBufferClass;

	/* Clear render passes */
	renderPassClass->DestroyRenderPass();
	delete renderPassClass;

	/* Clear swap chains */
	swapChainClass->DeInialize();
	delete swapChainClass;

	/* Clear surface */
	surfaceClass->DeInitialize();
	delete surfaceClass;

	/* Clear logical device */
	deviceClass->DeInitialize();
	delete deviceClass;

	/* Destroy instance */
#ifdef _DEBUG
	DestroyDebugReportCallbackEXT(instance, callback, NULL);
#endif
	DestroyInstance();
}

bool Vulkan::Update()
{
	return false;
}

bool Vulkan::Render()
{
	cmdClass->Update();

	queueClass->SubmitQueue(currentFrame);
	queueClass->PresentQueue(currentFrame);

	currentFrame = (currentFrame + 1) % (maxFrame + 1);

	vkQueueWaitIdle(queueClass->presentQueue);
	return false;
}

VkResult Vulkan::CreateInstance()
{
	VkApplicationInfo appInfo;
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = NULL;
	appInfo.pApplicationName = option.appName;
	appInfo.applicationVersion = option.appVersion;
	appInfo.pEngineName = CoreInfo::GetEngineName();
	appInfo.engineVersion = CoreInfo::GetEngineVer();
#if defined(__ANDROID__) || defined(TARGET_OS_IPHONE)
	appInfo.apiVersion = VK_API_VERSION_1_0;
#else
	appInfo.apiVersion = VK_API_VERSION_1_1;
#endif

	VkInstanceCreateInfo instInfo = {};
	instInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instInfo.pNext = NULL;
	instInfo.flags = 0;
	instInfo.pApplicationInfo = &appInfo;
	instInfo.enabledLayerCount = (uint32_t)instanceLayerNames.size();
	instInfo.ppEnabledLayerNames = instanceLayerNames.size() ? instanceLayerNames.data() : NULL;
	instInfo.enabledExtensionCount = (uint32_t)instanceExtensionNames.size();
	instInfo.ppEnabledExtensionNames = instanceExtensionNames.data();

	VkResult result = vkCreateInstance(&instInfo, NULL, &instance);

#ifdef _DEBUG
	if (result == VK_SUCCESS) {
		VkDebugReportCallbackCreateInfoEXT createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
		createInfo.pfnCallback = DebugCallback;
		createInfo.pUserData = this;

		result = CreateDebugReportCallbackEXT(instance, &createInfo, NULL, &callback);

		if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to set up debug callback!");
		}
	}
#endif

	return result;
}

#ifdef _DEBUG
bool Vulkan::IsInstanceLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, NULL);

	vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : instanceLayerNames)
	{
		bool layerFound = false;
		for (const VkLayerProperties& layerProperties : availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}
		if (!layerFound) {
			return false;
		}
	}

	return true;
}

VkResult Vulkan::CreateDebugReportCallbackEXT(
	VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback)
{
	auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
	if (func != NULL) {
		return func(instance, pCreateInfo, pAllocator, pCallback);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void Vulkan::DestroyDebugReportCallbackEXT(
	VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
	if (func != NULL) {
		func(instance, callback, pAllocator);
	}
}

VKAPI_ATTR VkBool32 VKAPI_CALL Vulkan::DebugCallback(
	VkDebugReportFlagsEXT flags,
	VkDebugReportObjectTypeEXT objType,
	uint64_t obj,
	size_t location,
	int32_t code,
	const char* layerPrefix,
	const char* msg,
	void* userData)
{
	printf("[VkLayer]:\t %s\n", msg);

	return VK_FALSE;
}

#endif