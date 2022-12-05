#ifndef WINDOW_H_
#define WINDOW_H_

#define NUM_MAX_WINDOWS 1

#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>

unsigned int wsWindowGetWidth(unsigned int);
unsigned int wsWindowGetHeight(unsigned int);
GLFWwindow* wsWindowGetPtr(unsigned int);

unsigned int wsWindowInit(unsigned int, unsigned int);
int wsWindowExit(unsigned int);

// Contains all GLFW window information.
typedef struct wsWindow {
	uint32_t width[NUM_MAX_WINDOWS];
	uint32_t height[NUM_MAX_WINDOWS];
	GLFWwindow* ptr[NUM_MAX_WINDOWS];
} wsWindow;

#endif