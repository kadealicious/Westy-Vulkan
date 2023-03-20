#ifndef INPUT_H_
#define INPUT_H_

#include<GLFW/glfw3.h>
#include<CGLM/cglm.h>


void wsInputInit(uint8_t windowID, float mouse_sensitivity);
void wsInputUpdate();

int wsInputGetKeyLast();
bool wsInputGetKeyHold(int key);
bool wsInputGetKeyPress(int key);
bool wsInputGetKeyRelease(int key);
bool wsInputGetKeyReleaseOnce(int key);
float wsInputGetMousePosX();
float wsInputGetMousePosY();
float wsInputGetMouseMoveX();
float wsInputGetMouseMoveY();
float wsInputGetMouseScroll();

void wsInputKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void wsInputCursorPosCallback(GLFWwindow* window, double posx, double posy);
void wsInputScrollCallback(GLFWwindow* window, double offsetx, double offsety);

#endif