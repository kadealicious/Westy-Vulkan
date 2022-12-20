#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>

#include <vulkan/vulkan.h>
#include<GLFW/glfw3.h>

#include"h/vulkan_interface.h"
#include"h/window.h"
#include"h/shader.h"


// General Vulkan interfacing functions.
void wsVulkanInit(wsVulkan* vk, uint8_t windowID);
void wsVulkanStop(wsVulkan* vk);

// Vulkan initialization functions.
bool wsVulkanEnableValidationLayers(VkInstanceCreateInfo* create_info);					// Enabled validation layers for detailed debugging.		
bool wsVulkanEnableRequiredExtensions(VkInstanceCreateInfo* create_info);				// Enabled extensions required by GLFW, debug mode, etc.
void wsVulkanAddDebugExtensions(const char*** extensions, uint32_t* num_extensions);	// Adds debug extension name to extensions**.

// Device-choosing and setup.
bool wsVulkanPickPhysicalDevice(wsVulkan* vk);	// Picks the best-suited GPU for our program.
int32_t wsVulkanRatePhysicalDevice(wsVulkan* vk, VkPhysicalDevice* physical_device);	// Rates GPU based on device features and properties.
bool wsVulkanCheckDeviceExtensionSupport(VkPhysicalDevice* physical_device);			// Queries whethor or not physical_device supports required extensions.
void wsVulkanQuerySwapChainSupport(wsVulkan* vk);										// Queries whether or not physical_device can support features required by vk.
void wsVulkanChooseSwapSurfaceFormat(wsVulkanSwapChainInfo* swapchain_info);	// Choose swap chain surface format.
void wsVulkanChooseSwapExtent(wsVulkan* vk);									// Choose swap chain image resolution.
void wsVulkanChooseSwapPresentMode(wsVulkanSwapChainInfo* swapchain_info);		// Choose swap chain presentation mode.
VkResult wsVulkanCreateSwapChain(wsVulkan* vk);									// Creates a swap chain for image buffering.
uint32_t wsVulkanCreateImageViews(wsVulkan* vk);								// Creates image views viewing swap chain images; returns number of image views created successfully.
VkResult wsVulkanCreateLogicalDevice(wsVulkan* vk, uint32_t num_validation_layers, const char* const* validation_layers);	// Creates a logical device for interfacing with the GPU.
VkResult wsVulkanCreateSurface(wsVulkan* vk);			// Creates a surface for drawing to the screen.
VkResult wsVulkanCreateGraphicsPipeline(wsVulkan* vk);	// Creates a graphics pipeline and stores its ID inside of struct vk.

// Queue family management.
void wsVulkanFindQueueFamilies(wsVulkanQueueFamilyIndices* indices, VkPhysicalDevice* physical_device, VkSurfaceKHR* surface);	// Finds required queue families and stores them in indices.
bool wsVulkanHasFoundAllQueueFamilies(wsVulkanQueueFamilyIndices* indices);	// Checks if all required queue families have been found.

// Debug-messenger-related functions.
VkResult wsVulkanInitDebugMessenger(wsVulkan* vk);
void wsVulkanStopDebugMessenger(wsVulkan* vk);
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
uint8_t debug;
void wsVulkanSetDebug(uint8_t debug_mode) { debug = debug_mode; }


