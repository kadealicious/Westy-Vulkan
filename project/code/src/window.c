#include"h/window.h"
#include<stdio.h>

// Stores all GLFW window-related data.
wsWindow windows;

unsigned int wsWindowGetWidth(unsigned int windowID) {
	return windows.width[windowID];
}

unsigned int wsWindowGetHeight(unsigned int windowID) {
	return windows.height[windowID];
}

GLFWwindow* wsWindowGetPtr(unsigned int windowID) {
	return windows.ptr[windowID];
}

// Call before wsVulkanInit().  Returns window ID.
unsigned int wsWindowInit(unsigned int win_width, unsigned int win_height) {
	static unsigned int windowID = 0;// Current window ID.
	
	glfwInit();
	
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	windows.ptr[windowID] = glfwCreateWindow(wsWindowGetWidth(windowID), wsWindowGetHeight(windowID), "Vulkan Window!", NULL, NULL);
	printf("Window %i created!\n", windowID);
	
	windowID++;
	return windowID - 1;
}

int wsWindowExit(unsigned int windowID) {
	glfwDestroyWindow(wsWindowGetPtr(windowID));
	glfwTerminate();
	
	return 0;
}