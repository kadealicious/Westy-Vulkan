#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
#include<pthread_time.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include"h/input.h"
#include"h/window.h"
#include"h/audio.h"
#include"h/graphics.h"
#include"h/game.h"

// Enables or disables debug mode.  0 == off, 1 == on.
#define DEBUG 1
enum WS_APP_STATES { PAUSED, UNPAUSED, REQUEST_PAUSE, REQUEST_UNPAUSE, REQUEST_QUIT, INITIALIZE };

typedef struct wsTime
{
	struct timespec info;
	time_t start;
	time_t end;
	time_t delta;
	double deltaSecs;
	uint16_t numFrames;
}
wsTime;

#define max(x,y) (((x) >= (y)) ? (x) : (y))
#define min(x,y) (((x) <= (y)) ? (x) : (y))
void wsAppControlPauseQuit(uint8_t* appState);
void wsAppUpdateWindowTitle(GLFWwindow* windowPtr, uint16_t* numFrames);
void wsGLFWErrorCallback(int code, const char* description);

int main(int argc, char* argv[])
{
	printf("===BEGIN%s===\n", DEBUG ? " DEBUG" : "");
	
	// TODO: Implement these things.
	wsAudio audio = {};
	
	wsWindow window = {};
	GLFWwindow* windowPtr;
	uint8_t windowID;
	wsInput input = {};
	wsVulkan gfx = {};
	wsGame game = {};
	wsTime time = {};	// Restricts update rate for more consistent simulation.
	uint8_t appState = INITIALIZE;
	
	// Initialize GLFW window.
	wsWindowInit(&window);
	windowID = wsWindowCreate(640, 480);
	windowPtr = wsWindowGetPtr(windowID);
	
	wsInputInit(&input, windowID, 1.0f);	// Bind keyboard input to our GLFW window.
	wsVulkanInit(&gfx, windowID, DEBUG);
	wsGameInit(&game);
	wsGameSetCameraPointers(&gfx);	// TODO: This should be handled outside of game.c.
	
	printf("\n===START%s RUN===\n", DEBUG ? " DEGUB" : "");
	while(!glfwWindowShouldClose(windowPtr))
	{
		// Restrict simulation update rate.
		time.deltaSecs = max(time.delta * 0.000000001, 0.0);
		clock_gettime(CLOCK_REALTIME, &time.info);
		time.start = time.info.tv_nsec;
		
		// Poll input events and control app state.
		wsInputPreUpdate();
		wsAppControlPauseQuit(&appState);
		
		switch(appState)
		{
			case REQUEST_QUIT: 
				glfwSetWindowShouldClose(windowPtr, GLFW_TRUE);
				break;
			
			case REQUEST_PAUSE: 
				glfwSetInputMode(windowPtr, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				float windowCenterX = wsWindowGetWidth(windowID) / 2.0f;
				float windowCenterY = wsWindowGetHeight(windowID) / 2.0f;
				glfwSetCursorPos(windowPtr, windowCenterX, windowCenterY);
				wsInputResetMouseMove();
				break;
			
			case REQUEST_UNPAUSE: 
			case INITIALIZE: 
				glfwSetInputMode(windowPtr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				wsInputResetMouseMove();
				break;
			
			case UNPAUSED: 
				wsGameUpdateFPSCamera(time.deltaSecs);
				break;
			
			case PAUSED: 
			default: 
				break;
		}
		
		wsGameUpdate(time.deltaSecs);
		wsVulkanDrawFrame(time.deltaSecs);
		wsInputPostUpdate();
		
		// Gets executed once every ~1 second.
		if(time.delta < 0)
		{
			wsAppUpdateWindowTitle(windowPtr, &time.numFrames);
			if(DEBUG)
				{ printf("INFO: Time is %lld // %i fps\n", time.info.tv_sec, time.numFrames); }
		}

		clock_gettime(CLOCK_REALTIME, &time.info);
		time.end = time.info.tv_nsec;
		time.delta = (time.end - time.start);
		time.numFrames++;
	}
	printf("===STOP%s RUN===\n\n", DEBUG ? " DEBUG" : "");
	
	// Program exit procedure.
	wsCameraTerminate();
	wsVulkanTerminate();
	wsWindowTerminate();
	
	printf("===END%s===\n", DEBUG ? " DEBUG" : "");
	return 0;
}

void wsAppControlPauseQuit(uint8_t* appState)
{
	// Change pause/unpause requests into distinct "already paused" states.
	switch(*appState)
	{
		case REQUEST_UNPAUSE: 
		case INITIALIZE: 
			*appState = UNPAUSED;
			break;
		
		case REQUEST_PAUSE: 
			*appState = PAUSED;
			break;
		
		default: 
			break;
	}
	
	// Control quit behavior.
	if(wsInputGetKeyReleaseOnce(GLFW_KEY_ESCAPE))
	{
		printf("INFO: User has requested window should close!\n");
		*appState = REQUEST_QUIT;
		return;
	}
	
	// Control pause behavior.
	if(wsInputGetKeyReleaseOnce(GLFW_KEY_F1))
	{
		if(*appState == UNPAUSED)
		{
			printf("INFO: Westy paused!\n");
			*appState = REQUEST_PAUSE;
		}
		else
		{
			printf("INFO: Westy unpaused!\n");
			*appState = REQUEST_UNPAUSE;
		}
	}
}

void wsAppUpdateWindowTitle(GLFWwindow* windowPtr, uint16_t* numFrames)
{
	char title[69];	// Nice.
	sprintf(title, "Westy Vulkan - %i fps",  *numFrames);
	glfwSetWindowTitle(windowPtr, &title[0]);
	*numFrames = 0;
}