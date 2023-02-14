#ifndef VULKAN_INTERFACE_H_
#define VULKAN_INTERFACE_H_

#include<stdbool.h>

#include <vulkan/vulkan.h>

#include"shader.h"

#define NUM_MAX_PIPELINES 100

// Queue family storage.
typedef struct wsVulkanQueueFamilies {
	// Used for drawing graphics!
	uint32_t ndx_graphics_family;// Index of graphical queue family.
	bool has_graphics_family;// Have we found a graphical queue family?
	VkQueue graphics_queue;	// Graphics queue family.
	
	// Used for presenting surfaces to the window.
	uint32_t ndx_present_family;// Index of presentation queue family.
	bool has_present_family;// Have we found a presentation queue family?
	VkQueue present_queue;	// Presentation queue family.
	
} wsVulkanQueueFamilies;

// Swap chain support details.
typedef struct wsVulkanSwapChain {
	VkSwapchainKHR sc;			// Actual Vulkan swap chain object.
	VkFramebuffer* framebuffers;// Stores framebuffers for rendering images to the swap chain!
	
	VkExtent2D extent;
	uint32_t num_images;		// Number of images allowed for swapchain buffering.  Default is 4.
	VkImage* images;			// Pointer to array of swap chain images.
	VkImageView* image_views;	// Specifies how we use and view each image within the swap chain.
	VkFormat image_format;		// Specifies image format for swapchain to use.

	VkSurfaceFormatKHR surface_format;	// Selected surface format.
	VkSurfaceFormatKHR* formats;		// List of supported surface formats.
	uint32_t num_formats;				// Number of supported surface formats.

	VkPresentModeKHR present_mode;	// Selected presentation mode.
	VkPresentModeKHR* present_modes;// List of supported presentation modes.
	uint32_t num_present_modes;		// Number of supported presentation modes.
	
	VkSurfaceCapabilitiesKHR capabilities;	// Specifies capabilities of swapchain.

} wsVulkanSwapChain;

// Vulkan object management.
typedef struct wsVulkan {
	VkInstance instance;	// Main Vulkan instance.
	VkSurfaceKHR surface;	// Window surface for drawing.
	VkPhysicalDevice	physical_device;	// Primary physical device.  Implicitly destroyed when Vulkan instance is destroyed.
	VkDevice			logical_device;		// Primary logical device used to interface with the physical device.
	VkDebugUtilsMessengerEXT debug_messenger;	// Main debug messenger.

	wsVulkanQueueFamilies queues;	// Contains all queue data.
	
	VkCommandPool	commandpool;	// Pool for queueing commands and sending to Vulkan for execution.
	VkCommandBuffer	commandbuffer;	// 
	
	wsVulkanSwapChain swapchain;	// Contains all swapchain data.
	VkRenderPass renderpass;	// Contains the render pass data.
	VkPipelineLayout pipeline_layout;	// Contains the pipeline layout data.
	VkPipeline pipeline;	// Contains all graphics pipeline data.
	uint8_t pipelineIDs[NUM_MAX_PIPELINES];	// Holds IDs of all graphics pipelines. // TODO: REMOVE THIS!!!
	wsShader shader;	// Used for loading and interfacing with shaders.
	
	uint8_t windowID;	// Used for interfacing with GLFW window.
} wsVulkan;

// Vulkan interfacing functions.
void wsVulkanInit(wsVulkan* vk, uint8_t windowID);
void wsVulkanStop(wsVulkan* vk);

// Self-explanatory.
void wsVulkanSetDebug(uint8_t debug_mode);

#endif