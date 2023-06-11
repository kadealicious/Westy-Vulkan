#include<stdio.h>
#include<string.h>
#include"h/input.h"
#include"h/window.h"

wsInput* input;

void wsInputInit(wsInput* inputData, uint8_t windowID, float mouse_sensitivity)
{
	// This is important.  Lol.
	input = inputData;
	
	input->windowID = windowID;
    input->mouse.sensitivity = mouse_sensitivity;
	input->mouse.scroll = 0.0f;
	input->mouse.firstMove = true;
	
	input->keyLast = -1;
	
	GLFWwindow* window_ptr = wsWindowGetPtr(windowID);
	glfwSetKeyCallback(window_ptr, wsInputKeyCallback);
	glfwSetCursorPosCallback(window_ptr, wsInputCursorPosCallback);
	glfwSetScrollCallback(window_ptr, wsInputScrollCallback);
	glfwSetInputMode(window_ptr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPos(window_ptr, wsWindowGetWidth(input->windowID) / 2.0f, wsWindowGetHeight(input->windowID) / 2.0f);

    printf("INFO: Input initialized: window ID %i, mouse sens %f\n", windowID, input->mouse.sensitivity);
}

// Function for getting initial impulse of key press.
char key_once[GLFW_KEY_LAST + 1];
#define glfwGetKeyOnce(window, key)             \
    (glfwGetKey(window, key) ?              \
     (key_once[key] ? false : (key_once[key] = true)) :   \
     (key_once[key] = false))

// Function for getting initial impulse of key release.
char key_nunce[GLFW_KEY_LAST + 1];
#define glfwGetKeyReleasedOnce(window, key)             \
    ((!glfwGetKey(window, key) && wsInputGetKeyLast() == key) ?              \
     (key_nunce[key] ? false : (key_nunce[key] = true)) :   \
     (key_nunce[key] = false))

int wsInputGetKeyLast()					{ return input->keyLast; }
bool wsInputGetKeyHold(int key)			{ return (glfwGetKey(wsWindowGetPtr(input->windowID), key) == GLFW_PRESS); }
bool wsInputGetKeyPress(int key)		{ return (glfwGetKeyOnce(wsWindowGetPtr(input->windowID), key) == GLFW_PRESS); }
bool wsInputGetKeyRelease(int key)		{ return (glfwGetKey(wsWindowGetPtr(input->windowID), key) == GLFW_RELEASE) && (key == input->keyLast); }
bool wsInputGetKeyReleaseOnce(int key)	{ return (glfwGetKeyReleasedOnce(wsWindowGetPtr(input->windowID), key) == GLFW_PRESS); }

float wsInputGetMousePosX()		{ return input->mouse.position[0]; }
float wsInputGetMousePosY()		{ return input->mouse.position[1]; }
float wsInputGetMouseMoveX()	{ return input->mouse.movement[0]; }
float wsInputGetMouseMoveY()	{ return input->mouse.movement[1]; }
float wsInputGetMouseScroll()	{ return input->mouse.scroll; }
void wsInputResetMouseMove()	{ input->mouse.movement[0] = 0.0f; input->mouse.movement[1] = 0.0f; }

// Automatically calls glfwPollEvents().
void wsInputPreUpdate()
{
	glfwPollEvents();
}
void wsInputPostUpdate()
{
	input->mouse.movement[0] = 0.0f;
	input->mouse.movement[1] = 0.0f;
}

void wsInputKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	input->keyLast = key;
}

void wsInputCursorPosCallback(GLFWwindow* window, double posx, double posy)
{
	static float posx_prev;
	static float posy_prev;
	
	if(input->mouse.firstMove)
	{
		posx_prev = posx;
		posy_prev = posy;
		input->mouse.firstMove = false;
	}
	
	float offsetx = posx - posx_prev;
	float offsety = posy_prev - posy;
	posx_prev = posx;
	posy_prev = posy;
	offsetx *= input->mouse.sensitivity;
	offsety *= input->mouse.sensitivity;
	
	input->mouse.movement[0] = offsetx;
	input->mouse.movement[1] = offsety;
	input->mouse.position[0] += input->mouse.movement[0];
	input->mouse.position[1] += input->mouse.movement[1];
}

void wsInputScrollCallback(GLFWwindow* window, double offsetx, double offsety)
{
	input->mouse.scroll -= (float)offsety;
	if(input->mouse.scroll < -100.0f)
		input->mouse.scroll = -100.0f;
	else if(input->mouse.scroll > 100.0f)
		input->mouse.scroll = 100.0f;
}