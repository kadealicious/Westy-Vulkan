#include<stdio.h>
#include<stdlib.h>
#include <vulkan/vulkan.h>
#include<GLFW/glfw3.h>
#include"h/vulkan_interface.h"

// Contains all vulkan data.
VkInstance instanceVK;

// Call after wsWindowInit().
void wsVulkanInit() {
	// Check that we have the required extensions for Vulkan.
	uint32_t vk_num_extensions = 0;
	vkEnumerateInstanceExtensionProperties(NULL, &vk_num_extensions, NULL);
	VkExtensionProperties* vk_extensions = malloc(vk_num_extensions * sizeof(VkExtensionProperties));
	vkEnumerateInstanceExtensionProperties(NULL, &vk_num_extensions, vk_extensions);
	
	printf("%i Vulkan extension(s) supported: ", vk_num_extensions);
	for(int i = 0; i < vk_num_extensions; i++) {
		printf("%s\t", vk_extensions[i].extensionName);
	}
	printf("\n");
	free(vk_extensions);
	
	// Create Vulkan instance along with app info.
	VkApplicationInfo app_info;
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName = "Westy Vulkan";
	app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.pEngineName = "No Engine";
	app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.apiVersion = VK_API_VERSION_1_0;
	
	VkInstanceCreateInfo create_info;
	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_info.pApplicationInfo = &app_info;
	
	// Get list of required Vulkan extensions from GLFW.
	uint32_t glfw_num_extensions = 0;
	const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_num_extensions);
	
	create_info.enabledExtensionCount = glfw_num_extensions;
	create_info.ppEnabledExtensionNames = glfw_extensions;
	create_info.enabledLayerCount = 0;
	
	printf("%i Vulkan extension(s) required by GLFW: ", glfw_num_extensions);
	for(int i = 0; i < glfw_num_extensions; i++) {
		printf("%s\t", glfw_extensions[i]);
	}
	printf("\n");
	
	// Create Vulkan instance!
	VkResult result = vkCreateInstance(&create_info, NULL, &instanceVK);
	if(result != VK_SUCCESS)
		printf("ERROR: Vulkan instance creation failed!\n");
	else printf("Vulkan instance created!\n");
}

void wsVulkanStop() {
	vkDestroyInstance(instanceVK, NULL);
	printf("Vulkan instance destroyed!\n");
}