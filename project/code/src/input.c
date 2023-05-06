#include<stdio.h>
#include<string.h>
#include"h/input.h"
#include"h/window.h"

// Misc. input data.
typedef struct wsInput
{
	uint8_t windowID;
}
wsInput;

// Keyboard data.
typedef struct wsKeyboard
{
	int key_last;
}
wsKeyboard;

// Mouse data.
typedef struct wsMouse
{
	vec2 position;
	vec2 movement;
	float sensitivity;
	float scroll;
	bool first_move;
}
wsMouse;

wsInput input;
wsKeyboard keys;
wsMouse mouse;


void wsInputInit(uint8_t windowID, float mouse_sensitivity) {
	input.windowID = windowID;
    mouse.sensitivity = (mouse_sensitivity / 1000.0f);
	mouse.scroll = 0.0f;
	mouse.first_move = true;
	
	keys.key_last = -1;
	
	GLFWwindow* window_ptr = wsWindowGetPtr(windowID);
	glfwSetKeyCallback(window_ptr, wsInputKeyCallback);
	glfwSetCursorPosCallback(window_ptr, wsInputCursorPosCallback);
	glfwSetScrollCallback(window_ptr, wsInputScrollCallback);
	glfwSetInputMode(window_ptr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPos(window_ptr, wsWindowGetWidth(input.windowID) / 2.0f, wsWindowGetHeight(input.windowID) / 2.0f);

    printf("INFO: Input initialized: window ID %i, mouse sens %f\n", windowID, mouse.sensitivity);
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

int wsInputGetKeyLast()	{ return keys.key_last; }
bool wsInputGetKeyHold(int key)	{ return (glfwGetKey(wsWindowGetPtr(input.windowID), key) == GLFW_PRESS); }
bool wsInputGetKeyPress(int key)	{ return (glfwGetKeyOnce(wsWindowGetPtr(input.windowID), key) == GLFW_PRESS); }
bool wsInputGetKeyRelease(int key)	{ return (glfwGetKey(wsWindowGetPtr(input.windowID), key) == GLFW_RELEASE) && (key == keys.key_last); }
bool wsInputGetKeyReleaseOnce(int key)	{ return (glfwGetKeyReleasedOnce(wsWindowGetPtr(input.windowID), key) == GLFW_PRESS); }

float wsInputGetMousePosX()		{ return mouse.position[0]; }
float wsInputGetMousePosY()		{ return mouse.position[1]; }
float wsInputGetMouseMoveX()	{ return mouse.movement[0]; }
float wsInputGetMouseMoveY()	{ return mouse.movement[1]; }
float wsInputGetMouseScroll()	{ return mouse.scroll; }
void wsInputResetMouseMove()		{ mouse.movement[0] = 0.0f; mouse.movement[1] = 0.0f; }

// Automatically calls glfwPollEvents().
void wsInputPreUpdate()
{
	glfwPollEvents();
}
void wsInputPostUpdate()
{
	mouse.movement[0] = 0.0f;
	mouse.movement[1] = 0.0f;
}

void wsInputKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	keys.key_last = key;
}

void wsInputCursorPosCallback(GLFWwindow* window, double posx, double posy) {
	static float posx_prev;
	static float posy_prev;
	
	if(mouse.first_move) {
		posx_prev = posx;
		posy_prev = posy;
		mouse.first_move = false;
	}
	
	float offsetx = posx - posx_prev;
	float offsety = posy_prev - posy;
	posx_prev = posx;
	posy_prev = posy;
	offsetx *= mouse.sensitivity;
	offsety *= mouse.sensitivity;
	
	mouse.movement[0] = offsetx;
	mouse.movement[1] = offsety;
	mouse.position[0] += mouse.movement[0];
	mouse.position[1] += mouse.movement[1];
}

void wsInputScrollCallback(GLFWwindow* window, double offsetx, double offsety) {
	mouse.scroll -= (float)offsety;
	if(mouse.scroll < -100.0f)
		mouse.scroll = -100.0f;
	else if(mouse.scroll > 100.0f)
		mouse.scroll = 100.0f;
}