// Call after wsWindowInit().
void wsVulkanInit(wsVulkan* vk, uint8_t windowID) {
	printf("---Begin Vulkan Initialization!---\n");

	vk->windowID = windowID;

	// Set application info.
	VkApplicationInfo app_info;
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName = "Westy";
	app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.pEngineName = "Westy Vulkan";
	app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.apiVersion = VK_API_VERSION_1_0;
	app_info.pNext = NULL;
	
	// Set application info within create_info struct.
	VkInstanceCreateInfo create_info;
	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_info.pApplicationInfo = &app_info;
	
	// Check that we have all the required extensions for Vulkan.
	if(!wsVulkanEnableRequiredExtensions(&create_info)) {
		printf("\tERROR: Not all GLFW-required Vulkan extensions are supported!\n");
	} else printf("\tAll GLFW-required Vulkan extensions are supported!\n");
	
	// Enable Vulkan validation layers if in debug mode.
	create_info.enabledLayerCount = 0;
	create_info.pNext = NULL;
	// If debug, do debug things.  If debug, we will need this struct in a moment for scope-related reasons in vkCreateInstance().
	VkDebugUtilsMessengerCreateInfoEXT debug_create_info;
	if(debug) {
		if(!wsVulkanEnableValidationLayers(&create_info)) {
			printf("\tERROR: required Vulkan validation layers NOT supported!\n");
		} else printf("\tRequired Vulkan validation layers are supported!\n");
		
		// This allows the debug messenger to debug vkCreateInstance().  Pretty important stuff!
		wsVulkanPopulateDebugMessengerCreationInfo(&debug_create_info);
		create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debug_create_info;
	}
	
	// Create Vulkan instance!
	VkResult result = vkCreateInstance(&create_info, NULL, &vk->instance);
	if(result != VK_SUCCESS)
		printf("ERROR: Vulkan instance creation failed with result code %i!\n", result);
	else printf("Vulkan instance created!\n");
	
	// Self-explanatory.  Used for receiving and sending debug messages to stdout.
	if(debug) {
		result = wsVulkanInitDebugMessenger(vk);
		if(result != VK_SUCCESS) {
			printf("ERROR: Vulkan debug messenger creation failed with result code %i!\n", result);
		} else printf("Vulkan debug messenger created!\n");
	}
	
	// Window surface creation procedure.
	result = wsVulkanCreateSurface(vk);
	if(result != VK_SUCCESS) {
		printf("ERROR: Vulkan surface creation for window %i failed with result code %i!\n", vk->windowID, result);
	} else printf("Vulkan surface created for window %i!\n", vk->windowID);
	
	// Find best-suited physical device.
	if(!wsVulkanPickPhysicalDevice(vk)) {
		printf("ERROR: Failed to find GPUs with proper Vulkan support!\n");
	} else printf("Found GPU with proper Vulkan support!\n");
	
	// Create logical device for interfacing with physical device.
	result = wsVulkanCreateLogicalDevice(vk, create_info.enabledLayerCount, create_info.ppEnabledLayerNames);
	if(result != VK_SUCCESS) {
		printf("ERROR: Vulkan logical device creation failed with result code %i!\n", result);
	} else printf("Vulkan logical device created!\n");
	
	result = wsVulkanCreateSwapChain(vk);
	if(result != VK_SUCCESS) {
		printf("ERROR: Vulkan swap chain creation failed with result code %i!\n", result);
	} else printf("Vulkan swap chain created!\n");

	uint32_t num_created = wsVulkanCreateImageViews(vk);
	if(num_created != vk->num_swapchain_images) {
		printf("ERROR: Only %i/%i image views created!\n", num_created, vk->num_swapchain_images);
	} else printf("%i/%i Vulkan image views created!\n", num_created, vk->num_swapchain_images);

	result = wsVulkanCreateGraphicsPipeline(vk);
	if(result != VK_SUCCESS) {
		printf("ERROR: Vulkan graphics pipeline creation failed!\n");
	} else printf("Vulkan graphics pipeline created!\n");

	printf("---End Vulkan Initialization!---\n");
}

