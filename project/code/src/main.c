#include<stdio.h>
#include"h/window.h"
#include"h/input.h"

#include <vulkan/vulkan.h>

#define CGLM_FORCE_RADIANS
#define CGLM_FORCE_DEPTH_ZERO_TO_ONE
#include<CGLM/vec4.h>
#include<CGLM/mat4.h>

#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>

void wsVulkanInit();
void wsRender();
void wsVulkanStop();

int main(int argc, char* argv[]) {
	printf("---Begin---\n");
	
	unsigned int windowID = wsWindowInit(640, 480);
	wsInputInit(windowID, 0.3f);
	wsVulkanInit();
	
	while(!glfwWindowShouldClose(wsWindowGetPtr(windowID))) {
		// Pre-logic-step.
		// ws
		
		// Logic step.
		// wsRun();
		
		// Post-logic-step.
		wsRender();
		wsInputUpdate();
	}
	
	wsVulkanStop();
	wsWindowExit(windowID);
	
	printf("---End---\n");
	// getchar();
	return 0;
}

// Call after wsWindowInit().
void wsVulkanInit() {
	uint32_t extension_count = 0;
	vkEnumerateInstanceExtensionProperties(NULL, &extension_count, NULL);
	printf("%i Vulkan extensions supported\n", extension_count);
	
	// Create Vulkan instance.
	// VkInstance instance;
	
}

void wsRender() {
	
}

void wsVulkanStop() {
	
}