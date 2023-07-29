#ifndef VULKAN_INTERFACE_H_
#define VULKAN_INTERFACE_H_


#include<stdbool.h>
#include<time.h>

#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include"shader.h"
#include"mesh.h"
#include"texture.h"
#include"camera.h"


#define WS_MAX_FRAMES_IN_FLIGHT 2
#define WS_MAX_VERTEX_BUFFERS 4
#define WS_MAX_DESCRIPTOR_BUFFERS 10
#define WS_MAX_RENDER_OBJECTS 150

#define WS_XAXIS_INIT	{1.0f, 0.0f, 0.0f}
#define WS_YAXIS_INIT	{0.0f, 1.0f, 0.0f}
#define WS_ZAXIS_INIT	{0.0f, 0.0f, 1.0f}
#define WS_XAXIS		((vec3)WS_XAXIS_INIT)
#define WS_YAXIS		((vec3)WS_YAXIS_INIT)
#define WS_ZAXIS		((vec3)WS_ZAXIS_INIT)


typedef struct wsRenderObject
{
	bool isActive;
	wsMesh mesh;
	wsTexture texture;
}
wsRenderObject;

typedef struct wsUniformBufferObject
{
	mat4 model;
	mat4 view;
	mat4 proj;
}
wsUBO;

typedef struct wsQueueFamilies
{
	uint32_t* unique_queue_family_indices;
	uint8_t num_unique_queue_families;
	
	// Used for drawing graphics!
	bool has_graphics_family;		// Have we found a graphical queue family?
	uint32_t ndx_graphics_family;	// Index of graphical queue family.
	VkQueue graphics_queue;			// Graphics queue family.
	
	/* Used for transferring data from the CPU to the GPU; very useful for vertex buffers!
		This family is implicitly supported if the GPU can queue graphics/compute operations, 
		however explicitly specifying it may allow for parallel command execution! */
	bool has_transfer_family;		// Have we found a transfer-capable queue family?
	uint32_t ndx_transfer_family;	// Index of transfer queue family.
	VkQueue transfer_queue;			// Transfer queue family.
	
	// Used for presenting surfaces to the window.
	bool has_present_family;	// Have we found a presentation queue family?
	uint32_t ndx_present_family;// Index of presentation queue family.
	VkQueue present_queue;		// Presentation queue family.
}
wsQueueFamilies;

typedef struct wsSwapChain
{
	VkSwapchainKHR sc;						// Actual Vulkan swap chain instance.
	VkFramebuffer* framebuffers;			// Stores framebuffers for rendering images to the swap chain!
	VkCommandBuffer* cmdBuffers;
	
	uint32_t current_frame;					// Stores current frame we are cycled to in the swapchain.
	bool framebuffer_hasresized;			// Has our framebuffer been resized?
	
	VkSampleCountFlagBits MSAASamples;
	wsTexture colorTexture;
	wsTexture depthTexture;
	
	VkExtent2D extent;			// Extent (resolution) of swapchain images.
	uint32_t num_images;		// Number of images allowed for swapchain buffering.  Default is 4.
	VkImage* images;			// Pointer to array of swap chain images.
	VkImageView* image_views;	// Specifies how we use and view each image within the swap chain.
	VkFormat image_format;		// Specifies image format for swapchain to use.
	
	VkSurfaceCapabilitiesKHR capabilities;	// Specifies capabilities of swapchain.
	VkSurfaceFormatKHR surface_format;	// Selected surface format.
	VkSurfaceFormatKHR* formats;		// List of supported surface formats.
	uint32_t num_formats;				// Number of supported surface formats.

	VkPresentModeKHR present_mode;	// Selected presentation mode.
	VkPresentModeKHR* present_modes;// List of supported presentation modes.
	uint32_t num_present_modes;		// Number of supported presentation modes.
}
wsSwapChain;

typedef struct wsVulkan
{
	VkInstance				 instance;
	VkPhysicalDevice		 physical_device;	// Primary physical device.  Implicitly destroyed when Vulkan instance is destroyed.
	VkDevice				 logical_device;
	VkDebugUtilsMessengerEXT debug_messenger;
	
	wsSwapChain			swapchain;
	VkSurfaceKHR		surface;
	VkPipeline			pipeline;
	VkPipelineLayout	pipeline_layout;
	VkRenderPass		renderpass;

	wsQueueFamilies	queues;					// Contains all queue data.
	VkCommandPool	commandpool_graphics;	// Pool for sending graphics/presentation commands to Vulkan for execution.
	VkCommandPool	commandpool_transfer;	// Pool for sending transfer commands to Vulkan for execution.
	
	VkFence*		inflight_fences;			// Used to check if image(s) can be processed by the CPU.
	VkSemaphore*	img_available_semaphores;	// Used to check if GPU has any image(s) available for rendering.
	VkSemaphore*	render_finish_semaphores;	// Used to check if GPU has finished rendering available image(s).
	
	// Buffer containing all UBOs with transformation matrices for projection.
	VkBuffer*		uniformbuffers;
	VkDeviceMemory*	uniformbuffers_memory;
	void**			uniformbuffers_mapped;
	
	// Necessary for calculating UBO view matrix.
	vec3* cameraPosition;
	vec3* cameraForward;
	vec3* cameraUp;
	mat4* cameraProjection;
	
	VkSampler texturesampler;
	
	wsRenderObject renderObjects[WS_MAX_RENDER_OBJECTS];
	wsRenderObject testRenderObject;
	wsTexture testTexture;
	wsMesh testMesh;
	
	wsShader shader;
	
	uint8_t windowID;
}
wsVulkan;


// These should only be called from main.c.  If they are called elsewhere, you fucked up.
void wsVulkanInit(wsVulkan* vulkan_data, uint8_t windowID, bool isDebug);
VkResult wsVulkanDrawFrame(double delta_time);
void wsVulkanTerminate();

wsRenderObject* wsVulkanCreateRenderObject(const char* meshPath, const char* texPath);
void wsVulkanDestroyRenderObject(wsRenderObject* renderObject);

void wsVulkanCreateTexture(const char* texPath, wsTexture* texture);
void wsVulkanDestroyTexture(wsTexture* texture);
VkResult wsVulkanCreateTextureImage(wsTexture* texture, const char* path);
VkResult wsVulkanCreateImageView(VkImage* image, VkImageView* image_view, VkFormat format, VkImageAspectFlags aspect_flags, uint32_t mipLevels);

void wsVulkanSetDebug(uint8_t debug_mode);
float wsVulkanGetAspectRatio();


#endif