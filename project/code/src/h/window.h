#ifndef WINDOW_H_
#define WINDOW_H_

#define NUM_MAX_WINDOWS 1

#include<GLFW/glfw3.h>

unsigned int wsWindowGetWidth(unsigned int windowID);
unsigned int wsWindowGetHeight(unsigned int windowID);
GLFWwindow* wsWindowGetPtr(unsigned int windowID);

unsigned int wsWindowInit(unsigned int window_width, unsigned int window_height);
void wsWindowExit(unsigned int windowID);

#endif