void wsVulkanStop(wsVulkan* vk) {
	if(debug) {
		wsVulkanStopDebugMessenger(vk);
		printf("Vulkan debug messenger destroyed!\n");
	}
	
	for(uint32_t i = 0; i < vk->num_swapchain_images; i++) {
		vkDestroyImageView(vk->logical_device, vk->swapchain_imageviews[i], NULL);
	}
	free(vk->swapchain_imageviews);
	printf("Vulkan image views destroyed!\n");

	vkDestroySwapchainKHR(vk->logical_device, vk->swapchain, NULL);
	free(vk->swapchain_info.formats);
	free(vk->swapchain_info.present_modes);
	free(vk->swapchain_images);
	printf("Vulkan swap chain destroyed!\n");
	
	vkDestroyDevice(vk->logical_device, NULL);
	printf("Vulkan logical device destroyed!\n");
	vkDestroySurfaceKHR(vk->instance, vk->surface, NULL);
	printf("Vulkan surface destroyed!\n");
	vkDestroyInstance(vk->instance, NULL);
	printf("Vulkan instance destroyed!\n");
}

VkResult wsVulkanCreateGraphicsPipeline(wsVulkan* vk) {

}

// Returns number of image views created successfully.
uint32_t wsVulkanCreateImageViews(wsVulkan* vk) {
	uint32_t num_created = 0;

	// Allocate space for our image views within struct vk.
	vk->swapchain_imageviews = malloc(vk->num_swapchain_images * sizeof(VkImageView));

	// Create image view for each swap chain image.
	for(uint32_t i = 0; i < vk->num_swapchain_images; i++) {
		VkImageViewCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		create_info.image = vk->swapchain_images[i];
		create_info.pNext = NULL;
		create_info.flags = 0;
		create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		create_info.format = vk->swapchain_imageformat;

		// Stick to default color mapping.
		create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		// subresourceRange describes what the images purpose is, as well as which part of the image we will be accessing.
		create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		create_info.subresourceRange.baseMipLevel = 0;	// Mipping &...
		create_info.subresourceRange.levelCount = 1;	// Mapping!
		create_info.subresourceRange.baseArrayLayer = 0;
		create_info.subresourceRange.layerCount = 1;

		VkResult result = vkCreateImageView(vk->logical_device, &create_info, NULL, &vk->swapchain_imageviews[i]);
		if(result != VK_SUCCESS)
			printf("ERROR: Image view %i not created successfully; code %i!\n", i, result);
		else num_created++;
	}

	return num_created;
}

VkResult wsVulkanCreateSwapChain(wsVulkan* vk) {
	// Initialize swap chain within struct vk.
	// wsVulkanQuerySwapChainSupport(vk);
	wsVulkanChooseSwapSurfaceFormat(&vk->swapchain_info);
	wsVulkanChooseSwapPresentMode(&vk->swapchain_info);
	wsVulkanChooseSwapExtent(vk);

	// Recommended to add at least 1 extra image to swap chain buffer to reduce wait times during rendering.
	uint32_t num_images = vk->swapchain_info.capabilities.minImageCount + 2;
	vk->num_swapchain_images = num_images;

	// Specify swap chain creation info.
	VkSwapchainCreateInfoKHR create_info;
	create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	create_info.pNext = NULL;
	create_info.flags = 0;
	create_info.surface = vk->surface;
	create_info.minImageCount = num_images;
	create_info.imageFormat = vk->swapchain_info.surface_format.format;
	create_info.imageColorSpace = vk->swapchain_info.surface_format.colorSpace;
	create_info.imageExtent = vk->swapchain_info.extent;
	create_info.imageArrayLayers = 1;	// Always 1 unless this is a stereoscopic 3D application.
	create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	// Check if queue family indices are unique.
	uint32_t queue_family_indices[] = {vk->indices.graphics_family, vk->indices.present_family};
	if(vk->indices.graphics_family != vk->indices.present_family) {
		create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		create_info.queueFamilyIndexCount = 2;
		create_info.pQueueFamilyIndices = queue_family_indices;
	} else {
		create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		create_info.queueFamilyIndexCount = 0;	// Optional.
		create_info.pQueueFamilyIndices = NULL;	// Optional.
	}

	create_info.preTransform = vk->swapchain_info.capabilities.currentTransform;	// Do we want to apply any transformations to our swap chain content?  Nope!
	create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;	// Do we want to blend our program into other windows in the window system?  No siree!
	create_info.presentMode = vk->swapchain_info.present_mode;
	create_info.clipped = VK_TRUE;	// If another window obscures some pixels from our window, should we ignore drawing them?  Yeah, probably.
	create_info.oldSwapchain = VK_NULL_HANDLE;	// Reference to old swap chain in case we ever have to create a new one!  NULL for now.

	VkResult result = vkCreateSwapchainKHR(vk->logical_device, &create_info, NULL, &vk->swapchain);
	
	// Store swap chain images in struct vk.
	vkGetSwapchainImagesKHR(vk->logical_device, vk->swapchain, &num_images, NULL);
	vk->swapchain_images = malloc(num_images * sizeof(VkImage));
	vkGetSwapchainImagesKHR(vk->logical_device, vk->swapchain, &num_images, vk->swapchain_images);
	
	// Set current swap chain image format and extent within struct vk.
	vk->swapchain_imageformat = vk->swapchain_info.surface_format.format;
	vk->swapchain_presentmode = vk->swapchain_info.present_mode;
	vk->swapchain_extent = vk->swapchain_info.extent;

	printf("Creating swap chain with properties: \n\tExtent: %ix%i\n\tSurface format: %i\n\tPresentation mode: %i\n", 
		vk->swapchain_extent.width, vk->swapchain_extent.height, vk->swapchain_info.surface_format.format, vk->swapchain_presentmode);

	return result;
}

