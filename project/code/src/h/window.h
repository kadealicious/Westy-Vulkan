#ifndef WINDOW_H_
#define WINDOW_H_

#define WS_MAX_NUM_WINDOWS 10

#include<GLFW/glfw3.h>
#include"graphics.h"

typedef struct wsWindow {
    
    GLFWwindow* ptr[WS_MAX_NUM_WINDOWS];
    uint16_t width[WS_MAX_NUM_WINDOWS];
    uint16_t height[WS_MAX_NUM_WINDOWS];
	bool isActive[WS_MAX_NUM_WINDOWS];
    
} wsWindow;

uint16_t wsWindowGetWidth(uint8_t windowID);
uint16_t wsWindowGetHeight(uint8_t windowID);
GLFWwindow* wsWindowGetPtr(uint8_t windowID);
int16_t wsWindowGetID(GLFWwindow* window);	// Returns -1 if window not found.

void wsWindowInit(wsWindow* window_data);
void wsWindowTerminate();
int16_t wsWindowCreate(uint16_t window_width, uint16_t window_height);
void wsWindowExit(uint8_t windowID);

#endif