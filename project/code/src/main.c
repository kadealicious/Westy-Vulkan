#include<stdio.h>
#include<stdbool.h>

#define CGLM_FORCE_RADIANS
#define CGLM_FORCE_DEPTH_ZERO_TO_ONE
#include<CGLM/vec4.h>
#include<CGLM/mat4.h>

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include"h/window.h"
#include"h/input.h"
#include"h/vulkan_interface.h"

// Enables or disables debug mode.  0 == off, 1 == on, 2 == verbose, 3 == too verbose.  Verbosity options are TODO.
#define DEBUG 1

void wsGLFWErrorCallback(int code, const char* description);

int main(int argc, char* argv[]) {
	printf("---Begin%s---\n", DEBUG ? " Debug" : "");
	
	// Initialize GLFW and related components.
	uint8_t windowID = wsWindowInit(640, 480);
	GLFWwindow* window = wsWindowGetPtr(windowID);
	glfwSetErrorCallback(&wsGLFWErrorCallback);
	wsInputInit(windowID, 0.3f);
	
	// Initialize Vulkan.
	VkInstance instanceVK;	// Main Vulkan instance.
	VkSurfaceKHR surfaceVK;	// Window surface for drawing.
	VkPhysicalDevice gpuVK = NULL;	// Primary physical device.  Implicitly destroyed when Vulkan instance is destroyed.
	VkDevice logical_gpuVK = NULL;	// Primary logical device used to interface with the physical device.
	VkDebugUtilsMessengerEXT debug_msgrVK;	// Main debug messenger.
	wsVulkanSetDebug(DEBUG);
	wsVulkanInit(&instanceVK, &surfaceVK, windowID, &gpuVK, &logical_gpuVK, &debug_msgrVK);
	
	// Main loop.
	printf("\n---Start%s Run---\n", DEBUG ? " Debug" : "");
	while(!glfwWindowShouldClose(window)) {
		// Pre-logic-step.
		// ws
		
		// Logic step.
		// wsRun();
		
		// Post-logic-step.
		// wsRender();
		wsInputUpdate();
		
		// Should the program close?
		if(wsInputGetKeyRelease(GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}
	}
	printf("---Stop%s Run---\n\n", DEBUG ? " Debug" : "");
	
	// Program exit procedure.
	wsVulkanStop(&instanceVK, &logical_gpuVK, &debug_msgrVK);
	wsWindowExit(windowID);
	
	printf("---End%s---\n", DEBUG ? " Debug" : "");
	// getchar();
	return 0;
}

void wsGLFWErrorCallback(int code, const char* description) {
	printf("ERROR: GLFW code %i: %s\n", code, description);
}