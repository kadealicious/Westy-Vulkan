#include<stdio.h>
#include<stdbool.h>

#define CGLM_FORCE_RADIANS
#define CGLM_FORCE_DEPTH_ZERO_TO_ONE
#include<CGLM/vec4.h>
#include<CGLM/mat4.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include"h/window.h"
#include"h/input.h"
#include"h/vulkan_interface.h"

// Enables or disables debug mode.  0 == off, 1 == on, 2 == verbose, 3 == too verbose.  Verbosity options are TODO.
#define DEBUG 1

void wsGLFWErrorCallback(int code, const char* description);

int main(int argc, char* argv[]) {
	printf("===Begin%s===\n", DEBUG ? " Debug" : "");
	
	// Initialize GLFW and related components.
	uint8_t windowID = wsWindowInit(640, 480);
	GLFWwindow* window = wsWindowGetPtr(windowID);
	glfwSetErrorCallback(&wsGLFWErrorCallback);
	wsInputInit(windowID, 0.3f);
	
	// Initialize Vulkan.
	wsVulkan vk = {};
	wsVulkanSetDebug(DEBUG);
	wsVulkanInit(&vk, windowID);
	
	// Main loop.
	printf("\n===Start%s Run===\n", DEBUG ? " Debug" : "");
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
	printf("===Stop%s Run===\n\n", DEBUG ? " Debug" : "");
	
	// Program exit procedure.
	wsVulkanStop(&vk);
	wsWindowExit(windowID);
	
	printf("===End%s===\n", DEBUG ? " Debug" : "");
	// getchar();
	return 0;
}

void wsGLFWErrorCallback(int code, const char* description) {
	printf("ERROR: GLFW code %i: %s\n", code, description);
}