// Choose a nice resolution to draw swap chain images at.
uint32_t clamp(uint32_t num, uint32_t min, uint32_t max) {const uint32_t t = num < min ? min : num;return t > max ? max : t;}
void wsVulkanChooseSwapExtent(wsVulkan* vk) {
	wsVulkanSwapChainInfo* swapchain_info = &vk->swapchain_info;
	VkSurfaceCapabilitiesKHR* capabilities = &vk->swapchain_info.capabilities;
	
	if(capabilities->currentExtent.width != UINT32_MAX) {
		swapchain_info->extent = capabilities->currentExtent;
	} else {
		// Query GLFW Window framebuffer size.
		int width, height;
		glfwGetFramebufferSize(wsWindowGetPtr(vk->windowID), &width, &height);

		// Set extent width.
		swapchain_info->extent.width = clamp((uint32_t)width, capabilities->minImageExtent.width, capabilities->maxImageExtent.width);
		swapchain_info->extent.height = clamp((uint32_t)height, capabilities->minImageExtent.height, capabilities->maxImageExtent.height);
	}
}

// Choose a presentation mode for our swap chain drawing surface.
void wsVulkanChooseSwapPresentMode(wsVulkanSwapChainInfo* swapchain_info) {
	// If we find a presentation mode that works for us, select it and return.
	for(int32_t i = 0; i < swapchain_info->num_present_modes; i++) {
		VkPresentModeKHR* current_mode = &swapchain_info->present_modes[i];
		if(*current_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
			swapchain_info->present_mode = VK_PRESENT_MODE_MAILBOX_KHR;
			return;
		}
	}

	// If we don't find one that matches our request, pick the only guaranteed presentation mode.
	swapchain_info->present_mode = VK_PRESENT_MODE_FIFO_KHR;
}

// Choose a format and color space for our swap chain drawing surface.
void wsVulkanChooseSwapSurfaceFormat(wsVulkanSwapChainInfo* swapchain_info) {
	// If we find a format that works for us, select it and return.
	for(int32_t i = 0; i < swapchain_info->num_formats; i++) {
		VkSurfaceFormatKHR* current_format = &swapchain_info->formats[i];
		if(current_format->format == VK_FORMAT_B8G8R8A8_SRGB && current_format->colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			swapchain_info->surface_format = *current_format;
			return;
		}
	}

	// If we don't find one that matches our request, pick the first available format.
	swapchain_info->surface_format = swapchain_info->formats[0];
}

