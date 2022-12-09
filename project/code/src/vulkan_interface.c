#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>

#include <vulkan/vulkan.h>
#include<GLFW/glfw3.h>

#include"h/vulkan_interface.h"

// Main Vulkan instance.
VkInstance instanceVK = NULL;

bool wsVulkanEnableValidationLayers(VkInstanceCreateInfo* create_info);

// Call after wsWindowInit().
void wsVulkanInit(bool debug) {
	// Set application info.
	VkApplicationInfo app_info;
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName = "Westy";
	app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.pEngineName = "Westy Vulkan";
	app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.apiVersion = VK_API_VERSION_1_0;
	
	// Set instance creation info.
	VkInstanceCreateInfo create_info;
	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_info.pApplicationInfo = &app_info;
	
	// Get list of required Vulkan extensions from GLFW.
	uint32_t num_required_extensions = 0;
	const char** required_extensions = glfwGetRequiredInstanceExtensions(&num_required_extensions);
	
	create_info.enabledExtensionCount = num_required_extensions;
	create_info.ppEnabledExtensionNames = required_extensions;
	
	// Check GLFW-required Vulkan extensions.
	printf("%i Vulkan extension(s) required by GLFW: ", num_required_extensions);
	for(int i = 0; i < num_required_extensions; i++) {
		printf("%s\t", required_extensions[i]);
	}
	printf("\n");
	
	// Enable debug validation layers if in debug mode.
	create_info.enabledLayerCount = 0;
	// Causes vkCreateInstance() to crash when validation layers are present.  Cool!
	if(debug) {
		if(wsVulkanEnableValidationLayers(&create_info)) {
			printf("Required Vulkan validation layers are supported!\n");
		} else printf("ERROR: required Vulkan validation layers NOT supported!\n");
	}
	
	// Check that we have all the required extensions for Vulkan.
	uint32_t num_available_extensions = 0;
	vkEnumerateInstanceExtensionProperties(NULL, &num_available_extensions, NULL);
	VkExtensionProperties* available_extensions = malloc(num_available_extensions * sizeof(VkExtensionProperties));
	vkEnumerateInstanceExtensionProperties(NULL, &num_available_extensions, available_extensions);
	
	// WHY DOES THIS SECTION OF CODE CRASH THE PROGRAM ON VKCREATEINSTANCE()
	bool has_all_extensions = true;
	for(int i = 0; i < num_required_extensions; i++) {
		bool extension_found = false;
		
		for(int j = 0; j < num_available_extensions; j++) {
			if(strcmp(required_extensions[i], available_extensions[j].extensionName) == 0) {
				extension_found = true;
				break;
			}
		}
		
		// Why does this crash?? No idea!!
		/*if(!extension_found) {
			printf("ERROR: GLFW-required Vulkan extension \"%s\" is NOT supported!\n", required_extensions[i]);
			has_all_extensions = false;
		}*/
	}
	if(has_all_extensions)
		printf("All GLFW-required Vulkan extensions are supported!\n");
	
	/*printf("%i Vulkan extension(s) supported: ", num_available_extensions);
	for(int i = 0; i < num_available_extensions; i++) {
		printf("%s\t", available_extensions[i].extensionName);
	}
	printf("\n");*/
	
	// Create Vulkan instance!
	VkResult result = vkCreateInstance(&create_info, NULL, &instanceVK);
	if(result != VK_SUCCESS)
		printf("ERROR: Vulkan instance creation failed!\n");
	else printf("Vulkan instance created!\n");
	
	free(available_extensions);
}

void wsVulkanStop() {
	vkDestroyInstance(instanceVK, NULL);
	printf("Vulkan instance destroyed!\n");
}

// For internal use only.
bool wsVulkanEnableValidationLayers(VkInstanceCreateInfo* create_info) {
	// Who knows if I need to free all this memory!?!?  Not me!
	size_t num_required_layers = 1;
	char** required_layers = malloc(num_required_layers * sizeof(char*));
	required_layers[0] = malloc(sizeof("VK_LAYER_KHRONOS_validation") * sizeof(char) + 1);
	required_layers[0] = "VK_LAYER_KHRONOS_validation\0";
	
	// Get available validation layers.
	uint32_t num_available_layers;
	vkEnumerateInstanceLayerProperties(&num_available_layers, NULL);
	VkLayerProperties* available_layers = malloc(num_available_layers * sizeof(VkLayerProperties));
	vkEnumerateInstanceLayerProperties(&num_available_layers, available_layers);
	
	for(int i = 0; i < num_required_layers; i++) {
		bool layer_found = false;
		
		for(int j = 0; j < num_available_layers; j++) {
			if(strcmp(required_layers[i], available_layers[j].layerName) == 0) {
				layer_found = true;
				break;
			}
		}
		
		if(!layer_found)
			return false;
	}
	free(available_layers);
	
	// If we have all required layers available for use.
	create_info->enabledLayerCount = num_required_layers;
	create_info->ppEnabledLayerNames = (const char**)required_layers;
	printf("%i Vulkan validation layer(s) required: ", num_required_layers);
	for(int i = 0; i < num_required_layers; i++) {
		printf("%s\t", required_layers[i]);
	}
	printf("\n");
	
	return true;
}