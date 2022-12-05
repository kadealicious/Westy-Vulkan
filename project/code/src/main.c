#include<stdio.h>
#include"h/window.h"

#define CGLM_FORCE_RADIANS
#define CGLM_FORCE_DEPTH_ZERO_TO_ONE
#include<CGLM/vec4.h>
#include<CGLM/mat4.h>

#define NUM_MAX_WINDOWS 1

int wsVulkanInit();
int wsRender();
int wsVulkanStop();

int main(int argc, char* argv[]) {
	printf("---Begin---\n");
	
	unsigned int windowID = wsWindowInit(640, 480);
	wsVulkanInit();
	
	while(!glfwWindowShouldClose(wsWindowGetPtr(windowID))) {
		glfwPollEvents();
		
		wsRender();
	}
	
	wsVulkanStop();
	wsWindowExit(windowID);
	
	printf("---End---\n");
	// getchar();
	return 0;
}

// Call after wsWindowInit().
int wsVulkanInit() {
	uint32_t extension_count = 0;
	vkEnumerateInstanceExtensionProperties(NULL, &extension_count, NULL);
	printf("%i Vulkan extensions supported\n", extension_count);
	
	// Create Vulkan instance.
	// VkInstance instance;
	
	return 0;
}

int wsRender() {
	
	return 0;
}

int wsVulkanStop() {
	
	return 0;
}