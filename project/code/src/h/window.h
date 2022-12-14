#ifndef WINDOW_H_
#define WINDOW_H_

#define NUM_MAX_WINDOWS 1

#include<GLFW/glfw3.h>

uint16_t wsWindowGetWidth(uint8_t windowID);
uint16_t wsWindowGetHeight(uint8_t windowID);
GLFWwindow* wsWindowGetPtr(uint8_t windowID);
int8_t wsWindowGetID(GLFWwindow* window);	// Returns -1 if window not found.

uint8_t wsWindowInit(uint16_t window_width, uint16_t window_height);
void wsWindowExit(uint8_t windowID);

#endif