#include<stdio.h>
#include<string.h>
#include"h/input.h"
#include"h/window.h"

typedef struct wsInput {
	vec2 mouse_position;
	vec2 mouse_position_prev;
	float mouse_sensitivity;
	float mouse_scroll;
	bool mouse_first_move;
	unsigned int windowID;
} wsInput;
wsInput input;

void wsInputInit(unsigned int windowID, float mouse_sensitivity) {
	input.windowID = windowID;
    input.mouse_sensitivity = mouse_sensitivity;
	input.mouse_scroll = 0.0f;
	input.mouse_first_move = true;
	
	glfwSetKeyCallback(wsWindowGetPtr(windowID), wsInputKeyCallback);
	glfwSetCursorPosCallback(wsWindowGetPtr(windowID), wsInputCursorPosCallback);
	glfwSetScrollCallback(wsWindowGetPtr(windowID), wsInputScrollCallback);

    printf("Input initialized: window ID %i, mouse sens %f\n", windowID, mouse_sensitivity);
}

bool wsInputGetHold(int key)	{ return (glfwGetKey(wsWindowGetPtr(input.windowID), key) == GLFW_PRESS); }
bool wsInputGetPress(int key)	{ return (glfwGetKeyOnce(wsWindowGetPtr(input.windowID), key) == GLFW_PRESS); }
bool wsInputGetRelease(int key)	{ return (glfwGetKey(wsWindowGetPtr(input.windowID), key) == GLFW_RELEASE); }

float wsInputGetMousePosX()		{ return input.mouse_position[0]; }
float wsInputGetMousePosY()		{ return input.mouse_position[1]; }
float wsInputGetMouseMoveX()	{ return input.mouse_position[0] - input.mouse_position_prev[0]; }
float wsInputGetMouseMoveY()	{ return input.mouse_position[1] - input.mouse_position_prev[1]; }
float wsInputGetMouseScroll()	{ return input.mouse_scroll; }

// Automatically calls glfwPollEvents().
void wsInputUpdate() {
	input.mouse_position_prev[0] = input.mouse_position[0];
	input.mouse_position_prev[1] = input.mouse_position[1];
	
	glfwPollEvents();
}

void wsInputKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	
}

void wsInputCursorPosCallback(GLFWwindow* window, double posx, double posy) {
	static float posx_prev;
	static float posy_prev;
	
	if(input.mouse_first_move) {
		posx_prev = posx;
		posy_prev = posy;
		input.mouse_first_move = false;
	}
	
	float offsetx = posx - posx_prev;
	float offsety = posy_prev - posy;
	posx_prev = posx;
	posy_prev = posy;
	offsetx *= input.mouse_sensitivity;
	offsety *= input.mouse_sensitivity;
	input.mouse_position[0] += offsetx;
	input.mouse_position[1] += offsety;
}

void wsInputScrollCallback(GLFWwindow* window, double offsetx, double offsety) {
	input.mouse_scroll -= (float)offsety;
	if(input.mouse_scroll < -100.0f)
		input.mouse_scroll = -100.0f;
	else if(input.mouse_scroll > 100.0f)
		input.mouse_scroll = 100.0f;
}