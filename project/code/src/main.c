#include<stdio.h>
#include"h/window.h"
#include"h/input.h"

#include <vulkan/vulkan.h>

#define CGLM_FORCE_RADIANS
#define CGLM_FORCE_DEPTH_ZERO_TO_ONE
#include<CGLM/vec4.h>
#include<CGLM/mat4.h>

#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>

// Contains all vulkan data.
typedef struct wsVulkan {
	VkInstance instance;
} wsVulkan;
wsVulkan vk;

void wsVulkanInit();
void wsRender();
void wsVulkanStop();

int main(int argc, char* argv[]) {
	printf("---Begin---\n");
	
	unsigned int windowID = wsWindowInit(640, 480);
	wsInputInit(windowID, 0.3f);
	wsVulkanInit();
	
	while(!glfwWindowShouldClose(wsWindowGetPtr(windowID))) {
		// Pre-logic-step.
		// ws
		
		// Logic step.
		// wsRun();
		
		// Post-logic-step.
		wsRender();
		wsInputUpdate();
	}
	
	wsVulkanStop();
	wsWindowExit(windowID);
	
	printf("---End---\n");
	// getchar();
	return 0;
}

// Call after wsWindowInit().
void wsVulkanInit() {
	uint32_t extension_count = 0;
	vkEnumerateInstanceExtensionProperties(NULL, &extension_count, NULL);
	printf("%i Vulkan extensions supported\n", extension_count);
	
	// Create Vulkan instance.
	VkApplicationInfo app_info;
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName = "Westy Vulkan";
	app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.pEngineName = "No Engine";
	app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.apiVersion = VK_API_VERSION_1_0;
	
	VkInstanceCreateInfo create_info;
	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_info.pApplicationInfo = &app_info;
	
	// Get desired platform-specific global extensions from GLFW.
	uint32_t glfw_extension_count = 0;
	const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
	
	create_info.enabledExtensionCount = glfw_extension_count;
	create_info.ppEnabledExtensionNames = glfw_extensions;
	create_info.enabledLayerCount = 0;
	
	VkResult result = vkCreateInstance(&create_info, NULL, &vk.instance);
	if(result != VK_SUCCESS)
		printf("ERROR: Vulkan instance creation failed!\n");
	else printf("Vulkan instance created!\n");
}

void wsRender() {
	
}

void wsVulkanStop() {
	vkDestroyInstance(vk.instance, NULL);
	printf("Vulkan instance destroyed!\n");
}