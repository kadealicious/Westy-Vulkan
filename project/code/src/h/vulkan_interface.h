#ifndef VULKAN_INTERFACE_H_
#define VULKAN_INTERFACE_H_

#include<stdbool.h>

#include <vulkan/vulkan.h>

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
	VkDevice			logical_device;	// Primary logical device used to interface with the physical device.
	VkDebugUtilsMessengerEXT debug_messenger;	// Main debug messenger.

	wsVulkanQueueFamilyIndices indices;
	VkQueue graphics_queue;
	VkQueue present_queue;

	VkSwapchainKHR swapchain;
	wsVulkanSwapChainInfo swapchain_info;

	uint8_t windowID;
} wsVulkan;

// Vulkan interfacing functions.
void wsVulkanInit(wsVulkan* vk, uint8_t windowID);
void wsVulkanStop(wsVulkan* vk);

// Self-explanatory.
void wsVulkanSetDebug(uint8_t debug_mode);

#endif