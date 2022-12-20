#ifndef VULKAN_INTERFACE_H_
#define VULKAN_INTERFACE_H_

#include<stdbool.h>

#include <vulkan/vulkan.h>

#define NUM_MAX_PIPELINES 100

// Swap chain support details.
typedef struct wsVulkanSwapChainInfo {
	VkSurfaceCapabilitiesKHR capabilities;
	VkExtent2D extent;

	VkSurfaceFormatKHR surface_format;
	VkSurfaceFormatKHR* formats;
	uint32_t num_formats;

	VkPresentModeKHR present_mode;
	VkPresentModeKHR* present_modes;
	uint32_t num_present_modes;

} wsVulkanSwapChainInfo;

// Queue family storage.
typedef struct wsVulkanQueueFamilyIndices {
	// Used for drawing graphics!
	uint32_t graphics_family;
	bool has_graphics_family;
	
	// Used for presenting surfaces to the window.
	uint32_t present_family;
	bool has_present_family;
	
} wsVulkanQueueFamilyIndices;

// Vulkan object management.
typedef struct wsVulkan {
	VkInstance instance;	// Main Vulkan instance.
	VkSurfaceKHR surface;	// Window surface for drawing.
	VkPhysicalDevice	physical_device;	// Primary physical device.  Implicitly destroyed when Vulkan instance is destroyed.
	VkDevice			logical_device;		// Primary logical device used to interface with the physical device.
	VkDebugUtilsMessengerEXT debug_messenger;	// Main debug messenger.

	wsVulkanQueueFamilyIndices indices;	// Queue family indices.
	VkQueue graphics_queue;				// Queue for submitting graphical commands.
	VkQueue present_queue;				// Queue for submitting presentation commands.

	VkSwapchainKHR swapchain;			// Swap chain.
	uint32_t num_swapchain_images;
	VkImage* swapchain_images;			// Pointer to array of (4) swap chain images.
	VkImageView* swapchain_imageviews;	// How do we view/use each image in the swap chain?
	VkFormat swapchain_imageformat;		// What image format is our swap chain using?
	VkPresentModeKHR swapchain_presentmode;	// Stores presentation mode of swap chain.
	VkExtent2D swapchain_extent;		// What is the resolution of the swap chain images?
	wsVulkanSwapChainInfo swapchain_info;	// Struct containing swapchain capabilities, surface formats, and presentation modes.
	
	uint8_t pipelineIDs[NUM_MAX_PIPELINES];	// Holds IDs of all graphics pipelines.

	uint8_t windowID;	// Used for interfacing with GLFW window.
} wsVulkan;

// Vulkan interfacing functions.
void wsVulkanInit(wsVulkan* vk, uint8_t windowID);
void wsVulkanStop(wsVulkan* vk);

// Self-explanatory.
void wsVulkanSetDebug(uint8_t debug_mode);

#endif