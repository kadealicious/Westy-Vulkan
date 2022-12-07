#include<stdio.h>
#include<stdbool.h>

#define CGLM_FORCE_RADIANS
#define CGLM_FORCE_DEPTH_ZERO_TO_ONE
#include<CGLM/vec4.h>
#include<CGLM/mat4.h>

#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>

#include"h/window.h"
#include"h/input.h"
#include"h/vulkan_interface.h"

// Enables or disables debug mode.
#define DEBUG true

void wsRender();

int main(int argc, char* argv[]) {
	printf("---Begin---\n");
	printf("Debug mode %s\n", DEBUG ? "on!" : "off!");
	
	unsigned int windowID = wsWindowInit(640, 480);
	GLFWwindow* window = wsWindowGetPtr(windowID);
	
	wsInputInit(windowID, 0.3f);
	wsVulkanInit(DEBUG);
	
	printf("\n---Start Run---\n");
	while(!glfwWindowShouldClose(window)) {
		// Pre-logic-step.
		// ws
		
		// Logic step.
		// wsRun();
		
		// Post-logic-step.
		wsRender();
		wsInputUpdate();
		
		if(wsInputGetKeyRelease(GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}
	}
	printf("---Stop Run---\n\n");
	
	wsVulkanStop();
	wsWindowExit(windowID);
	
	printf("---End---\n");
	// getchar();
	return 0;
}

void wsRender() {
	
}

