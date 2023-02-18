#include<stdio.h>
#include<string.h>
#include"h/input.h"
#include"h/window.h"

// Keyboard data.
int key_last;

// Mouse data.
vec2 mouse_position;
vec2 mouse_position_prev;
float mouse_sensitivity;
float mouse_scroll;
bool mouse_first_move;
uint8_t windowID;

void wsInputInit(uint8_t windowID, float mouse_sensitivity) {
	windowID = windowID;
    mouse_sensitivity = mouse_sensitivity;
	mouse_scroll = 0.0f;
	mouse_first_move = true;
	
	key_last = -1;
	
	glfwSetKeyCallback(wsWindowGetPtr(windowID), wsInputKeyCallback);
	glfwSetCursorPosCallback(wsWindowGetPtr(windowID), wsInputCursorPosCallback);
	glfwSetScrollCallback(wsWindowGetPtr(windowID), wsInputScrollCallback);

    printf("INFO: Input initialized: window ID %i, mouse sens %f\n", windowID, mouse_sensitivity);
}

int wsInputGetKeyLast()	{ return key_last; }
bool wsInputGetKeyHold(int key)	{ return (glfwGetKey(wsWindowGetPtr(windowID), key) == GLFW_PRESS); }
bool wsInputGetKeyPress(int key)	{ return (glfwGetKeyOnce(wsWindowGetPtr(windowID), key) == GLFW_PRESS); }
bool wsInputGetKeyRelease(int key)	{ return (glfwGetKey(wsWindowGetPtr(windowID), key) == GLFW_RELEASE) && (key == key_last); }
bool wsInputGetKeyReleaseOnce(int key)	{ return (glfwGetKeyReleasedOnce(wsWindowGetPtr(windowID), key) == GLFW_PRESS); }

float wsInputGetMousePosX()		{ return mouse_position[0]; }
float wsInputGetMousePosY()		{ return mouse_position[1]; }
float wsInputGetMouseMoveX()	{ return mouse_position[0] - mouse_position_prev[0]; }
float wsInputGetMouseMoveY()	{ return mouse_position[1] - mouse_position_prev[1]; }
float wsInputGetMouseScroll()	{ return mouse_scroll; }

// Automatically calls glfwPollEvents().
void wsInputUpdate() {
	mouse_position_prev[0] = mouse_position[0];
	mouse_position_prev[1] = mouse_position[1];
	
	glfwPollEvents();
}

void wsInputKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	key_last = key;
}

void wsInputCursorPosCallback(GLFWwindow* window, double posx, double posy) {
	static float posx_prev;
	static float posy_prev;
	
	if(mouse_first_move) {
		posx_prev = posx;
		posy_prev = posy;
		mouse_first_move = false;
	}
	
	float offsetx = posx - posx_prev;
	float offsety = posy_prev - posy;
	posx_prev = posx;
	posy_prev = posy;
	offsetx *= mouse_sensitivity;
	offsety *= mouse_sensitivity;
	mouse_position[0] += offsetx;
	mouse_position[1] += offsety;
}

void wsInputScrollCallback(GLFWwindow* window, double offsetx, double offsety) {
	mouse_scroll -= (float)offsety;
	if(mouse_scroll < -100.0f)
		mouse_scroll = -100.0f;
	else if(mouse_scroll > 100.0f)
		mouse_scroll = 100.0f;
}