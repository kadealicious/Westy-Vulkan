#ifndef INPUT_H_
#define INPUT_H_

#include<GLFW/glfw3.h>
#include<CGLM/cglm.h>

// Function for key something.
char key_once[GLFW_KEY_LAST + 1];
#define glfwGetKeyOnce(window, key)             \
    (glfwGetKey(window, key) ?              \
     (key_once[key] ? false : (key_once[key] = true)) :   \
     (key_once[key] = false))

void wsInputInit(unsigned int windowID, float mouse_sensitivity);
void wsInputUpdate();

int wsInputGetKeyLast();
bool wsInputGetKeyHold(int key);
bool wsInputGetKeyPress(int key);
bool wsInputGetKeyRelease(int key);
float wsInputGetMousePosX();
float wsInputGetMousePosY();
float wsInputGetMouseMoveX();
float wsInputGetMouseMoveY();
float wsInputGetMouseScroll();

void wsInputKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void wsInputCursorPosCallback(GLFWwindow* window, double posx, double posy);
void wsInputScrollCallback(GLFWwindow* window, double offsetx, double offsety);

#endif