void wsVulkanQuerySwapChainSupport(wsVulkan* vk) {
	// Query surface details into vk->swapchain_deets.
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk->physical_device, vk->surface, &vk->swapchain_info.capabilities);

	// Query supported surface formats.
	uint32_t num_formats;
	
	vkGetPhysicalDeviceSurfaceFormatsKHR(vk->physical_device, vk->surface, &num_formats, NULL);
	vk->swapchain_info.num_formats = num_formats;

	if(num_formats != 0) {
		vk->swapchain_info.formats = malloc(num_formats * sizeof(VkSurfaceFormatKHR));
		vkGetPhysicalDeviceSurfaceFormatsKHR(vk->physical_device, vk->surface, &num_formats, vk->swapchain_info.formats);
	}

	// Query supported presentation modes.
	uint32_t num_present_modes;
	
	vkGetPhysicalDeviceSurfacePresentModesKHR(vk->physical_device, vk->surface, &num_present_modes, NULL);
	vk->swapchain_info.num_present_modes = num_present_modes;

	if(num_present_modes != 0) {
		vk->swapchain_info.present_modes = malloc(num_present_modes * sizeof(VkPresentModeKHR));
		vkGetPhysicalDeviceSurfacePresentModesKHR(vk->physical_device, vk->surface, &num_present_modes, vk->swapchain_info.present_modes);
	}
}

// Creates a surface bound to our GLFW window.
VkResult wsVulkanCreateSurface(wsVulkan* vk) {
	return glfwCreateWindowSurface(vk->instance, wsWindowGetPtr(vk->windowID), NULL, &vk->surface);
}

// Creates a logical device to interface with the physical one.
VkResult wsVulkanCreateLogicalDevice(wsVulkan* vk, uint32_t num_validation_layers, const char* const* validation_layers) {
	
	// Get required queue family indices and store them in vk.
	wsVulkanFindQueueFamilies(&vk->indices, &vk->physical_device, &vk->surface);
	
	// Figure out how many unique queue families we need to initialize.
	uint8_t num_unique_queue_families;
	if(vk->indices.graphics_family == vk->indices.present_family)
		num_unique_queue_families = 1;
	else num_unique_queue_families = 2;
	
	// Store queue family creation infos for later use in binding to logical device creation info.
	uint32_t unique_queue_family_indices[2] = {vk->indices.graphics_family, vk->indices.present_family};
	VkDeviceQueueCreateInfo* queue_create_infos = calloc(num_unique_queue_families, sizeof(VkDeviceQueueCreateInfo));
	
	// Specify creation info for queue families.
	for(uint8_t i = 0; i < num_unique_queue_families; i++) {
		queue_create_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_create_infos[i].queueFamilyIndex = unique_queue_family_indices[i];
		queue_create_infos[i].queueCount = 1;
		
		// Specify a queue priority from 0.0f-1.0f.  Required regardless of number of queues; influences scheduling of command buffer execution.
		float queue_priority = 1.0f;
		queue_create_infos[i].pQueuePriorities = &queue_priority;
	}
	
	// Used to specify device features that will be used.
	VkPhysicalDeviceFeatures device_features = {};
	
	// Specify creation info for logical_gpuVK.
	VkDeviceCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	
	// Specify queue creation info count & location.
	create_info.queueCreateInfoCount = num_unique_queue_families;
	create_info.pQueueCreateInfos = queue_create_infos;
	
	// Device features.
	create_info.pEnabledFeatures = &device_features;
	create_info.pNext = NULL;
	
	// Modern way of handling device-specific validation layers.
	const char* device_extensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
	create_info.enabledExtensionCount = 1;
	create_info.ppEnabledExtensionNames = device_extensions;
	// Device-specific validation layers are deprecated for modern API versions, but required for older versions.
	if(debug) {
		create_info.enabledLayerCount = (uint32_t)num_validation_layers;
		create_info.ppEnabledLayerNames = validation_layers;
	} else create_info.enabledLayerCount = 0;
	
	// Create logical_device and return result!
	VkResult result = vkCreateDevice(vk->physical_device, &create_info, NULL, &vk->logical_device);
	
	// Assign queue handles from logical_device.
	vkGetDeviceQueue(vk->logical_device, vk->indices.graphics_family, 0, &vk->graphics_queue);
	vkGetDeviceQueue(vk->logical_device, vk->indices.present_family, 0, &vk->present_queue);
	
	printf("%i unique Vulkan queue families exist; indices are as specified:\n", num_unique_queue_families);
	printf("\tGraphics: %i\n\tPresentation: %i\n", vk->indices.graphics_family, vk->indices.present_family);
	
	// FREE MEMORY!!!
	free(queue_create_infos);
	
	return result;
}

