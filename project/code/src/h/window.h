#ifndef WINDOW_H_
#define WINDOW_H_

#define NUM_MAX_WINDOWS 1

#include<GLFW/glfw3.h>
#include"vulkan_interface.h"

typedef struct wsWindow {
    
    GLFWwindow* ptr[NUM_MAX_WINDOWS];
    
    uint16_t width[NUM_MAX_WINDOWS];
    uint16_t height[NUM_MAX_WINDOWS];
    
} wsWindow;

uint16_t wsWindowGetWidth(uint8_t windowID);
uint16_t wsWindowGetHeight(uint8_t windowID);
GLFWwindow* wsWindowGetPtr(uint8_t windowID);
int16_t wsWindowGetID(GLFWwindow* window);	// Returns -1 if window not found.

int16_t wsWindowInit(uint16_t window_width, uint16_t window_height, wsWindow* window_data);
void wsWindowExit(uint8_t windowID);

#endif