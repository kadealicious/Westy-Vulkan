#include<stdio.h>
#include<stdint.h>
#include"h/window.h"

// Contains pointer to all GLFW window data.
wsWindow* window;

void wsWindowGLFWErrorCallback(int code, const char* description);

// Call before wsVulkanInit().
void wsWindowInit(wsWindow* window_data) 
{
	window = window_data;
	
	glfwInit();
	glfwSetErrorCallback(&wsWindowGLFWErrorCallback);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
}
void wsWindowTerminate()
{
	for(uint8_t i = 0; i < WS_MAX_NUM_WINDOWS; i++)
	{
		if(window->isActive[i])
		{
			wsWindowExit(i);
		}
	}
	
	glfwTerminate();
	printf("INFO: GLFW terminated and all windows closed!\n");
}

int16_t wsWindowCreate(uint16_t window_width, uint16_t window_height)
{
	static uint8_t windowID = 0;
	
	window->width[windowID] = window_width;
	window->height[windowID] = window_height;
	window->ptr[windowID] = glfwCreateWindow(wsWindowGetWidth(windowID), wsWindowGetHeight(windowID), "The Rewritten Kitten is Smitten", NULL, NULL);
	printf("INFO: GLFW Window %i created: res %ix%i\n", windowID, window_width, window_height);
	
	return windowID++;
}
void wsWindowExit(uint8_t windowID)
{
	glfwDestroyWindow(wsWindowGetPtr(windowID));
	printf("INFO: GLFW Window %i destroyed!\n", windowID);
}

// [p]OOP rears its ugly head in the best possible way: getter methods.
uint16_t wsWindowGetWidth(uint8_t windowID)
{
	return window->width[windowID];
}

uint16_t wsWindowGetHeight(uint8_t windowID)
{
	return window->height[windowID];
}

GLFWwindow* wsWindowGetPtr(uint8_t windowID)
{
	return window->ptr[windowID];
}

int16_t wsWindowGetID(GLFWwindow* window)
{
	for(int i = 0; i < WS_MAX_NUM_WINDOWS; i++)
	{
		if(window == wsWindowGetPtr(i))
		{
			return i;
		}
	}
	return -1;
}

void wsWindowGLFWErrorCallback(int code, const char* description)
{
	printf("ERROR: GLFW code %i: %s\n", code, description);
}