// Checks if all queue families have been found.
bool wsVulkanHasFoundAllQueueFamilies(wsVulkanQueueFamilyIndices* indices) {
	if(!indices->has_graphics_family)
		return false;
	if(!indices->has_present_family)
		return false;
	
	return true;
}

// Finds queue families and stores their indices in *indices.
void wsVulkanFindQueueFamilies(wsVulkanQueueFamilyIndices *indices, VkPhysicalDevice* physical_device, VkSurfaceKHR* surface) {
	// Get list of queue families available to us.
	uint32_t num_queue_families = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(*physical_device, &num_queue_families, NULL);
	VkQueueFamilyProperties* queue_families = malloc(num_queue_families * sizeof(VkQueueFamilyProperties));
	vkGetPhysicalDeviceQueueFamilyProperties(*physical_device, &num_queue_families, queue_families);
	
	// These will end up as true once a family is found.
	indices->has_graphics_family = false;
	indices->has_present_family = false;
	
	for(int32_t i = 0; i < num_queue_families; i++) {
		
		// Current queue family to analyze.
		VkQueueFamilyProperties queue_family = queue_families[i];
		
		// Check for graphics queue family support.
		if(queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices->graphics_family = i;
			indices->has_graphics_family = true;
		}
		// Check for presentation queue family support.
		VkBool32 has_present_support = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(*physical_device, i, *surface, &has_present_support);
		if(has_present_support) {
			indices->present_family = i;
			indices->has_present_family = true;
		}
		
		// If we have found all queue families, we are done searching!
		if(wsVulkanHasFoundAllQueueFamilies(indices))
			return;
	}
}

// Pick an appropriate physical device for Vulkan to utilize.  Returns whether or not one was found.
bool wsVulkanPickPhysicalDevice(wsVulkan* vk) {
	// Get list of physical devices and store in GPUs[].
	uint32_t num_GPUs = 0;
	vkEnumeratePhysicalDevices(vk->instance, &num_GPUs, NULL);
	if(num_GPUs <= 0) {
		return false;
	}
	VkPhysicalDevice* GPUs = malloc(num_GPUs * sizeof(VkPhysicalDevice));
	vkEnumeratePhysicalDevices(vk->instance, &num_GPUs, GPUs);
	
	// Pick GPU with highest score.
	int32_t max_score = -1;
	int32_t ndx_GPU = -1;

	printf("Rating %i GPU(s)...\n", num_GPUs);

	for(int32_t i = 0; i < num_GPUs; i++) {
		printf("GPU %i: \t", i);

		// Make sure we are checking current GPU's suitability;
		vk->physical_device = GPUs[i];
		int32_t current_score = wsVulkanRatePhysicalDevice(vk, &(GPUs[i]));

		if(current_score > max_score) {
			max_score = current_score;
			ndx_GPU = i;
		}

		printf("\tGPU %i scored %i!\n", i, current_score);
	}
	
	// If the max GPU score is not able to function with the program, then we've got a problemo
	if(max_score != -1) {
		vk->physical_device = GPUs[ndx_GPU];
		
		// TODO: Make this list the properties of chosen GPU.
		
		return true;
	} else return false;
	
	// Don't free GPU list so that we can allow user to swap GPUs in settings later.
}

