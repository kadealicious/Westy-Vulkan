#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include <vulkan/vulkan.h>
#include<GLFW/glfw3.h>

#include"h/vulkan_interface.h"

#define MAX_QUEUE_FAMILIES 10

// General Vulkan interfacing functions.
void wsVulkanInit(const bool debug_mode);
void wsVulkanStop();

// Vulkan initialization functions.
bool wsVulkanEnableValidationLayers(VkInstanceCreateInfo* create_info);
bool wsVulkanEnableRequiredExtensions(VkInstanceCreateInfo* create_info);
void wsAddDebugExtensions(const char*** extensions, uint32_t* num_extensions);
bool wsVulkanPickPhysicalDevice();
int wsVulkanRatePhysicalDevice(VkPhysicalDevice* GPU);

// TODO: Make this not invalidate graphics card verification.
typedef struct wsVulkanQueueFamilyIndices {
	uint32_t graphics_family;
	bool has_graphics_family;
} wsVulkanQueueFamilyIndices;
void wsVulkanFindQueueFamilies(wsVulkanQueueFamilyIndices *indices, VkPhysicalDevice* GPU);

// Debug-messenger-related functions.
void wsVulkanInitDebugMessenger();
void wsVulkanStopDebugMessenger();
void wsVulkanPopulateDebugMessengerCreationInfo(VkDebugUtilsMessengerCreateInfoEXT* create_info);

// Vulkan proxy functions.
VkResult wsVulkanCreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* create_info, 
	const VkAllocationCallbacks* allocator, 
	VkDebugUtilsMessengerEXT* debug_messenger);
void wsVulkanDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debug_messenger, const VkAllocationCallbacks* allocator);
static VKAPI_ATTR VkBool32 VKAPI_CALL wsVulkanDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT msg_severity, 
	VkDebugUtilsMessageTypeFlagsEXT msg_type, 
	const VkDebugUtilsMessengerCallbackDataEXT* callback_data, 
	void* user_data);

// Debug mode.
bool debug;
// Main Vulkan instance.
VkInstance instanceVK;
// Primary physical device.  Implicitly destroyed when Vulkan instance is destroyed.
VkPhysicalDevice gpuVK = NULL;
// Main debug messenger.
VkDebugUtilsMessengerEXT debug_msgrVK;


// Call after wsWindowInit().
void wsVulkanInit(const bool debug_mode) {
	debug = debug_mode;
	
	// Set application info.
	VkApplicationInfo app_info;
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName = "Westy";
	app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.pEngineName = "Westy Vulkan";
	app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.apiVersion = VK_API_VERSION_1_0;
	app_info.pNext = NULL;
	
	// Set instance creation info.
	VkInstanceCreateInfo create_info;
	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_info.pApplicationInfo = &app_info;
	
	// Check that we have all the required extensions for Vulkan.
	if(wsVulkanEnableRequiredExtensions(&create_info)) {
		printf("\tAll GLFW-required Vulkan extensions are supported!\n");
	} else printf("\tERROR: Not all GLFW-required Vulkan extensions are supported!\n");
	
	// Enable Vulkan validation layers if in debug mode.
	create_info.enabledLayerCount = 0;
	create_info.pNext = NULL;
	// If debug, do debug things.  If debug, we will need this struct in a moment for scope-related reasons in vkCreateInstance().
	VkDebugUtilsMessengerCreateInfoEXT debug_create_info;
	if(debug) {
		if(wsVulkanEnableValidationLayers(&create_info)) {
			printf("\tRequired Vulkan validation layers are supported!\n");
		} else printf("\tERROR: required Vulkan validation layers NOT supported!\n");
		
		// This allows the debug messenger to debug vkCreateInstance().
		wsVulkanPopulateDebugMessengerCreationInfo(&debug_create_info);
		create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debug_create_info;
	}
	
	// Create Vulkan instance!
	VkResult result = vkCreateInstance(&create_info, NULL, &instanceVK);
	if(result != VK_SUCCESS)
		printf("ERROR: Vulkan instance creation failed with result code %i!\n", result);
	else printf("Vulkan instance created!\n");
	
	// Self-explanatory.  Used for receiving and sending debug messages to stdout.
	if(debug) {
		wsVulkanInitDebugMessenger();
	}
	
	if(wsVulkanPickPhysicalDevice()) {
		printf("Found GPU with proper Vulkan support!\n");
	} else printf("ERROR: Failed to find GPUs with proper Vulkan support!\n");
}

