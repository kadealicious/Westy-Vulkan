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
	bool		isActive;
	wsMesh		mesh;
	wsTexture	texture;
}
wsRenderObject;

typedef struct wsUniformBufferObject
{
	mat4 model;
	mat4 view;
	mat4 proj;
}
wsUBO;

typedef struct wsGlobalPushConstantBlock
{
	float time;
}
wsGPCB;

typedef struct wsQueueFamilies
{
	uint32_t*	uniqueQueueFamilyIndices;
	uint8_t		uniqueQueueFamilyCount;
	
	bool		hasGraphicsFamily;	// This should always be true.
	uint32_t	graphicsFamilyIndex;
	VkQueue		graphicsQueue;
	
	/* Used for transferring data from the CPU to the GPU.  This family is implicitly 
		supported if the GPU can queue graphics/compute operations, however explicitly 
		specifying it allows for parallel command execution between the unique queue families. */
	bool		hasTransferFamily;	// This should always be true.
	uint32_t	transferFamilyIndex;
	VkQueue		transferQueue;
	
	bool		hasPresentFamily;	// This should always be true.
	uint32_t	presentFamilyIndex;
	VkQueue		presentQueue;
}
wsQueueFamilies;

typedef struct wsSwapChain
{
	VkSwapchainKHR		sc;				// Actual Vulkan swap chain instance.
	VkFramebuffer*		framebuffers;	// Stores framebuffers for rendering images to the swap chain!
	VkCommandBuffer*	cmdBuffers;
	uint32_t			currentFrame;		// Stores current frame we are cycled to in the swapchain.
	bool				framebufferHasResized;	// Has our framebuffer been resized?
	
	VkSampleCountFlagBits MSAASamples;
	wsTexture colorTexture;
	wsTexture depthTexture;
	
	VkExtent2D		extent;			// Extent (resolution) of swapchain images.
	uint32_t		imageCount;		// Number of images allowed for swapchain buffering.  Default is 4.
	VkImage*		images;			// Pointer to array of swap chain images.
	VkImageView*	imageViews;		// Specifies how we use and view each image within the swap chain.
	VkFormat		imageFormat;	// Specifies image format for swapchain to use.
	
	VkSurfaceFormatKHR	surfaceFormat;		// Selected surface format.
	VkSurfaceFormatKHR* supportedFormats;
	uint32_t			supportedFormatCount;

	VkPresentModeKHR	presentMode;	// Selected presentation mode.
	VkPresentModeKHR*	supportedPresentModes;
	uint32_t			supportedPresentModeCount;
	
	VkSurfaceCapabilitiesKHR capabilities;
}
wsSwapChain;

typedef struct wsVulkan
{
	VkInstance				 instance;
	VkPhysicalDevice		 physicalDevice;	// Primary physical device.  Implicitly destroyed when Vulkan instance is destroyed.
	VkDevice				 logicalDevice;
	VkDebugUtilsMessengerEXT debugMessenger;
	
	wsSwapChain			swapchain;
	VkSurfaceKHR		surface;
	uint8_t				windowID;
	VkPipeline			pipeline;
	VkPipelineLayout	pipelineLayout;
	VkRenderPass		renderPass;

	wsQueueFamilies	queues;					// Contains all queue data.
	VkCommandPool	cmdPoolGraphics;	// Pool for sending graphics/presentation commands to Vulkan for execution.
	VkCommandPool	cmdPoolTransfer;	// Pool for sending transfer commands to Vulkan for execution.
	
	VkFence*		inFlightFences;			// Used to check if image(s) can be processed by the CPU.
	VkSemaphore*	imageAvailableSemaphores;	// Used to check if GPU has any image(s) available for rendering.
	VkSemaphore*	renderFinishSemaphores;	// Used to check if GPU has finished rendering available image(s).
	
	VkBuffer*		uniformBuffers;
	VkDeviceMemory*	uniformBuffersMemory;
	void**			uniformBuffersMapped;
	
	wsShader shader;
	VkPushConstantRange globalPushConstantRange;
	wsGPCB globalPushConstants;
	
	// Necessary for calculating UBO view matrix.
	vec3* cameraPosition;
	vec3* cameraForward;
	vec3* cameraUp;
	mat4* cameraProjection;
	
	VkSampler texturesampler;
	
	wsRenderObject	renderObjects[WS_MAX_RENDER_OBJECTS];
	wsRenderObject	testRenderObject;
	wsTexture		testTexture;
	wsMesh			testMesh;
	
	bool supportsRayTracing;
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