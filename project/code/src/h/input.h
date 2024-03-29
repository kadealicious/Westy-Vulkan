#ifndef INPUT_H_
#define INPUT_H_

#include<GLFW/glfw3.h>
#include<CGLM/cglm.h>

// Mouse data.
typedef struct wsMouse
{
	vec2 position;
	vec2 movement;
	float sensitivity;
	float scroll;
	bool firstMove;
}
wsMouse;
// Container for all input.
typedef struct wsInput
{
	uint8_t windowID;
	int keyLast;
	wsMouse mouse;
}
wsInput;

void wsInputInit(wsInput* inputData, uint8_t windowID, float mouse_sensitivity);
void wsInputPreUpdate();
void wsInputPostUpdate();

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
void wsInputResetMouseMove();

void wsInputKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void wsInputCursorPosCallback(GLFWwindow* window, double posx, double posy);
void wsInputScrollCallback(GLFWwindow* window, double offsetx, double offsety);

#endif