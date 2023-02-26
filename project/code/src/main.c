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
#include"h/model.h"


// Enables or disables debug mode.  0 == off, 1 == on, 2 == verbose, 3 == too verbose.  Verbosity options are TODO.
#define DEBUG 1


void wsGLFWErrorCallback(int code, const char* description);


int main(int argc, char* argv[]) {
	printf("===BEGIN%s===\n", DEBUG ? " DEBUG" : "");
	
	
	// Program data struct 0-initialization: 
	wsWindow wnd = {};
	wsVulkan vk = {};
	wsModel md = {};
	
	
	// Initialize GLFW.
	uint8_t windowID = wsWindowInit(640, 480, &wnd);
	GLFWwindow* window = wsWindowGetPtr(windowID);
	wsInputInit(windowID, 0.3f);	// Bind keyboard input to our GLFW window.
	
	wsModelInit(&md);	// Allow models to exist for Vulkan.
	// Initialize Vulkan.
	wsVulkanSetDebug(DEBUG);
	wsVulkanInit(&vk, windowID);
	
	
	// Main loop.
	printf("\n===START%s RUN===\n", DEBUG ? " DEGUB" : "");
	
	while(!glfwWindowShouldClose(window)) {
		
		// Pre-logic-step.
		wsInputUpdate();
		if(wsInputGetKeyReleaseOnce(GLFW_KEY_ESCAPE)) {
			printf("INFO: User has requested window should close!\n");
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}
		
		
		// Logic step.
		
		
		// Post-logic step.
		wsVulkanDrawFrame(&vk);
		
		
	}
	printf("===STOP%s RUN===\n\n", DEBUG ? " DEBUG" : "");
	
	
	// Program exit procedure.
	wsModelStop();
	wsVulkanStop(&vk);
	wsWindowExit(windowID);
	
	
	printf("===END%s===\n", DEBUG ? " DEBUG" : "");
	return 0;
}