#include<stdio.h>
#include<stdbool.h>
#include<time.h>
#include<stdlib.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include"h/window.h"
#include"h/input.h"
#include"h/vulkan_interface.h"
#include"h/mesh.h"
#include"h/camera.h"

// Enables or disables debug mode.  0 == off, 1 == on, 2 == verbose, 3 == too verbose.  Verbosity options are TODO.
#define DEBUG 1

#define max(x,y) (((x) >= (y)) ? (x) : (y))
#define min(x,y) (((x) <= (y)) ? (x) : (y))

void wsGLFWErrorCallback(int code, const char* description);


int main(int argc, char* argv[])
{
	printf("===BEGIN%s===\n", DEBUG ? " DEBUG" : "");
	
	
	// Program data struct 0-initialization: 
	wsWindow wnd = {};
	wsVulkan vk = {};
	wsMesh md = {};		// Managed in vulkan_interface.c.
	wsCamera cm = {};
	
	
	// Initialize GLFW.
	uint8_t windowID = wsWindowInit(640, 480, &wnd);
	GLFWwindow* window_ptr = wsWindowGetPtr(windowID);
	wsInputInit(windowID, 1.0f);					// Bind keyboard input to our GLFW window.
	
	// Initialize cameras!
	wsCameraInit(&cm);
	uint8_t camera_main = wsCameraCreate();
	
	// Create meshes!
	wsMeshInit(&md);
	
	// Initialize Vulkan.
	wsVulkanSetDebug(DEBUG);
	wsVulkanInit(&vk, &md, &cm, windowID);
	
	
	// Main loop.
	printf("\n===START%s RUN===\n", DEBUG ? " DEGUB" : "");
	
	struct timespec time_info = {};
	time_t time_start = 0;
	time_t time_end = 0;
	time_t time_delta = 0;
	double time_delta_adj = 0;
	uint16_t num_frames = 0;
	
	bool is_paused = false;
	
	while(!glfwWindowShouldClose(window_ptr))
	{
		// Pre-logic-step.
		time_delta_adj = max(time_delta * 0.000000001, 0.0);
		timespec_get(&time_info, TIME_UTC);
		time_start = time_info.tv_nsec;
		
		// Poll input events through GLFW.
		wsInputPreUpdate();
		if(wsInputGetKeyReleaseOnce(GLFW_KEY_ESCAPE))
		{
			printf("INFO: User has requested window should close!\n");
			glfwSetWindowShouldClose(window_ptr, GLFW_TRUE);
		}
		if(wsInputGetKeyReleaseOnce(GLFW_KEY_F1))
		{
			if(!is_paused)
			{
				printf("INFO: Westy paused!\n");
				glfwSetInputMode(window_ptr, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				glfwSetCursorPos(window_ptr, wsWindowGetWidth(windowID) / 2.0f, wsWindowGetHeight(windowID) / 2.0f);
				wsInputResetMouseMove();
			}
			else
			{
				printf("INFO: Westy unpaused!\n");
				glfwSetInputMode(window_ptr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				wsInputResetMouseMove();
			}
			is_paused = !is_paused;
		}
		
		
		
		
		// Logic step.
		if(!is_paused)
		{
			wsCameraUpdateFPSCamera(camera_main, time_delta_adj);
		}
		
		
		
		
		// Post-logic step.
		wsCameraUpdateProjection(camera_main);
		wsVulkanDrawFrame(time_delta_adj, camera_main);
		
		if(DEBUG && time_delta < 0)
		{
			// TODO: Set window title to FPS instead of printing it.
			printf("INFO: Time is %lld // %i fps\n", time_info.tv_sec, num_frames);
			num_frames = 0;
		}
		timespec_get(&time_info, TIME_UTC);
		time_end = time_info.tv_nsec;
		time_delta = (time_end - time_start);
		num_frames++;
		
		wsInputPostUpdate();
	}
	printf("===STOP%s RUN===\n\n", DEBUG ? " DEBUG" : "");
	
	
	// Program exit procedure.
	wsCameraStop();
	wsVulkanStop(&vk);
	wsWindowExit(windowID);
	
	
	printf("===END%s===\n", DEBUG ? " DEBUG" : "");
	return 0;
}