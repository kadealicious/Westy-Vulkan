#include<stdio.h>
#include<stdint.h>
#include"h/window.h"


wsWindow* wnd;	// Contains pointer to all GLFW window data.

// GLFW callback functions.
void wsWindowGLFWErrorCallback(int code, const char* description)
{
	printf("ERROR: GLFW code %i: %s\n", code, description);
}

// Call before wsVulkanInit().  Returns window ID.
int16_t wsWindowInit(uint16_t window_width, uint16_t window_height, wsWindow* window_data) 
{
	static uint8_t windowID = 0;// Current window ID.
	
	// Point to program data!!!
	wnd = window_data;
	
	// TODO: MAKE THIS WORK
	// If current windowID is already in use, find another!  Otherwise, return -1.
	/*for(uint8_t i = windowID; wnd->ptr != NULL; i++)
	{
		if(i >= NUM_MAX_WINDOWS)
		{
			printf("ERROR: You have reached the maximum number of GLFW windows at %i; failed to create another!\n", NUM_MAX_WINDOWS);
			return -1;
		}
	}*/
	
	glfwInit();
	
	glfwSetErrorCallback(&wsWindowGLFWErrorCallback);
	
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
	
	wnd->width[windowID] = window_width;
	wnd->height[windowID] = window_height;
	wnd->ptr[windowID] = glfwCreateWindow(wsWindowGetWidth(windowID), wsWindowGetHeight(windowID), "Westy Vulkan", NULL, NULL);
	printf("INFO: GLFW Window %i created: res %ix%i\n", windowID, window_width, window_height);
	
	return windowID++;
}
void wsWindowExit(uint8_t windowID)
{
	glfwDestroyWindow(wsWindowGetPtr(windowID));
	glfwTerminate();
	
	printf("INFO: GLFW Window %i destroyed!\n", windowID);
}

// [p]OOP rears its ugly head in the best possible way: getter methods.
uint16_t wsWindowGetWidth(uint8_t windowID)
{
	return wnd->width[windowID];
}

uint16_t wsWindowGetHeight(uint8_t windowID)
{
	return wnd->height[windowID];
}

GLFWwindow* wsWindowGetPtr(uint8_t windowID)
{
	return wnd->ptr[windowID];
}

int16_t wsWindowGetID(GLFWwindow* window) {
	for(int i = 0; i < NUM_MAX_WINDOWS; i++)
	{
		if(window == wsWindowGetPtr(i))
		{
			return i;
		}
	}
	
	return -1;
}