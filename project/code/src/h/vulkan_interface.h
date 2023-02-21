#ifndef VULKAN_INTERFACE_H_
#define VULKAN_INTERFACE_H_


#include<stdbool.h>
#include <vulkan/vulkan.h>
#include"shader.h"


#define NUM_MAX_FRAMES_IN_FLIGHT 2


// All the data Vulkan could possibly want!
typedef struct wsVulkanQueueFamilies {
	
	// Used for drawing graphics!
	bool has_graphics_family;// Have we found a graphical queue family?
	uint32_t ndx_graphics_family;// Index of graphical queue family.
	VkQueue graphics_queue;	// Graphics queue family.
	
	// Used for presenting surfaces to the window.
	bool has_present_family;// Have we found a presentation queue family?
	uint32_t ndx_present_family;// Index of presentation queue family.
	VkQueue present_queue;	// Presentation queue family.
	
} wsVulkanQueueFamilies;
typedef struct wsVulkanSwapChain {
	
	VkSwapchainKHR sc;						// Actual Vulkan swap chain instance.
	VkFramebuffer* framebuffers;			// Stores framebuffers for rendering images to the swap chain!
	VkSurfaceCapabilitiesKHR capabilities;	// Specifies capabilities of swapchain.
	uint32_t current_frame;					// Stores current frame we are cycled to in the swapchain.
	bool framebuffer_hasresized;			// Has our framebuffer been resized?
	
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
	
} wsVulkanSwapChain;
typedef struct wsVulkan {
	
	VkInstance instance;					// Main Vulkan instance.
	VkPhysicalDevice	physical_device;	// Primary physical device.  Implicitly destroyed when Vulkan instance is destroyed.
	VkDevice			logical_device;		// Primary logical device used to interface with the physical device.
	VkDebugUtilsMessengerEXT debug_messenger;	// Main debug messenger.
	
	wsVulkanQueueFamilies queues;	// Contains all queue data.
	VkCommandPool commandpool;		// Pool for sending queued and sending to Vulkan for execution.
	VkCommandBuffer* commandbuffers;// Buffer(s) used for recording commands to for sending to command pool.
	
	VkSemaphore* img_available_semaphores;	// Used to check if GPU has any image(s) available for rendering.
	VkSemaphore* render_finish_semaphores;	// Used to check if GPU has finished rendering available image(s).
	VkFence* inflight_fences;				// Used to check if image(s) can be processed by the CPU.
	
	wsVulkanSwapChain swapchain;	// Contains all swapchain data.
	VkRenderPass renderpass;	// Contains the render pass data.
	VkPipelineLayout pipeline_layout;	// Contains the pipeline layout data.
	VkPipeline pipeline;	// Contains all graphics pipeline data.
	
	wsShader shader;	// Used for loading and interfacing with shaders.
	
	uint8_t windowID;	// Used for interfacing with GLFW window.
	VkSurfaceKHR surface;	// Window surface for drawing.
	
} wsVulkan;


// Vulkan external interfacing functions.
void wsVulkanInit(wsVulkan* vulkan_data, uint8_t windowID);
VkResult wsVulkanDrawFrame();
void wsVulkanStop();

void wsVulkanFramebufferResizeCallback(GLFWwindow* window, int width, int height);	// Used for interfacing in window.c.
void wsVulkanSetDebug(uint8_t debug_mode);


#endif