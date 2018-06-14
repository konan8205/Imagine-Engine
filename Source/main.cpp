
#include <vector>
#include <stdint.h>
#include <system_error>
#include <assert.h>

#include <iostream>
#include <cstdio>
#include <thread>

#include "Core/Console.h"
#include "Render/Vulkan/Vulkan.h"

using namespace std;

int main()
{
	VulkanCreateInfo vulkanCreateInfo;
	vulkanCreateInfo.appName = "Imagine Engine Sample";
	vulkanCreateInfo.appVersion = 1;

	Vulkan* vulkan = new Vulkan(vulkanCreateInfo);
	
	try {
		if (!vulkan->Initialize()) {
#if defined(_WIN32) && defined(_DEBUG)
			system("pause");
			return 0;
#endif
		}
	}
	catch (const std::runtime_error e){
		printf("[Error]\t%s\n", e.what());
#if defined(_WIN32) && defined(_DEBUG)
		system("pause");
		return 0;
#endif
	}
	
	while (vulkan->surfaceClass->Render())
	{
		try {
			vulkan->Render();
		}
		catch (const std::runtime_error e) {
			printf("[Error]\t%s\n", e.what());
#if defined(_WIN32) && defined(_DEBUG)
			system("pause");
			return 0;
		}
#endif
	}
	

	vulkan->DeInitialize();
	delete vulkan;

#if defined(_WIN32) && defined(_DEBUG)
	system("pause");
#endif
	return 0;
}