#include<stdio.h>
#include<stdbool.h>
#include<time.h>
#include<stdlib.h>

#define CGLM_FORCE_RADIANS
#define CGLM_FORCE_DEPTH_ZERO_TO_ONE
#include<CGLM/vec4.h>
#include<CGLM/mat4.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include"h/window.h"
#include"h/input.h"
#include"h/vulkan_interface.h"
#include"h/mesh.h"

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
	wsMesh md;	// Handled in vulkan_interface.c.
	
	
	// Initialize GLFW.
	uint8_t windowID = wsWindowInit(640, 480, &wnd);
	GLFWwindow* window = wsWindowGetPtr(windowID);
	wsInputInit(windowID, 0.3f);	// Bind keyboard input to our GLFW window.
	
	// Initialize Vulkan.
	wsVulkanSetDebug(DEBUG);
	wsVulkanInit(&vk, &md, windowID);
	
	
	// Main loop.
	printf("\n===START%s RUN===\n", DEBUG ? " DEGUB" : "");
	
	struct timespec time_info = {};
	time_t time_start = 0;
	time_t time_end = 0;
	time_t time_delta = 0;
	double time_delta_adj = 0;
	
	while(!glfwWindowShouldClose(window))
	{
		// Pre-logic-step.
		time_delta_adj = max(time_delta * 0.000000001, 0.0);
		timespec_get(&time_info, TIME_UTC);
		time_start = time_info.tv_nsec;
		
		wsInputUpdate();
		if(wsInputGetKeyReleaseOnce(GLFW_KEY_ESCAPE)) {
			printf("INFO: User has requested window should close!\n");
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}
		
		
		// Logic step.
		// Not workie; make crashy :(
		/*md.vertices[0][0].color[0] = 1;
		md.vertices[0][0].color[1] = 1;
		md.vertices[0][0].color[2] = 1;*/
		
		
		// Post-logic step.
		wsVulkanDrawFrame(time_delta_adj);
		
		if(DEBUG && time_delta < 0)
		{
			printf("INFO: Time is %lld\n", time_info.tv_sec);
		}
		timespec_get(&time_info, TIME_UTC);
		time_end = time_info.tv_nsec;
		time_delta = (time_end - time_start);
	}
	printf("===STOP%s RUN===\n\n", DEBUG ? " DEBUG" : "");
	
	
	// Program exit procedure.
	wsVulkanStop(&vk);
	wsWindowExit(windowID);
	
	
	printf("===END%s===\n", DEBUG ? " DEBUG" : "");
	return 0;
}