// Verify if physical device is suitable for our use-cases and score it based on functionality.
int32_t wsVulkanRatePhysicalDevice(wsVulkan* vk, VkPhysicalDevice* physical_device) {
	enum GPU_INCOMPATIBILITy_CODES {NO_GEOMETRY_SHADER = INT_MIN, NO_GRAPHICS_FAMILY, NO_PRESENTATION_FAMILY, NO_DEVICE_EXTENSION_SUPPORT, NO_SWAPCHAIN_SUPPORT};
	int32_t score = 0;
	
	VkPhysicalDeviceProperties device_properties;
	VkPhysicalDeviceFeatures device_features;
	vkGetPhysicalDeviceProperties(*physical_device, &device_properties);
	vkGetPhysicalDeviceFeatures(*physical_device, &device_features);
	
	// Program cannot function without geometry shaders!
	if(!device_features.geometryShader)
		return NO_GEOMETRY_SHADER;
	
	// Program cannot function without certain queue families.
	wsVulkanQueueFamilyIndices indices;
	wsVulkanFindQueueFamilies(&indices, physical_device, &vk->surface);
	if(!indices.has_graphics_family)
		return NO_GRAPHICS_FAMILY;
	if(!indices.has_present_family)
		return NO_PRESENTATION_FAMILY;
	
	// Program cannot function without certain device-specific extensions.
	if(!wsVulkanCheckDeviceExtensionSupport(physical_device))
		return NO_DEVICE_EXTENSION_SUPPORT;
	
	// Program cannot function without proper swapchain support!
	wsVulkanQuerySwapChainSupport(vk);
	if(vk->swapchain_info.num_formats <= 0 || vk->swapchain_info.num_present_modes <= 0)
		return NO_SWAPCHAIN_SUPPORT;

	// If these are the same queue family, this could increase performance.
	score += indices.graphics_family == indices.present_family ? 500 : 0;
	
	// Prefer discrete GPU.  If the max texture size is much lower on the discrete card, it might be old and suck.
	score += device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? 1000 : 0;
	score += device_properties.limits.maxImageDimension2D;
	
	return score;
}

bool wsVulkanCheckDeviceExtensionSupport(VkPhysicalDevice* physical_device) {
	// Query number of extensions available and their names.
	uint32_t num_available_extensions;
	vkEnumerateDeviceExtensionProperties(*physical_device, NULL, &num_available_extensions, NULL);
	VkExtensionProperties* available_extensions = malloc(num_available_extensions * sizeof(VkExtensionProperties));
	vkEnumerateDeviceExtensionProperties(*physical_device, NULL, &num_available_extensions, available_extensions);

	// Required device-specific extensions to verify we have access to.
	uint32_t num_required_extensions = 1;
	const char* required_extensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

	printf("%i device-specific Vulkan extensions required: \n", num_required_extensions);
	for(int32_t i = 0; i < num_required_extensions; i++) {
		printf("\t%s\n", required_extensions[i]);
	}

	// Search for each required extension.
	for(uint32_t i = 0; i < num_required_extensions; i++) {
		bool has_found_extension = false;
		
		for(uint32_t j = 0; j < num_available_extensions; j++) {
			if(strcmp(required_extensions[i], available_extensions[j].extensionName) == 0) {
				has_found_extension = true;
				break;
			}
		}

		if(!has_found_extension) {
			printf("\tERROR: Not all required device-specific Vulkan extension(s) are supported by your GPU!\n");
			return false;
		}
	}

	printf("\tAll required device-specific Vulkan extensions are supported by your GPU!\n");

	return true;
}

