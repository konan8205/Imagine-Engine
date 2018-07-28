#pragma once

#include "Render/Vulkan/VulkanHeader.h"
#include "Render/Vulkan/Vulkan.h"

class Vulkan;

struct WindowCreateInfo {
	int width = INT_MAX;
	int height = INT_MAX;
	int x = INT_MAX;
	int y = INT_MAX;
	const wchar_t* title = NULL;
	bool resizable = false;
};

enum class VulkanSurfaceType {
	None, Android, iOS, macOS, Windows, Wayland, XCB, Xlib
};

class VulkanSurface
{
	/* Variables */
public:
	VkInstance* instance;

	VkSurfaceKHR surface;
	VulkanSurfaceType surfaceType;

	int width;
	int height;
	bool resizable;

	/* Functions */
public:
	VulkanSurface(VkInstance* _instance)
		: instance(_instance)
		, width(INT_MAX)
		, height(INT_MAX)
		, resizable(false)
	{}
	~VulkanSurface() {}
	
	inline void DestroySurface() {
		vkDestroySurfaceKHR(*instance, surface, NULL);
	}
};