void wsVulkanStop() {
	if(debug) {
		wsVulkanStopDebugMessenger();
		printf("Vulkan debug messenger destroyed!\n");
	}
	
	vkDestroyInstance(instanceVK, NULL);
	printf("Vulkan instance destroyed!\n");
}

// Finds queue families and stores their indices in *indices.
void wsVulkanFindQueueFamilies(wsVulkanQueueFamilyIndices *indices, VkPhysicalDevice* GPU) {
	uint32_t num_queue_families = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(*GPU, &num_queue_families, NULL);
	VkQueueFamilyProperties* queue_families = malloc(num_queue_families * sizeof(VkQueueFamilyProperties));
	vkGetPhysicalDeviceQueueFamilyProperties(*GPU, &num_queue_families, queue_families);
	
	indices->has_graphics_family = false;
	for(int i = 0; i < num_queue_families; i++) {
		VkQueueFamilyProperties queue_family = queue_families[i];
		// Check for graphics queue family with proper support.
		if(queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices->graphics_family = i;
			indices->has_graphics_family = true;
			return;
		}
	}
}

// Pick an appropriate physical device for Vulkan to utilize.  Returns whether or not one was found.
bool wsVulkanPickPhysicalDevice() {
	// Get list of physical devices and store in GPUs[].
	uint32_t num_GPUs = 0;
	vkEnumeratePhysicalDevices(instanceVK, &num_GPUs, NULL);
	if(num_GPUs <= 0) {
		return false;
	}
	VkPhysicalDevice* GPUs = malloc(num_GPUs * sizeof(VkPhysicalDevice));
	vkEnumeratePhysicalDevices(instanceVK, &num_GPUs, GPUs);
	
	// Pick most suitable GPU.
	int max_score = -1;
	int ndx_GPU = -1;
	for(int i = 0; i < num_GPUs; i++) {
		int current_score = wsVulkanRatePhysicalDevice(&(GPUs[i]));
		if(current_score > max_score) {
			max_score = current_score;
			ndx_GPU = i;
		}
	}
	
	// If the max GPU score is not able to function with the program, then we've got a problemo
	if(max_score != -1) {
		gpuVK = GPUs[ndx_GPU];
		return true;
	} else return false;
	
	// Don't free GPU list so that we can allow user to swap GPUs in settings later.
}

// Verify if physical device is suitable for our use-cases and score it based on functionality.
int wsVulkanRatePhysicalDevice(VkPhysicalDevice* GPU) {
	int score = 0;
	
	VkPhysicalDeviceProperties device_properties;
	VkPhysicalDeviceFeatures device_features;
	vkGetPhysicalDeviceProperties(*GPU, &device_properties);
	vkGetPhysicalDeviceFeatures(*GPU, &device_features);
	
	// Program cannot function without geometry shaders!
	if(!device_features.geometryShader)
		return -1;
	
	// Program cannot function without certain queue families.
	wsVulkanQueueFamilyIndices indices;
	wsVulkanFindQueueFamilies(&indices, GPU);
	if(!indices.has_graphics_family)
		return -1;
	
	score += device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? 1000 : 0;
	score += device_properties.limits.maxImageDimension2D;
	
	return score;
}

void wsVulkanInitDebugMessenger() {
	// Populate creation info for debug messenger.
	VkDebugUtilsMessengerCreateInfoEXT create_info;
	wsVulkanPopulateDebugMessengerCreationInfo(&create_info);
	
	// Create debug messenger!
	if(wsVulkanCreateDebugUtilsMessengerEXT(instanceVK, &create_info, NULL, &debug_msgrVK) == VK_SUCCESS) {
		printf("Vulkan debug messenger created!\n");
	} else printf("ERROR: Vulkan debug messenger creation failed!\n");
}

void wsVulkanStopDebugMessenger() {
	wsVulkanDestroyDebugUtilsMessengerEXT(instanceVK, debug_msgrVK, NULL);
}

// Create debug messenger.
VkResult wsVulkanCreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* create_info, 
	const VkAllocationCallbacks* allocator, 
	VkDebugUtilsMessengerEXT* debug_messenger) {
    
	PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != NULL) {
        return func(instance, create_info, allocator, debug_messenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

// Destroy debug messenger.
void wsVulkanDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debug_messenger, const VkAllocationCallbacks* allocator) {
    PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != NULL) {
        func(instance, debug_messenger, allocator);
    }
}