VkResult wsVulkanInitDebugMessenger(wsVulkan* vk) {
	// Populate creation info for debug messenger.
	VkDebugUtilsMessengerCreateInfoEXT create_info;
	wsVulkanPopulateDebugMessengerCreationInfo(&create_info);
	
	// Create debug messenger!
	return wsVulkanCreateDebugUtilsMessengerEXT(vk->instance, &create_info, NULL, &vk->debug_messenger);
}

void wsVulkanStopDebugMessenger(wsVulkan* vk) {
	wsVulkanDestroyDebugUtilsMessengerEXT(vk->instance, vk->debug_messenger, NULL);
}

// Vulkan proxy function; Create debug messenger.
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

// Vulkan proxy function; Destroy debug messenger.
void wsVulkanDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debug_messenger, const VkAllocationCallbacks* allocator) {
    PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != NULL) {
        func(instance, debug_messenger, allocator);
    }
}

// Vulkan proxy function; Callback for Vulkan debug messages.
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
	create_info->messageSeverity = // VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | 
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

// Adds debug extensions to required extension list.
void wsVulkanAddDebugExtensions(const char*** extensions, uint32_t* num_extensions) {
	// Copy given extension list.
	char** debug_extensions = malloc((*num_extensions + 1) * sizeof(char*));
	for(int32_t i = 0; i < *num_extensions; i++) {
		debug_extensions[i] = (char*)(*extensions)[i];
	}
	
	// Append Debug utility extension to list.
	debug_extensions[*num_extensions] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
	
	// FREE MEMORY!!!
	free(extensions);
	
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
		wsVulkanAddDebugExtensions(&required_extensions, &num_required_extensions);
	}
	
	// Set correct fields in create_info.  Cannot create instance without this.
	create_info->flags = 0;
	create_info->enabledExtensionCount = num_required_extensions;
	create_info->ppEnabledExtensionNames = required_extensions;
	
	// List all required extensions.
	printf("%i Vulkan extension(s) required by GLFW: \n", num_required_extensions);
	for(int32_t i = 0; i < num_required_extensions; i++) {
		printf("\t%s\n", required_extensions[i]);
	}
	
	// Fetch required extensions & required extension count.
	uint32_t num_available_extensions = 0;
	vkEnumerateInstanceExtensionProperties(NULL, &num_available_extensions, NULL);
	VkExtensionProperties* available_extensions = malloc(num_available_extensions * sizeof(VkExtensionProperties));
	vkEnumerateInstanceExtensionProperties(NULL, &num_available_extensions, available_extensions);
	
	// Check that required extensions are supported.
	bool has_all_extensions = true;
	for(int32_t i = 0; i < num_required_extensions; i++) {
		bool extension_found = false;
		
		for(int32_t j = 0; j < num_available_extensions; j++) {
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
	for(int32_t i = 0; i < num_available_extensions; i++) {
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
	
	for(int32_t i = 0; i < num_required_layers; i++) {
		bool layer_found = false;
		
		for(int32_t j = 0; j < num_available_layers; j++) {
			if(strcmp(required_layers[i], available_layers[j].layerName) == 0) {
				layer_found = true;
				break;
			}
		}
		
		if(!layer_found)
			return false;
	}
	
	// If we have all required layers available for use.
	create_info->enabledLayerCount = num_required_layers;
	create_info->ppEnabledLayerNames = (const char**)required_layers;
	printf("%i Vulkan validation layer(s) required: \n", num_required_layers);
	for(int32_t i = 0; i < num_required_layers; i++) {
		printf("\t%s\n", required_layers[i]);
	}
	
	// FREE MEMORY!!!
	free(available_layers);
	// I believe this sometimes causes validation layers to cry.
	// free(required_layers);
	
	return true;
}