#ifdef _WIN32

#include "Render/Vulkan/Platform/VulkanSurfaceWindows.h"

VulkanSurface::VulkanSurface(Vulkan* _VulkanClass)
	: VulkanClass(_VulkanClass)
	, instance(&_VulkanClass->instance)
	, width(-1)
	, height(-1)
{

}
VulkanSurface::~VulkanSurface()
{
}

bool VulkanSurface::Initialize(VulkanSurfaceCreateInfo _createInfo)
{	
	if (CreateWindowWin32(_createInfo.width, _createInfo.height) &&
		CreateSurface())
	{
		width = _createInfo.width;
		height = _createInfo.height;
		return true;
	}

	return false;
}

bool VulkanSurface::Render()
{
	MSG msg;
	PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
	if (msg.message == WM_QUIT) {
		return false;
	}
	TranslateMessage(&msg);
	DispatchMessage(&msg);
	RedrawWindow(hWnd, NULL, NULL, RDW_INTERNALPAINT);
	return true;
}

bool VulkanSurface::CreateSurface()
{
	const VkInstance instance = VulkanClass->instance;

	VkWin32SurfaceCreateInfoKHR surfaceInfo = {};
	surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceInfo.hinstance = hInstance;
	surfaceInfo.hwnd = hWnd;

	if (vkCreateWin32SurfaceKHR(instance, &surfaceInfo, NULL, &surface) != VK_SUCCESS) {
		throw runtime_error("Failed to create window surface");
		return false;
	}


	return true;
}

bool VulkanSurface::CreateWindowWin32(const int& _width, const int& _height)
{
	width = _width;
	height = _height;
	assert(width > 0 || height > 0);

	WNDCLASSEX winInfo;
	memset(&winInfo, 0, sizeof(WNDCLASSEX));
	winInfo.cbSize = sizeof(WNDCLASSEX);
	winInfo.style = CS_HREDRAW | CS_VREDRAW;
	winInfo.lpfnWndProc = WndProc;
	winInfo.cbClsExtra = 0;
	winInfo.cbWndExtra = 0;
	winInfo.hInstance = hInstance;
	winInfo.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winInfo.hCursor = LoadCursor(NULL, IDC_ARROW);
	winInfo.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	winInfo.lpszMenuName = NULL;
	winInfo.lpszClassName = VulkanClass->option.appName;
	winInfo.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

	if (!RegisterClassEx(&winInfo)) {
		printf("Unexpected error trying to start the application!\n");
		fflush(stdout);
		exit(1);
	}

	RECT wr = { 0, 0, width, height };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
	hWnd = CreateWindowEx(0,
		VulkanClass->option.appName,	// class name
		VulkanClass->option.appName,	// app name
		WS_OVERLAPPEDWINDOW |	// window style
		WS_VISIBLE |
		WS_SYSMENU,
		100, 100,				// x/y coords
		wr.right - wr.left,     // width
		wr.bottom - wr.top,     // height
		NULL,					// handle to parent
		NULL,					// handle to menu
		hInstance,				// hInstance
		NULL);					// no extra parameters

	if (!hWnd) {
		throw runtime_error("Failed to create window");
		return false;
	}
	SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)this);

	return true;
}

LRESULT CALLBACK VulkanSurface::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	VulkanSurface* VulkanSurfaceClass =
		static_cast<VulkanSurface*>((VulkanSurface*)GetWindowLongPtr(hWnd, GWLP_USERDATA));

	switch (uMsg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		break;

	case WM_PAINT:

		return 0;

	case WM_SIZE:
		if (wParam != SIZE_MINIMIZED) {
			// SetWindowLongPtr will be called after WM_SIZE called as first
			if (VulkanSurfaceClass != NULL) {
				VulkanSurfaceClass->width = lParam & 0xffff;
				VulkanSurfaceClass->height = (lParam & 0xffff0000) >> 16;
			}
		}
		break;

	default:
		break;
	}

	return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}
#endif