// Callback for Vulkan debug messages.
static VKAPI_ATTR VkBool32 VKAPI_CALL wsVulkanDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT msg_severity, 
	VkDebugUtilsMessageTypeFlagsEXT msg_type, 
	const VkDebugUtilsMessengerCallbackDataEXT* callback_data, 
	void* user_data) {
	
	if(msg_severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
		printf("ERROR: ");
	else if(msg_severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		printf("WARNING: ");
	else if(msg_severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
		printf("INFO: ");
	
	printf("Validation layer says: %s\n", callback_data->pMessage);
	
	return VK_FALSE;
}

// Populate debug messenger creation info; used for debugging vkCreateInstance().
void wsVulkanPopulateDebugMessengerCreationInfo(VkDebugUtilsMessengerCreateInfoEXT* create_info) {
	// Specify creation info for debug messenger.
	create_info->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	// Specify which messages the debug callback function should be called for.
	create_info->messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | 
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	// Specify which messages the debug callback function should be notified about.
	create_info->messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | 
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | 
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	// Debug callback function pointer.
	create_info->pfnUserCallback = &wsVulkanDebugCallback;
	// Unnecessary, but will get warning without these two assignments.
	create_info->pUserData = NULL;
	create_info->flags = 0;
}

// For internal use only.
void wsAddDebugExtensions(const char*** extensions, uint32_t* num_extensions) {
	// Copy given extension list.
	char** debug_extensions = malloc((*num_extensions + 1) * sizeof(char*));
	for(int i = 0; i < *num_extensions; i++) {
		debug_extensions[i] = (char*)(*extensions)[i];
	}
	
	// Append Debug utility extension to list.
	debug_extensions[*num_extensions] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
	
	// Increment original number of extensions and return new extension list.
	(*num_extensions)++;
	*extensions = (const char**)debug_extensions;
}

// For internal use only.
bool wsVulkanEnableRequiredExtensions(VkInstanceCreateInfo* create_info) {
	// Get list of required Vulkan extensions from GLFW.
	uint32_t num_required_extensions = 0;
	const char** required_extensions = glfwGetRequiredInstanceExtensions(&num_required_extensions);
	if(debug) {
		wsAddDebugExtensions(&required_extensions, &num_required_extensions);
	}
	
	// Set correct fields in create_info.  Cannot create instance without this.
	create_info->flags = 0;
	create_info->enabledExtensionCount = num_required_extensions;
	create_info->ppEnabledExtensionNames = required_extensions;
	
	// List all required extensions.
	printf("%i Vulkan extension(s) required by GLFW: ", num_required_extensions);
	for(int i = 0; i < num_required_extensions; i++) {
		printf("%s\t", required_extensions[i]);
	}
	printf("\n");
	
	// Fetch required extensions & required extension count.
	uint32_t num_available_extensions = 0;
	vkEnumerateInstanceExtensionProperties(NULL, &num_available_extensions, NULL);
	VkExtensionProperties* available_extensions = malloc(num_available_extensions * sizeof(VkExtensionProperties));
	vkEnumerateInstanceExtensionProperties(NULL, &num_available_extensions, available_extensions);
	
	// Check that required extensions are supported.
	bool has_all_extensions = true;
	for(int i = 0; i < num_required_extensions; i++) {
		bool extension_found = false;
		
		for(int j = 0; j < num_available_extensions; j++) {
			if(strcmp(required_extensions[i], available_extensions[j].extensionName) == 0) {
				extension_found = true;
				break;
			}
		}
		
		if(!extension_found) {
			printf("\tERROR: GLFW-required Vulkan extension \"%s\" is NOT supported!\n", required_extensions[i]);
			has_all_extensions = false;
		}
	}
	
	// List all supported extensions.
	/*printf("\t%i Vulkan extension(s) supported: ", num_available_extensions);
	for(int i = 0; i < num_available_extensions; i++) {
		printf("%s\t", available_extensions[i].extensionName);
	}
	printf("\n");*/
	
	free(available_extensions);
	// NEVER FREE REQUIRED_EXTENSIONS!  GLFW guarantees pointer validity for lifetime of program.  Breaks non-debug mode.  Oopsie daisies!
	// free(required_extensions);
	
	return has_all_extensions;
}

// For internal use only.
bool wsVulkanEnableValidationLayers(VkInstanceCreateInfo* create_info) {
	// Who knows if I need to free all this memory!?!?  Not me!
	size_t num_required_layers = 1;
	char** required_layers = malloc(num_required_layers * sizeof(char*));
	required_layers[0] = malloc(sizeof("VK_LAYER_KHRONOS_validation") * sizeof(char));
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