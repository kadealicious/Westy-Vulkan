#include<stdio.h>
#include"h/window.h"

// Contains all GLFW windows' data.
uint32_t width[NUM_MAX_WINDOWS];
uint32_t height[NUM_MAX_WINDOWS];
GLFWwindow* window_ptr[NUM_MAX_WINDOWS];

unsigned int wsWindowGetWidth(unsigned int windowID) {
	return width[windowID];
}

unsigned int wsWindowGetHeight(unsigned int windowID) {
	return height[windowID];
}

GLFWwindow* wsWindowGetPtr(unsigned int windowID) {
	return window_ptr[windowID];
}

// Call before wsVulkanInit().  Returns window ID.
unsigned int wsWindowInit(unsigned int window_width, unsigned int window_height) {
	static unsigned int windowID = 0;// Current window ID.
	
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

void wsWindowExit(unsigned int windowID) {
	glfwDestroyWindow(wsWindowGetPtr(windowID));
	glfwTerminate();
	
	printf("Window %i destroyed!\n", windowID);
}