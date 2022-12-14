#include<stdio.h>
#include"h/window.h"

// Contains all GLFW windows' data.
uint32_t width[NUM_MAX_WINDOWS];
uint32_t height[NUM_MAX_WINDOWS];
GLFWwindow* window_ptr[NUM_MAX_WINDOWS];

uint16_t wsWindowGetWidth(uint8_t windowID) {
	return width[windowID];
}

uint16_t wsWindowGetHeight(uint8_t windowID) {
	return height[windowID];
}

GLFWwindow* wsWindowGetPtr(uint8_t windowID) {
	return window_ptr[windowID];
}

int8_t wsWindowGetID(GLFWwindow* window) {
	for(int i = 0; i < NUM_MAX_WINDOWS; i++) {
		if(window == wsWindowGetPtr(i)) {
			return i;
		}
	}
	
	return -1;
}

// Call before wsVulkanInit().  Returns window ID.
uint8_t wsWindowInit(uint16_t window_width, uint16_t window_height) {
	static uint8_t windowID = 0;// Current window ID.
	
	glfwInit();
	
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
	
	width[windowID] = window_width;
	height[windowID] = window_height;
	window_ptr[windowID] = glfwCreateWindow(wsWindowGetWidth(windowID), wsWindowGetHeight(windowID), "Westy Vulkan", NULL, NULL);
	printf("Window %i created: res %ix%i\n", windowID, window_width, window_height);
	
	windowID++;
	return windowID - 1;
}

void wsWindowExit(uint8_t windowID) {
	glfwDestroyWindow(wsWindowGetPtr(windowID));
	glfwTerminate();
	
	printf("Window %i destroyed!\n", windowID);
}