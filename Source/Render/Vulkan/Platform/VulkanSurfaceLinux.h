#pragma once

#ifdef __linux__
#include "Render/Vulkan/VulkanHeader.h"
#include "Render/Vulkan/Vulkan.h"
#include "Render/Vulkan/Platform/VulkanSurface.h"

class Vulkan;
class VulkanSurface;

struct WindowCreateInfo;
enum class VulkanSurfaceType;

class VulkanSurfaceLinux : public VulkanSurface
{
	/* Variables */
private:

	/* Functions */
public:
	// Wayland
	bool CreateWindowWayland(const WindowCreateInfo& _createInfo);
	inline void DestroyWindowWayland() {

	}
	bool SetWindowAttributeWayland(const WindowCreateInfo& _createInfoo);

	// Xlib
	bool CreateWindowXlib(const WindowCreateInfo& _createInfo);
	inline void DestroyWindowXlib() {

	}
	bool SetWindowAttributeXlib(const WindowCreateInfo& _createInfoo);

	bool CreateSurface();
	bool Render();
};
#endif