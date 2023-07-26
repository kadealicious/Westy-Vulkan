#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>
#include<limits.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include<CGLM/cglm.h>
#define STB_IMAGE_IMPLEMENTATION
#include"h/stb_image.h"

#include"h/graphics.h"
#include"h/window.h"


enum WS_VK_ID_STATE
	{ NONE = INT32_MAX };
enum WS_VK_COMMAND_TYPE
	{ COMMAND_GRAPHICS, COMMAND_TRANSFER, COMMAND_QUEUE_OWNERSHIP_TRANSFER };
enum GPU_INCOMPATIBILITY_CODES
{
	NO_GEOMETRY_SHADER = -100, NO_GRAPHICS_FAMILY, NO_TRANSFER_FAMILY, NO_PRESENTATION_FAMILY, 
	NO_DEVICE_EXTENSION_SUPPORT, NO_SWAPCHAIN_SUPPORT
};


// Contains pointer to all Vulkan data; instantiation happens in main.c.
wsVulkan* vk;


// The meaty bits.
void wsVulkanInit(wsVulkan* vulkan_data, uint8_t windowID, bool isDebug);
VkResult wsVulkanDrawFrame(double delta_time);
void wsVulkanTerminate();

// Debug & debug-messenger-related functions.
uint8_t debug;
void wsVulkanSetDebug(uint8_t debug_mode) { debug = debug_mode; }
VkResult wsVulkanInitDebugMessenger();
void wsVulkanStopDebugMessenger();
void wsVulkanPopulateDebugMessengerCreationInfo(VkDebugUtilsMessengerCreateInfoEXT* create_info);

// Verify device and platform functionality with Vulkan.
bool wsVulkanEnableValidationLayers(VkInstanceCreateInfo* create_info);					// Enabled validation layers for detailed debugging.		
bool wsVulkanEnableRequiredExtensions(VkInstanceCreateInfo* create_info);				// Enabled extensions required by GLFW, debug mode, etc.
void wsVulkanAddDebugExtensions(const char*** extensions, uint32_t* num_extensions);	// Adds debug extension name to extensions**.

// Queue family management.
uint8_t wsVulkanCountUniqueQueueIndices();
void wsVulkanPopulateUniqueQueueFamiliesArray();
uint32_t wsVulkanFindQueueFamilies(wsVulkanQueueFamilies* indices, VkPhysicalDevice* physical_device, VkSurfaceKHR* surface);	// Finds required queue families and stores them in indices.
bool wsVulkanHasFoundAllQueueFamilies(wsVulkanQueueFamilies* indices);	// Checks if all required queue families have been found.

// Choose a physical device and set up a logical device for interfacing.
bool wsVulkanPickPhysicalDevice();											// Picks the best-suited GPU for our program.
int32_t wsVulkanRatePhysicalDevice(VkPhysicalDevice* physical_device);		// Rates GPU based on device features and properties.
bool wsVulkanCheckDeviceExtensionSupport(VkPhysicalDevice* physical_device);// Queries whethor or not physical_device supports required extensions.
void wsVulkanQuerySwapChainSupport();										// Queries whether or not physical_device can support features required by Vulkan.
VkResult wsVulkanCreateLogicalDevice(uint32_t num_validation_layers, const char* const* validation_layers);	// Creates a logical device for interfacing with the GPU.

// Did someone say swap meet?  How bazaar.
VkResult wsVulkanCreateSwapChain();	// Creates a swap chain for image buffering.
void wsVulkanRecreateSwapChain();	// Recreates the swap chain when it is no longer compatible with the window surface (typically on resize).
void wsVulkanDestroySwapChain();	// Destroys swap chain.
void wsVulkanChooseSwapSurfaceFormat(wsVulkanSwapChain* swapchain_info);// Choose swap chain surface format.
void wsVulkanChooseSwapExtent();										// Choose swap chain image resolution.
void wsVulkanChooseSwapPresentMode(wsVulkanSwapChain* swapchain_info);	// Choose swap chain presentation mode.

uint32_t wsVulkanCreateImageViews();		// Creates image views viewing swap chain images; returns number of image views created successfully.
VkResult wsVulkanCreateFrameBuffers();		// Creates framebuffers that reference image views representing image attachments (color, depth, etc.).
VkResult wsVulkanCreateSurface();			// Creates a surface for drawing to the screen; stores inside of struct vk.
VkResult wsVulkanCreateRenderPass();		// Creates a render pass; stores inside of struct vk.
VkResult wsVulkanCreateGraphicsPipeline();	// Creates a graphics pipeline; stores inside of struct vk.

// Very useful for getting data into GPU memory for use in shaders.
VkResult wsVulkanCreateDescriptorPool();
VkResult wsVulkanCreateDescriptorSets();
VkResult wsVulkanCreateDescriptorSetLayout();
VkShaderModule wsVulkanCreateShaderModule(uint8_t shaderID);	// Creates a shader module for the indicated shader.

VkResult wsVulkanCreateSyncObjects();		// Creates semaphores (for GPU command execution syncing) & fence(s) (for GPU & CPU command execution syncing).
VkResult wsVulkanCreateCommandPool();		// Create command pool for queueing commands to Vulkan.
VkResult wsVulkanCreateCommandBuffers();	// Creates command buffer for holding commands.
VkResult wsVulkanBeginSingleTimeCommands(VkCommandBuffer* commandbuffer, int32_t commandtype);
VkResult wsVulkanEndSingleTimeCommands(VkCommandBuffer* commandbuffer, int32_t commandtype);
VkResult wsVulkanRecordCommandBuffer(VkCommandBuffer* buffer, uint32_t img_ndx);

// Used to associate models and textures.
wsRenderObject* wsVulkanCreateRenderObject(const char* meshPath, const char* texPath);
void wsVulkanDestroyRenderObject(wsRenderObject* renderObject);

// Depth buffering.
VkFormat wsVulkanFindDepthFormat();
bool wsVulkanHasStencilComponent(VkFormat format);
VkResult wsVulkanCreateDepthResources();	// Creates depth buffer image and associated resources.

// Functions for creating and handling images and their memory.
VkResult wsVulkanCreateImageView(VkImage* image, VkImageView* image_view, VkFormat format, VkImageAspectFlags aspect_flags);
VkResult wsVulkanCreateTextureSampler();
VkResult wsVulkanCreateTextureImage(VkImage* tex_image, VkDeviceMemory* image_memory, const char* path);
VkFormat wsVulkanFindSupportedImageFormat(VkFormat* candidates, uint8_t num_candidates, VkImageTiling desired_tiling, VkFormatFeatureFlags desired_features);
void wsVulkanTransitionImageLayout(VkImage image, VkFormat format, VkImageLayout layout_old, VkImageLayout layout_new);
void wsVulkanCopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

// Buffer stuffer.
VkResult wsVulkanCopyBuffer(VkBuffer buffer_src, VkBuffer buffer_dst, VkDeviceSize size);
VkResult wsVulkanCreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer *buffer, VkDeviceMemory *buffer_memory);
VkResult wsVulkanCreateVertexBuffer(wsMesh* mesh);
VkResult wsVulkanCreateIndexBuffer(wsMesh* mesh);
VkResult wsVulkanCreateUniformBuffers();
void wsVulkanUpdateUniformBuffer(uint32_t current_frame, double delta_time);

// Vulkan proxy functions.
VkResult wsVulkanCreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* create_info, const VkAllocationCallbacks* allocator, VkDebugUtilsMessengerEXT* debug_messenger);
void wsVulkanDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debug_messenger, const VkAllocationCallbacks* allocator);
static VKAPI_ATTR VkBool32 VKAPI_CALL wsVulkanDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT msg_severity, VkDebugUtilsMessageTypeFlagsEXT msg_type, const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data);

// Extras.
void wsVulkanFramebufferResizeCallback(GLFWwindow* window, int width, int height);	// Passed to GLFW for the window resize event.
float wsVulkanGetAspectRatio();
void wsVulkanPrint(const char* str, int32_t ID, int32_t numerator, int32_t denominator, VkResult result);		// Always prints.
void wsVulkanPrintQuiet(const char* str, int32_t ID, int32_t numerator, int32_t denominator, VkResult result);	// Only prints on error.
uint32_t clamp(uint32_t num, uint32_t min, uint32_t max) {const uint32_t t = num < min ? min : num;return t > max ? max : t;}	// Clamp function!  WHY DOESN'T C HAVE THIS BUILT IN


// Call after wsWindowInit().
void wsVulkanInit(wsVulkan* vulkan_data, uint8_t windowID, bool isDebug)
{
	wsVulkanSetDebug(isDebug);
	
	// Non-Vulkan-specific initialization stuff.
	vk = vulkan_data;								// Point to program data!!!
	vk->swapchain.framebuffer_hasresized = false;	// Ensure framebuffer is not immediately and unnecessarily resized.
	vk->windowID = windowID;																		// Specify which window we will be rendering to.
	glfwSetWindowUserPointer(wsWindowGetPtr(windowID), vk);											// Set the window user to our vulkan data.
	glfwSetFramebufferSizeCallback(wsWindowGetPtr(windowID), wsVulkanFramebufferResizeCallback);	// Allow Vulkan to resize its framebuffer when the window resizes.

	printf("\n---BEGIN VULKAN INITIALIZATION---\n");

	// Specify application info and store inside struct create_info.
	VkApplicationInfo app_info = {};
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName = "Westy Vulkan";
	app_info.applicationVersion = VK_MAKE_API_VERSION(0, 0, 0, 1);
	app_info.pEngineName = "Westy Vulkan";
	app_info.engineVersion = VK_MAKE_API_VERSION(0, 0, 0, 1);
	app_info.apiVersion = VK_API_VERSION_1_3;
	app_info.pNext = NULL;
	
	VkInstanceCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_info.pApplicationInfo = &app_info;
	
	// Check that we have all the required extensions for Vulkan.
	wsVulkanEnableRequiredExtensions(&create_info);
	
	// If debug, we will need this struct in a moment for vkCreateInstance().
	VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {};
	if(debug)
	{
		wsVulkanEnableValidationLayers(&create_info);
		wsVulkanPopulateDebugMessengerCreationInfo(&debug_create_info);	// Allow the debug messenger to debug vkCreateInstance().
		create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debug_create_info;
	}
	
	// Create Vulkan instance!
	VkResult result = vkCreateInstance(&create_info, NULL, &vk->instance);
	wsVulkanPrint("instance creation", NONE, NONE, NONE, result);
	
	// Sends messages to stdout when debug messages occur.
	if(debug)
		{ wsVulkanInitDebugMessenger(); }
	
	wsVulkanCreateSurface();	// Creates window surface for drawing.
	wsVulkanPickPhysicalDevice();	// Physical device == GPU.
	wsVulkanCreateLogicalDevice(create_info.enabledLayerCount, create_info.ppEnabledLayerNames);	// Logical device interfaces with physical device for us.
	wsVulkanCreateSwapChain();		// Swap chain is in charge of swapping images to the screen when they are needed.
	wsVulkanCreateImageViews();		// Image views describe how we will use, write-to, and read each image.
	wsVulkanCreateRenderPass();
	wsVulkanCreateDescriptorSetLayout();
	vk->testMesh = *wsMeshInit();
	wsVulkanCreateGraphicsPipeline();	// Graphics pipeline combines all created objects and information into one abstraction.
	wsVulkanCreateDepthResources();		// Create depth buffer image and memory.
	wsVulkanCreateFrameBuffers();		// Creates framebuffer objects for interfacing with image view attachments.
	wsVulkanCreateCommandPool();		// Creates command pools, which are used for executing commands sent via command buffer.
	vk->testTexture = *wsTextureInit(&vk->logical_device);
	wsVulkanCreateTextureSampler();	// Creates a texture sampler for use with ALL textures!
	vk->testRenderObject = *wsVulkanCreateRenderObject("models/vikingroom.gltf", "textures/vikingroom.png");
	wsVulkanCreateVertexBuffer(&vk->testMesh);// Creates vertex buffers which hold our vertex input data.
	wsVulkanCreateIndexBuffer(&vk->testMesh);
	wsVulkanCreateUniformBuffers();
	wsVulkanCreateDescriptorPool();
	wsVulkanCreateDescriptorSets();
	wsVulkanCreateCommandBuffers();		// Creates command buffer(s), used for queueing commands for the command pool to execute.
	wsVulkanCreateSyncObjects();		// Creates semaphores & fences for preventing CPU & GPU sync issues when passing image data around.
	
	printf("---END VULKAN INITIALIZATION---\n");
}

VkResult wsVulkanDrawFrame(double delta_time)
{
	// Wait for any important GPU tasks to finish up first.
	vkWaitForFences(vk->logical_device, 1, &vk->inflight_fences[vk->swapchain.current_frame], VK_TRUE, UINT64_MAX);
	
	// Acquire next swapchain image.  If out of date or suboptimal, recreate it!
	uint32_t img_ndx;
	VkResult result = vkAcquireNextImageKHR(vk->logical_device, vk->swapchain.sc, UINT64_MAX, vk->img_available_semaphores[vk->swapchain.current_frame], VK_NULL_HANDLE, &img_ndx);
	switch(result)
	{
		case VK_SUCCESS: 
		case VK_SUBOPTIMAL_KHR: 
			break;
			
		case VK_ERROR_OUT_OF_DATE_KHR: 
			printf("WARNING: Vulkan swap chain configuration found to be out of date while acquiring next image; recreating!\n");
			wsVulkanRecreateSwapChain();
			return result;
		
		default: 
			printf("ERROR: Vulkan swap chain failed to acquire next image with result code %i!", result);
			return result;
	}
	
	// This may be the incorrect argument; try "image_ndx"?
	wsVulkanUpdateUniformBuffer(vk->swapchain.current_frame, delta_time);
	
	// Reset fences and command buffer, then begin recording commands.
	vkResetFences(vk->logical_device, 1, &vk->inflight_fences[vk->swapchain.current_frame]);
	vkResetCommandBuffer(vk->commandbuffers[vk->swapchain.current_frame], 0);
	wsVulkanRecordCommandBuffer(&vk->commandbuffers[vk->swapchain.current_frame], img_ndx);
	
	// Create configuration for queue submission & synchronization.
	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &vk->commandbuffers[vk->swapchain.current_frame];
	VkSemaphore wait_semaphores[]		= {vk->img_available_semaphores[vk->swapchain.current_frame]};
	VkPipelineStageFlags wait_stages[]	= {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = wait_semaphores;
	submit_info.pWaitDstStageMask = wait_stages;
	VkSemaphore signal_semaphores[] = {vk->render_finish_semaphores[vk->swapchain.current_frame]};
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = signal_semaphores;
	
	// Submit queue to be executed by Vulkan.
	result = vkQueueSubmit(vk->queues.graphics_queue, 1, &submit_info, vk->inflight_fences[vk->swapchain.current_frame]);
	wsVulkanPrintQuiet("draw command buffer submission", NONE, NONE, NONE, result);
	
	// Configure presentation specification.
	VkPresentInfoKHR present_info = {};
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = signal_semaphores;
	VkSwapchainKHR swapchains[] = {vk->swapchain.sc};
	present_info.swapchainCount = 1;
	present_info.pSwapchains = swapchains;
	present_info.pImageIndices = &img_ndx;
	present_info.pResults = NULL;
	
	// DRAW!!!  TO!!!!!!  SCREEN!!!!!!!!!  AAAAAAAAAHHHHHHHHH!!!!!!!!!!!!!!!!!!
	result = vkQueuePresentKHR(vk->queues.present_queue, &present_info);
	
	if(vk->swapchain.framebuffer_hasresized)
	{
		vk->swapchain.framebuffer_hasresized = false;
		printf("WARNING: Vulkan framebuffer found to be wrong size while presenting image; recreating!\n");
		wsVulkanRecreateSwapChain();
		return result;
	}

	// Is it necessary to check this again?  Find out!
	// Update: It is.
	switch(result) {
		
		case VK_SUCCESS: 
			break;
			
		case VK_ERROR_OUT_OF_DATE_KHR: 
			printf("WARNING: Vulkan swap chain configuration found to be out of date while presenting image; recreating!\n");
			wsVulkanRecreateSwapChain();
			return result;
		
		case VK_SUBOPTIMAL_KHR: 
			printf("WARNING: Vulkan swap chain configuration found to be suboptimal while presenting image; recreating!\n");
			wsVulkanRecreateSwapChain();
			return result;
		
		default: 
			printf("ERROR: Vulkan swap chain failed to present image with result code %i!", result);
			return result;
	}
	
	// Loop around to the next frame in the swapchain.
	vk->swapchain.current_frame = (vk->swapchain.current_frame + 1) % WS_VULKAN_MAX_FRAMES_IN_FLIGHT;
	
	return VK_SUCCESS;
}
void wsVulkanTerminate()
{
	// Wait for all asynchronous elements to finish execution and return to an idle state before continuing on.sss
	vkDeviceWaitIdle(vk->logical_device);
	
	printf("\n---BEGIN VULKAN TERMINATION---\n");
	
	if(debug)
	{
		wsVulkanStopDebugMessenger();
	}
	
	// Destroy all sync objects.
	for(uint8_t i = 0; i < WS_VULKAN_MAX_FRAMES_IN_FLIGHT; i++)
		{ vkDestroySemaphore(vk->logical_device, vk->img_available_semaphores[i], NULL);	printf("INFO: Vulkan \"image available?\" semaphore %i/%i destroyed!\n", (i + 1), WS_VULKAN_MAX_FRAMES_IN_FLIGHT); }
	for(uint8_t i = 0; i < WS_VULKAN_MAX_FRAMES_IN_FLIGHT; i++)
		{ vkDestroySemaphore(vk->logical_device, vk->render_finish_semaphores[i], NULL);	printf("INFO: Vulkan \"render finished?\" semaphore %i/%i destroyed!\n", (i + 1), WS_VULKAN_MAX_FRAMES_IN_FLIGHT); }
	for(uint8_t i = 0; i < WS_VULKAN_MAX_FRAMES_IN_FLIGHT; i++)
		{ vkDestroyFence(vk->logical_device, vk->inflight_fences[i], NULL);					printf("INFO: Vulkan \"in flight?\" fence %i/%i destroyed!\n", (i + 1), WS_VULKAN_MAX_FRAMES_IN_FLIGHT); }
	
	// Free memory associated with queue families.
	free(vk->queues.unique_queue_family_indices);
	
	// Command queueing and pooling.
	vkDestroyCommandPool(vk->logical_device, vk->commandpool_graphics, NULL);	printf("INFO: Vulkan graphics/presentation command pool destroyed!\n");
	vkDestroyCommandPool(vk->logical_device, vk->commandpool_transfer, NULL);	printf("INFO: Vulkan transfer command pool destroyed!\n");
	
	vkDestroyPipeline(vk->logical_device, vk->pipeline, NULL);				printf("INFO: Vulkan graphics pipeline destroyed!\n");
	vkDestroyPipelineLayout(vk->logical_device, vk->pipeline_layout, NULL);	printf("INFO: Vulkan pipeline layout destroyed!\n");
	wsVulkanDestroySwapChain();												// Already prints info messages!
	
	vkDestroySampler(vk->logical_device, vk->texturesampler, NULL);			printf("INFO: Vulkan texture sampler destroyed!\n");
	
	for(uint8_t i = 0; i < WS_VULKAN_MAX_RENDER_OBJECTS; i++)
	{
		wsVulkanDestroyRenderObject(&vk->renderObjects[i]);
	}
	// wsVulkanDestroyRenderObject(&vk->testRenderObject);
	wsTextureDestroy(&vk->testTexture);
	wsMeshDestroy(&vk->testMesh, &vk->logical_device);
	
	// Free UBO, vertex, & index buffer memory.
	for(uint8_t i = 0; i < WS_VULKAN_MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroyBuffer(vk->logical_device, vk->uniformbuffers[i], NULL);
		vkFreeMemory(vk->logical_device, vk->uniformbuffers_memory[i], NULL);
	}
	printf("INFO: All Vulkan UBOs destroyed!\n");
	// TODO: Install Valgrind to double check that I am not leaking memory here.  I don't think I am...

	vkDestroyDescriptorPool(vk->logical_device, vk->descriptorpool, NULL);				printf("INFO: Vulkan descriptor pool destroyed!\n");
	vkDestroyDescriptorSetLayout(vk->logical_device, vk->descriptorset_layout, NULL);	printf("INFO: Vulkan descriptor set layout destroyed!\n");
	free(vk->descriptorsets);
	
	vkDestroyRenderPass(vk->logical_device, vk->renderpass, NULL);	printf("INFO: Vulkan render pass destroyed!\n");
	wsShaderUnloadAll(&vk->shader);
	vkDestroyDevice(vk->logical_device, NULL);				printf("INFO: Vulkan logical device destroyed!\n");
	vkDestroySurfaceKHR(vk->instance, vk->surface, NULL);	printf("INFO: Vulkan surface destroyed!\n");
	vkDestroyInstance(vk->instance, NULL);					printf("INFO: Vulkan instance destroyed!\n");
	
	printf("---END VULKAN TERMINATION---\n\n");
}

VkResult wsVulkanCreateSyncObjects()
{
	// Resize arrays to hold all required semaphores & fences.
	vk->img_available_semaphores= malloc(WS_VULKAN_MAX_FRAMES_IN_FLIGHT * sizeof(VkSemaphore));
	vk->render_finish_semaphores= malloc(WS_VULKAN_MAX_FRAMES_IN_FLIGHT * sizeof(VkSemaphore));
	vk->inflight_fences			= malloc(WS_VULKAN_MAX_FRAMES_IN_FLIGHT * sizeof(VkFence));
	
	VkSemaphoreCreateInfo semaphore_info = {};
	semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	
	VkFenceCreateInfo fence_info = {};
	fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;		// Prevents first call to wsVulkanDrawFrame() from hanging up on an unsignaled fence.
	
	// Create all semaphores & fences!
	for(uint8_t i = 0; i < WS_VULKAN_MAX_FRAMES_IN_FLIGHT; i++)
		{ VkResult result = vkCreateSemaphore(vk->logical_device, &semaphore_info, NULL, &vk->img_available_semaphores[i]);	wsVulkanPrint("\"image available?\" semaphore creation", NONE, (i + 1), WS_VULKAN_MAX_FRAMES_IN_FLIGHT, result); }
	for(uint8_t i = 0; i < WS_VULKAN_MAX_FRAMES_IN_FLIGHT; i++)
		{ VkResult result = vkCreateSemaphore(vk->logical_device, &semaphore_info, NULL, &vk->render_finish_semaphores[i]);	wsVulkanPrint("\"render finished?\" semaphore creation", NONE, (i + 1), WS_VULKAN_MAX_FRAMES_IN_FLIGHT, result); }
	for(uint8_t i = 0; i < WS_VULKAN_MAX_FRAMES_IN_FLIGHT; i++)
		{ VkResult result = vkCreateFence(vk->logical_device, &fence_info, NULL, &vk->inflight_fences[i]);	wsVulkanPrint("\"in flight?\" semaphore creation", NONE, (i + 1), WS_VULKAN_MAX_FRAMES_IN_FLIGHT, result); }
	
	return VK_SUCCESS;
}

VkResult wsVulkanRecordCommandBuffer(VkCommandBuffer* commandbuffer, uint32_t img_ndx)
{
	// Begin recording to the command buffer!
	VkCommandBufferBeginInfo begin_info = {};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = 0;
	begin_info.pInheritanceInfo = NULL;
	
	VkResult result = vkBeginCommandBuffer(*commandbuffer, &begin_info);
	wsVulkanPrintQuiet("command buffer recording begin", NONE, NONE, NONE, result);
	
	// Begin render pass.
	VkRenderPassBeginInfo renderpass_info = {};
	renderpass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderpass_info.renderPass = vk->renderpass;
	renderpass_info.framebuffer = vk->swapchain.framebuffers[img_ndx];
	renderpass_info.renderArea.offset.x = 0;
	renderpass_info.renderArea.offset.y = 0;
	renderpass_info.renderArea.extent = vk->swapchain.extent;
	
	VkClearValue clear_color = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
	VkClearValue clear_depth = {{{1.0f, 0}}};
	VkClearValue clear_values[2] = {clear_color, clear_depth};
	renderpass_info.clearValueCount = 2;
	renderpass_info.pClearValues = &clear_values[0];
	
	vkCmdBeginRenderPass(*commandbuffer, &renderpass_info, VK_SUBPASS_CONTENTS_INLINE);
	
	// Bind graphics pipeline, then specify viewport & scissor states for this pipeline.
	vkCmdBindPipeline(*commandbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk->pipeline);
	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)vk->swapchain.extent.width;
	viewport.height = (float)vk->swapchain.extent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(*commandbuffer, 0, 1, &viewport);
	VkRect2D scissor = {};
	scissor.offset.x = 0;
	scissor.offset.y = 0;
	scissor.extent = vk->swapchain.extent;
	vkCmdSetScissor(*commandbuffer, 0, 1, &scissor);
	
	// Bind mesh information & draw!
	/*VkBuffer vertexBuffers[WS_VULKAN_MAX_RENDER_OBJECTS];
	VkBuffer indexBuffers[WS_VULKAN_MAX_RENDER_OBJECTS];
	VkDeviceSize offsets[WS_VULKAN_MAX_RENDER_OBJECTS];
	uint8_t bufferCount = 0;
	
	for(uint8_t i = 0; i < WS_VULKAN_MAX_RENDER_OBJECTS; i++)
	{
		if(vk->renderObjects[i].isActive)
		{
			vertexBuffers[bufferCount] = vk->renderObjects[i].mesh.vertexBuffer;
			indexBuffers[bufferCount] = vk->renderObjects[i].mesh.indexBuffer;
			bufferCount++;
			if(bufferCount < WS_VULKAN_MAX_RENDER_OBJECTS)
				{ offsets[bufferCount] = vk->renderObjects[i].mesh.num_vertices * sizeof(wsVertex); }
		}
	}
	
	vkCmdBindVertexBuffers(*commandbuffer, 0, bufferCount, vertexBuffers, offsets);
	vkCmdBindDescriptorSets(*commandbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk->pipeline_layout, 
		0, 1, &vk->descriptorsets[vk->swapchain.current_frame], 0, NULL);
	
	for(uint8_t i = 0; i < bufferCount; i++)
	{
		vkCmdBindIndexBuffer(*commandbuffer, indexBuffers[i], 0, VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(*commandbuffer, (uint32_t)vk->renderObjects[i].mesh.num_indices, 1, 0, 0, 0);
	}*/
	
	// VkBuffer vertexbuffers[1] = {vk->testMesh.vertexBuffer.buffer};
	VkBuffer vertexbuffers[1] = {vk->testRenderObject.mesh.vertexBuffer.buffer};
	VkDeviceSize offsets[1] = {0};
	vkCmdBindVertexBuffers(*commandbuffer, 0, 1, vertexbuffers, offsets);
	// vkCmdBindIndexBuffer(*commandbuffer, vk->testMesh.indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT16);
	vkCmdBindIndexBuffer(*commandbuffer, vk->testRenderObject.mesh.indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT16);
	vkCmdBindDescriptorSets(*commandbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk->pipeline_layout, 
		0, 1, &vk->descriptorsets[vk->swapchain.current_frame], 0, NULL);
	vkCmdDrawIndexed(*commandbuffer, (uint32_t)vk->testRenderObject.mesh.num_indices, 1, 0, 0, 0);
	
	// End render pass.
	vkCmdEndRenderPass(*commandbuffer);
	
	// End command buffer recording.
	result = vkEndCommandBuffer(*commandbuffer);
	wsVulkanPrintQuiet("command buffer recording end", NONE, NONE, NONE, result);
	return result;
}

VkResult wsVulkanCreateCommandBuffers()
{
	// Resize command buffers array to contain enough command buffers for each in-flight frame.
	vk->commandbuffers = malloc(WS_VULKAN_MAX_FRAMES_IN_FLIGHT * sizeof(VkCommandBuffer));
	
	// Specify command buffer allocation configuration.
	VkCommandBufferAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	alloc_info.commandPool = vk->commandpool_graphics;
	alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	alloc_info.commandBufferCount = (uint32_t)WS_VULKAN_MAX_FRAMES_IN_FLIGHT;
	
	// Allocate all command buffers.
	for(uint8_t i = 0; i < WS_VULKAN_MAX_FRAMES_IN_FLIGHT; i++)
	{
		VkResult result = vkAllocateCommandBuffers(vk->logical_device, &alloc_info, vk->commandbuffers);
		wsVulkanPrint("command buffer creation", NONE, (i + 1), WS_VULKAN_MAX_FRAMES_IN_FLIGHT, result);
	}
	
	return VK_SUCCESS;
}

VkResult wsVulkanCreateCommandPool()
{
	VkCommandPoolCreateInfo graphicspool_info = {};
	graphicspool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	graphicspool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	graphicspool_info.queueFamilyIndex = vk->queues.ndx_graphics_family;
	
	VkResult result = vkCreateCommandPool(vk->logical_device, &graphicspool_info, NULL, &vk->commandpool_graphics);
	wsVulkanPrint("graphics/presentation command pool creation", NONE, NONE, NONE, result);
	if(result != VK_SUCCESS)
		return result;
	
	VkCommandPoolCreateInfo transferpool_info = {};
	transferpool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	transferpool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	transferpool_info.queueFamilyIndex = vk->queues.ndx_transfer_family;
	
	result = vkCreateCommandPool(vk->logical_device, &transferpool_info, NULL, &vk->commandpool_transfer);
	wsVulkanPrint("transfer command pool creation", NONE, NONE, NONE, result);
	return result;
}

uint32_t wsVulkanFindMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memory_properties;
	vkGetPhysicalDeviceMemoryProperties(vk->physical_device, &memory_properties);
	
	for(uint32_t i = 0; i < memory_properties.memoryTypeCount; i++)
	{
		// Make sure that we have found the requested memory type and that its read/write properties match our needs.
		if((type_filter & (1 << i)) && (memory_properties.memoryTypes[i].propertyFlags & properties) == properties)
			{ return i; }
	}
	
	printf("ERROR: Vulkan memory type not found!\n");
	return NONE;
}

wsRenderObject* wsVulkanCreateRenderObject(const char* meshPath, const char* texPath)
{
	for(uint8_t i = 0; i < WS_VULKAN_MAX_RENDER_OBJECTS; i++)
	{
		if(!vk->renderObjects[i].isActive)
		{
			vk->renderObjects[i].isActive = true;
			
			wsTextureCreate(texPath, &vk->renderObjects[i].texture);
			
			wsMeshCreate(meshPath, &vk->renderObjects[i].mesh);
			wsVulkanCreateVertexBuffer(&vk->renderObjects[i].mesh);// Creates vertex buffers which hold our vertex input data.
			wsVulkanCreateIndexBuffer(&vk->renderObjects[i].mesh);
			
			return &vk->renderObjects[i];
		}
	}
	
	return &vk->testRenderObject;
}

void wsVulkanDestroyRenderObject(wsRenderObject* renderObject)
{
	if(renderObject->isActive)
	{
		renderObject->isActive = false;
		wsTextureDestroy(&renderObject->texture);
		wsMeshDestroy(&renderObject->mesh, &vk->logical_device);
	}
}

VkResult wsVulkanCreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage* image, VkDeviceMemory *image_memory)
{
	// Initialize VkImage inside of struct vk.
	VkImageCreateInfo image_info = {};
	image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	image_info.imageType = VK_IMAGE_TYPE_2D;
	image_info.extent.width = width;
	image_info.extent.height = height;
	image_info.extent.depth = 1;
	image_info.mipLevels = 1;
	image_info.arrayLayers = 1;
	image_info.format = format;
	image_info.tiling = tiling;
	image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	image_info.usage = usage;
	image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	image_info.samples = VK_SAMPLE_COUNT_1_BIT;
	image_info.flags = 0;	// Optional.
	VkResult result = vkCreateImage(vk->logical_device, &image_info, NULL, image);
	wsVulkanPrint("image creation", (uintptr_t)image, NONE, NONE, result);
	
	// Specify memory requirements for image.
	VkMemoryRequirements memory_requirements;
	vkGetImageMemoryRequirements(vk->logical_device, *image, &memory_requirements);
	
	VkMemoryAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	alloc_info.allocationSize = memory_requirements.size;
	alloc_info.memoryTypeIndex = wsVulkanFindMemoryType(memory_requirements.memoryTypeBits, properties);
	result = vkAllocateMemory(vk->logical_device, &alloc_info, NULL, image_memory);
	wsVulkanPrint("image memory allocation", (uintptr_t)image_memory, NONE, NONE, result);
	
	result = vkBindImageMemory(vk->logical_device, *image, *image_memory, 0);
	return result;
}

VkResult wsVulkanCreateTextureImage(VkImage* tex_image, VkDeviceMemory* image_memory, const char* path)
{
	// First, load the image using stb_image.h.
	int tex_width;
	int tex_height;
	int tex_channels;
	stbi_uc* pixel_data = stbi_load(path, &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
	VkDeviceSize img_size = tex_width * tex_height * 4;	// For STBI_rgb_alpha, there will be 4 bytes needed per pixel.
	if(!pixel_data)
		{ printf("ERROR: Failed to load texture at path \"%s\"!\n", path); }
	else printf("INFO: Texture at path \"%s\" of dimension %ix%i w/ %i channels successfully loaded!\n", 
		path, tex_width, tex_height, tex_channels);
	
	// Next, store the image data into a host(CPU)-visible buffer for staging.
	VkBuffer stagingbuffer;
	VkDeviceMemory stagingbuffer_memory;
	wsVulkanCreateBuffer(img_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | 
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingbuffer, &stagingbuffer_memory);
	
	// Copy pixel data into staging buffer & clean up original pixel array.
	void* data;
	vkMapMemory(vk->logical_device, stagingbuffer_memory, 0, img_size, 0, &data);
		memcpy(data, pixel_data, (size_t)img_size);
	vkUnmapMemory(vk->logical_device, stagingbuffer_memory);
	stbi_image_free(pixel_data);
	
	// Create the image!
	VkResult result = wsVulkanCreateImage(tex_width, tex_height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, tex_image, image_memory);
	wsVulkanPrint("image memory binding", NONE, NONE, NONE, result);
	
	// Copy our staging buffer to a GPU/shader-suitable VkImage format.
	wsVulkanTransitionImageLayout(*tex_image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	wsVulkanCopyBufferToImage(stagingbuffer, *tex_image, (uint32_t)tex_width, (uint32_t)tex_height);
	wsVulkanTransitionImageLayout(*tex_image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	
	// Clean up!
	vkDestroyBuffer(vk->logical_device, stagingbuffer, NULL);
	vkFreeMemory(vk->logical_device, stagingbuffer_memory, NULL);
		
	return result;
}

VkResult wsVulkanCreateImageView(VkImage* image, VkImageView* image_view, VkFormat format, VkImageAspectFlags aspect_flags)
{
	VkImageViewCreateInfo view_info = {};
	view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	view_info.image = *image;
	view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	view_info.format = format;
	// Stick to default color mapping.
	view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	// subresourceRange describes what the image's purpose is, as well as which part of the image we will be accessing.
	view_info.subresourceRange.aspectMask = aspect_flags;
	view_info.subresourceRange.baseMipLevel = 0;
	view_info.subresourceRange.levelCount = 1;
	view_info.subresourceRange.baseArrayLayer = 0;
	view_info.subresourceRange.layerCount = 1;
	
	VkResult result = vkCreateImageView(vk->logical_device, &view_info, NULL, image_view);
	wsVulkanPrint("image view creation", (intptr_t)image_view, NONE, NONE, result);
	return result;
}

void wsVulkanTransitionImageLayout(VkImage image, VkFormat format, VkImageLayout layout_old, VkImageLayout layout_new)
{
	VkImageMemoryBarrier barrier_acquire = {};
	VkImageMemoryBarrier barrier_release = {};	// Only used if queue family ownership transfer is necessary.
	barrier_acquire.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier_acquire.oldLayout = layout_old;
	barrier_acquire.newLayout = layout_new;
	barrier_acquire.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier_acquire.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier_acquire.image = image;
	
	if(layout_new == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		barrier_acquire.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		
		if(wsVulkanHasStencilComponent(format))
			{ barrier_acquire.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT; }
	}
	else barrier_acquire.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	
	barrier_acquire.subresourceRange.baseMipLevel = 0;
	barrier_acquire.subresourceRange.levelCount = 1;
	barrier_acquire.subresourceRange.baseArrayLayer = 0;
	barrier_acquire.subresourceRange.layerCount = 1;
	
	// Set access masks and pipeline stages, so that fragment shaders will, when necessary, wait on the CPU to complete writing to image before reading.
	VkPipelineStageFlags source_stage;
	VkPipelineStageFlags dest_stage;
	int32_t commandtype;
	
	if(layout_old == VK_IMAGE_LAYOUT_UNDEFINED && layout_new == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{	// This case does not require a release barrier, as no queue family ownership changes occur.
		commandtype = COMMAND_TRANSFER;
		
		barrier_acquire.srcAccessMask = 0;
		barrier_acquire.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		
		source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		dest_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		
	}
	else if(layout_old == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && layout_new == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{	/* If we are transferring to a shader-preferred layout AND our transfer queue is not the same as our graphics queue, 
			then this case requires a release barrier from transfer queue family to graphics queue family. */
		
		if(vk->queues.ndx_graphics_family != vk->queues.ndx_transfer_family)
		{
			commandtype = COMMAND_QUEUE_OWNERSHIP_TRANSFER;
			
			barrier_release.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier_release.oldLayout = layout_old;
			barrier_release.newLayout = layout_new;
			barrier_release.srcQueueFamilyIndex = vk->queues.ndx_transfer_family;
			barrier_release.dstQueueFamilyIndex = vk->queues.ndx_graphics_family;
			barrier_release.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier_release.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			barrier_release.image = image;
			barrier_release.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			barrier_release.subresourceRange.baseMipLevel = 0;
			barrier_release.subresourceRange.levelCount = 1;
			barrier_release.subresourceRange.baseArrayLayer = 0;
			barrier_release.subresourceRange.layerCount = 1;
		}
		
		barrier_acquire.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier_acquire.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		
		source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		dest_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if(layout_old == VK_IMAGE_LAYOUT_UNDEFINED && layout_new == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		barrier_acquire.srcAccessMask = 0;
		barrier_acquire.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		
		source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		dest_stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else
	{
		printf("ERROR: Unsupported Vulkan image layout transition!\n");
		printf("\tOld layout: %i, new layout: %i\n", layout_old, layout_new);
	}
	
	if(commandtype == COMMAND_QUEUE_OWNERSHIP_TRANSFER)
	{
		// Begin by transferring queue family ownership via a barrier; keep the stage the same, but indicate that the queue family should release ownership of the image.
		VkCommandBuffer commandbuffer_transfer_ownership;
		wsVulkanBeginSingleTimeCommands(&commandbuffer_transfer_ownership, COMMAND_QUEUE_OWNERSHIP_TRANSFER);
			vkCmdPipelineBarrier(commandbuffer_transfer_ownership, source_stage, source_stage, 0, 0, NULL, 0, NULL, 1, &barrier_release);
		wsVulkanEndSingleTimeCommands(&commandbuffer_transfer_ownership, COMMAND_QUEUE_OWNERSHIP_TRANSFER);
		
		/* Transfer the image to the graphics queue family, as graphics queue families implicitly support transfer queue operations as well 
			as their own, including the VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT stage that we require here (which transfer queues do not support). */
		VkCommandBuffer commandbuffer;
		wsVulkanBeginSingleTimeCommands(&commandbuffer, COMMAND_GRAPHICS);
			vkCmdPipelineBarrier(commandbuffer, source_stage, dest_stage, 0, 0, NULL, 0, NULL, 1, &barrier_acquire);
		wsVulkanEndSingleTimeCommands(&commandbuffer, COMMAND_GRAPHICS);
	}
	else 
	{	// Because no queue family ownership transfer is required here, we can simply change our image's format without a release barrier;
		VkCommandBuffer commandbuffer;
		wsVulkanBeginSingleTimeCommands(&commandbuffer, COMMAND_TRANSFER);
			vkCmdPipelineBarrier(commandbuffer, source_stage, dest_stage, 0, 0, NULL, 0, NULL, 1, &barrier_acquire);
		wsVulkanEndSingleTimeCommands(&commandbuffer, COMMAND_TRANSFER);
	}
}

VkResult wsVulkanCreateTextureSampler()
{
	VkSamplerCreateInfo sampler_info = {};
	sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	sampler_info.magFilter = VK_FILTER_NEAREST;
	sampler_info.minFilter = VK_FILTER_NEAREST;
	sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	
	// Query if our physical device supports anisotropy (Highly likely).
	VkPhysicalDeviceFeatures device_features = {};
	vkGetPhysicalDeviceFeatures(vk->physical_device, &device_features);
	sampler_info.anisotropyEnable = device_features.samplerAnisotropy;
	
	// Get the maximum anisotropy level available to us, and use that!
	if(device_features.samplerAnisotropy)
	{
		VkPhysicalDeviceProperties device_properties = {};
		vkGetPhysicalDeviceProperties(vk->physical_device, &device_properties);
		sampler_info.maxAnisotropy = device_properties.limits.maxSamplerAnisotropy;
	}
	else
	{
		sampler_info.maxAnisotropy = 1.0f;
	}
	
	sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	sampler_info.unnormalizedCoordinates = VK_FALSE;
	sampler_info.compareEnable = VK_FALSE;
	sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
	
	sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	sampler_info.mipLodBias = 0.0f;
	sampler_info.minLod = 0.0f;
	sampler_info.maxLod = 0.0f;
	
	VkResult result = vkCreateSampler(vk->logical_device, &sampler_info, NULL, &vk->texturesampler);
	wsVulkanPrint("texture sampler creation", NONE, NONE, NONE, result);
	return result;
}

void wsVulkanCopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
	VkCommandBuffer commandbuffer;
	wsVulkanBeginSingleTimeCommands(&commandbuffer, COMMAND_TRANSFER);
	
	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = (VkOffset3D){0, 0, 0};
	region.imageExtent = (VkExtent3D){width, height, 1};
	
	vkCmdCopyBufferToImage(commandbuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
	
	wsVulkanEndSingleTimeCommands(&commandbuffer, COMMAND_TRANSFER);
}

bool wsVulkanHasStencilComponent(VkFormat format)
{
	return (format == VK_FORMAT_D32_SFLOAT_S8_UINT) || (format == VK_FORMAT_D24_UNORM_S8_UINT);
}

VkFormat wsVulkanFindDepthFormat()
{
	VkFormat desired_formats[3] = {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT};
	return wsVulkanFindSupportedImageFormat(&desired_formats[0], 3, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

VkFormat wsVulkanFindSupportedImageFormat(VkFormat* candidates, uint8_t num_candidates, VkImageTiling desired_tiling, VkFormatFeatureFlags desired_features)
{
	VkFormat format;
	
	for(uint8_t i = 0; i < num_candidates; i++)
	{
		format = candidates[i];
		
		VkFormatProperties properties;
		vkGetPhysicalDeviceFormatProperties(vk->physical_device, format, &properties);
		
		bool has_linear_features = (properties.linearTilingFeatures & desired_features) == desired_features;
		bool has_optimal_features = (properties.optimalTilingFeatures & desired_features) == desired_features;
		
		if(desired_tiling == VK_IMAGE_TILING_LINEAR && has_linear_features)
		{
			wsVulkanPrintQuiet("supported linear image format search", format, NONE, NONE, VK_SUCCESS);
			return format;
		}
		else if(desired_tiling == VK_IMAGE_TILING_OPTIMAL && has_optimal_features)
		{
			wsVulkanPrintQuiet("supported optimal image format search", format, NONE, NONE, VK_SUCCESS);
			return format;
		}
	}
	
	printf("WARNING: Vulkan supported image format search failed; using last format in candidate list %i!\n", format);
	return format;
}

VkResult wsVulkanCreateDepthResources()
{
	VkFormat depth_format = wsVulkanFindDepthFormat();
	VkResult result = wsVulkanCreateImage(vk->swapchain.extent.width, vk->swapchain.extent.height, depth_format, VK_IMAGE_TILING_OPTIMAL, 
											VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vk->depthimage, 
											&vk->depthimage_memory);
	result = wsVulkanCreateImageView(&vk->depthimage, &vk->depthimage_view, depth_format, VK_IMAGE_ASPECT_DEPTH_BIT);
	
	return result;
}

VkResult wsVulkanBeginSingleTimeCommands(VkCommandBuffer* commandbuffer, int32_t commandtype)
{
	VkCommandPool* commandpool;
	if(commandtype == COMMAND_GRAPHICS)
		{ commandpool = &vk->commandpool_graphics; }
	else { commandpool = &vk->commandpool_transfer; }
	
	VkCommandBufferAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	alloc_info.commandPool = *commandpool;	// I bet this will crash, because I forget if I initialized this command pool.  I guess we'll see, won't we!
	alloc_info.commandBufferCount = 1;
	
	vkAllocateCommandBuffers(vk->logical_device, &alloc_info, commandbuffer);
	
	VkCommandBufferBeginInfo begin_info = {};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	
	VkResult result = vkBeginCommandBuffer(*commandbuffer, &begin_info);
	return result;
}

VkResult wsVulkanEndSingleTimeCommands(VkCommandBuffer* commandbuffer, int32_t commandtype)
{
	vkEndCommandBuffer(*commandbuffer);
	
	VkQueue* queue;
	VkCommandPool* commandpool;
	if(commandtype == COMMAND_GRAPHICS)
	{
		queue = &vk->queues.graphics_queue;
		commandpool = &vk->commandpool_graphics;
	} else
	{
		queue = &vk->queues.transfer_queue;
		commandpool = &vk->commandpool_transfer;
	}
	
	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = commandbuffer;
	VkResult result = vkQueueSubmit(*queue, 1, &submit_info, VK_NULL_HANDLE);
	
	/* TODO: Make this use fences instead of waiting for the queue to idle; this 
		would (allegedly) allow for multiple transfer commands to execute in parallel. */
	vkQueueWaitIdle(*queue);
	
	vkFreeCommandBuffers(vk->logical_device, *commandpool, 1, commandbuffer);
	
	return result;
}

VkResult wsVulkanCopyBuffer(VkBuffer buffer_src, VkBuffer buffer_dst, VkDeviceSize size)
{
	VkCommandBuffer commandbuffer;
	wsVulkanBeginSingleTimeCommands(&commandbuffer, COMMAND_TRANSFER);
	
		VkBufferCopy copyregion = {};
		copyregion.srcOffset = 0;	// Optional.
		copyregion.dstOffset = 0;	// (Also) optional.
		copyregion.size = size;
		vkCmdCopyBuffer(commandbuffer, buffer_src, buffer_dst, 1, &copyregion);
	
	VkResult result = wsVulkanEndSingleTimeCommands(&commandbuffer, COMMAND_TRANSFER);
	
	return result;
}

VkResult wsVulkanCreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer *buffer, VkDeviceMemory *buffer_memory)	// Helper function for wsVulkanCreateVertexBuffer().
{
	// Create vertex buffer.
	VkBufferCreateInfo buffer_info = {};
	buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_info.size = size;
	buffer_info.usage = usage;
	if(vk->queues.num_unique_queue_families > 1)
		buffer_info.sharingMode = VK_SHARING_MODE_CONCURRENT;
	else buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	buffer_info.queueFamilyIndexCount = vk->queues.num_unique_queue_families;
	buffer_info.pQueueFamilyIndices = &vk->queues.unique_queue_family_indices[0];
	
	VkResult result = vkCreateBuffer(vk->logical_device, &buffer_info, NULL, buffer);
	wsVulkanPrintQuiet("buffer creation", (intptr_t)buffer, NONE, NONE, result);
	
	// Specify vertex buffer memory requirements.
	VkMemoryRequirements memory_requirements;
	vkGetBufferMemoryRequirements(vk->logical_device, *buffer, &memory_requirements);
	
	VkMemoryAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	alloc_info.allocationSize = memory_requirements.size;
	VkMemoryPropertyFlags memory_properties = properties;
	alloc_info.memoryTypeIndex = wsVulkanFindMemoryType(memory_requirements.memoryTypeBits, memory_properties);
	
	// Allocate and bind buffer memory to appropriate vertex buffer.
	result = vkAllocateMemory(vk->logical_device, &alloc_info, NULL, buffer_memory);
	wsVulkanPrintQuiet("buffer memory allocation", (intptr_t)buffer_memory, NONE, NONE, result);
	if(result != VK_SUCCESS)
		{ return result; }
	
	result = vkBindBufferMemory(vk->logical_device, *buffer, *buffer_memory, 0);
	return result;
}

VkResult wsVulkanCreateVertexBuffer(wsMesh* mesh)
{
	uint32_t buffer_size = wsMeshGetCurrentVertexBufferSize(mesh);
	
	// Create staging buffer for CPU (host) visibility's sake.
	VkBuffer stagingbuffer;
	VkDeviceMemory stagingbuffer_memory;
	VkResult result = wsVulkanCreateBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
		&stagingbuffer, &stagingbuffer_memory);
	
	void* buffer_data;
	vkMapMemory(vk->logical_device, stagingbuffer_memory, 0, buffer_size, 0, &buffer_data);
		memcpy(buffer_data, &mesh->vertices[0], (size_t)buffer_size);
	vkUnmapMemory(vk->logical_device, stagingbuffer_memory);
	
	// Create actual buffer for GPU streaming.
	result = wsVulkanCreateBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &mesh->vertexBuffer.buffer, &mesh->vertexBuffer.memory);
	
	// Copy the staging buffer's data into the vertex buffer!
	result = wsVulkanCopyBuffer(stagingbuffer, mesh->vertexBuffer.buffer, buffer_size);
	
	vkDestroyBuffer(vk->logical_device, stagingbuffer, NULL);
	vkFreeMemory(vk->logical_device, stagingbuffer_memory, NULL);
	
	return result;
}

VkResult wsVulkanCreateIndexBuffer(wsMesh* mesh)
{
	uint32_t buffer_size = wsMeshGetCurrentIndexBufferSize(mesh);
	
	// Create staging buffer for CPU (host) visibility's sake.
	VkBuffer stagingbuffer;
	VkDeviceMemory stagingbuffer_memory;
	VkResult result = wsVulkanCreateBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
		&stagingbuffer, &stagingbuffer_memory);
	
	void* buffer_data;
	vkMapMemory(vk->logical_device, stagingbuffer_memory, 0, buffer_size, 0, &buffer_data);
		memcpy(buffer_data, &mesh->indices[0], (size_t)buffer_size);
	vkUnmapMemory(vk->logical_device, stagingbuffer_memory);
	
	// Create actual buffer for GPU streaming.
	result = wsVulkanCreateBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &mesh->indexBuffer.buffer, &mesh->indexBuffer.memory);
	
	// Copy the staging buffer's data into the vertex buffer!
	result = wsVulkanCopyBuffer(stagingbuffer, mesh->indexBuffer.buffer, buffer_size);
	
	vkDestroyBuffer(vk->logical_device, stagingbuffer, NULL);
	vkFreeMemory(vk->logical_device, stagingbuffer_memory, NULL);
	
	return result;
}

VkResult wsVulkanCreateUniformBuffers()
{
	VkDeviceSize buffer_size = sizeof(wsVulkanUBO);
	vk->uniformbuffers			= (VkBuffer*)malloc(WS_VULKAN_MAX_FRAMES_IN_FLIGHT * sizeof(VkBuffer));
	vk->uniformbuffers_memory	= (VkDeviceMemory*)malloc(WS_VULKAN_MAX_FRAMES_IN_FLIGHT * sizeof(VkDeviceMemory));
	vk->uniformbuffers_mapped	= (void**)malloc(WS_VULKAN_MAX_FRAMES_IN_FLIGHT * sizeof(void*));
	
	
	VkResult result;
	
	for(uint8_t i = 0; i < WS_VULKAN_MAX_FRAMES_IN_FLIGHT; i++)
	{
		wsVulkanCreateBuffer(buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | 
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &vk->uniformbuffers[i], &vk->uniformbuffers_memory[i]);
		
		// We may crash when we dereference a void* here.
		// vk->uniformbuffers_mapped[i] = (void*)malloc(buffer_size);
		result = vkMapMemory(vk->logical_device, vk->uniformbuffers_memory[i], 0, buffer_size, 0, &vk->uniformbuffers_mapped[i]);
		
		wsVulkanPrintQuiet("uniform buffer creation", NONE, (int32_t)i, (int32_t)WS_VULKAN_MAX_FRAMES_IN_FLIGHT, result);
	}
	
	wsVulkanPrint("uniform buffer creation", NONE, NONE, NONE, result);
	return result;
}

void wsVulkanUpdateUniformBuffer(uint32_t current_frame, double delta_time)
{
	wsVulkanUBO ubo = {};
	
	// Model matrix.
	vec3 rotation_axis = {0.0f, 0.0f, 1.0f};
	static float rotation_amount = 0.0f;
	rotation_amount += delta_time;
	glm_mat4_copy(GLM_MAT4_IDENTITY, ubo.model);
	glm_rotate(ubo.model, sinf(rotation_amount), rotation_axis);
	
	// View matrix.
	vec3 cameraTarget = GLM_VEC3_ZERO_INIT;
	glm_vec3_add(*vk->cameraPosition, *vk->cameraForward, cameraTarget);
	glm_lookat(*vk->cameraPosition, cameraTarget, *vk->cameraUp, ubo.view);
	
	// Projection matrix.
	glm_mat4_copy(*vk->cameraProjection, ubo.proj);
	
	// We may crash when we dereference a void* here.
	memcpy(vk->uniformbuffers_mapped[current_frame], &ubo, sizeof(ubo));
}

VkResult wsVulkanCreateDescriptorPool()
{
	VkDescriptorPoolSize pool_sizes[2] = {};
	pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	pool_sizes[0].descriptorCount = (uint32_t)WS_VULKAN_MAX_FRAMES_IN_FLIGHT;
	pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	pool_sizes[1].descriptorCount = (uint32_t)WS_VULKAN_MAX_FRAMES_IN_FLIGHT;
	
	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.poolSizeCount = 2;
	pool_info.pPoolSizes = &pool_sizes[0];
	pool_info.maxSets = WS_VULKAN_MAX_FRAMES_IN_FLIGHT;
	pool_info.flags = 0;	// Optional.
	
	VkResult result = vkCreateDescriptorPool(vk->logical_device, &pool_info, NULL, &vk->descriptorpool);
	wsVulkanPrint("descriptor pool creation", NONE, NONE, NONE, result);
	return result;
}

VkResult wsVulkanCreateDescriptorSets()
{
	VkDescriptorSetLayout* layouts = (VkDescriptorSetLayout*)malloc(WS_VULKAN_MAX_FRAMES_IN_FLIGHT * sizeof(VkDescriptorSetLayout));
	for(uint8_t i = 0; i < WS_VULKAN_MAX_FRAMES_IN_FLIGHT; i++)
		{ memcpy(&layouts[i], &vk->descriptorset_layout, sizeof(VkDescriptorSetLayout)); }
	
	VkDescriptorSetAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	alloc_info.descriptorPool = vk->descriptorpool;
	alloc_info.descriptorSetCount = (uint32_t)WS_VULKAN_MAX_FRAMES_IN_FLIGHT;
	alloc_info.pSetLayouts = &layouts[0];
	
	vk->descriptorsets = (VkDescriptorSet*)malloc(WS_VULKAN_MAX_FRAMES_IN_FLIGHT * sizeof(VkDescriptorSet));
	
	VkResult result = vkAllocateDescriptorSets(vk->logical_device, &alloc_info, &vk->descriptorsets[0]);
	wsVulkanPrint("descriptor set allocation", NONE, NONE, NONE, result);
	if(result != VK_SUCCESS)
		{ return result; }
	free(layouts);	// Causes crash?  Maybe!
	
	for(uint8_t i = 0; i < WS_VULKAN_MAX_FRAMES_IN_FLIGHT; i++)
	{
		VkDescriptorBufferInfo buffer_info = {};
		buffer_info.buffer = vk->uniformbuffers[i];
		buffer_info.offset = 0;
		buffer_info.range = sizeof(wsVulkanUBO);
		
		VkDescriptorImageInfo image_info = {};
		image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		image_info.imageView = vk->testRenderObject.texture.view;
		image_info.sampler = vk->texturesampler;
		
		
		VkWriteDescriptorSet descriptor_writes[2] = {};
		
		descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptor_writes[0].dstSet = vk->descriptorsets[i];
		descriptor_writes[0].dstBinding = 0;
		descriptor_writes[0].dstArrayElement = 0;
		descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptor_writes[0].descriptorCount = 1;
		descriptor_writes[0].pBufferInfo = &buffer_info;
		descriptor_writes[0].pImageInfo = NULL;			// Optional.
		descriptor_writes[0].pTexelBufferView = NULL;	// Optional.
		
		descriptor_writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptor_writes[1].dstSet = vk->descriptorsets[i];
		descriptor_writes[1].dstBinding = 1;
		descriptor_writes[1].dstArrayElement = 0;
		descriptor_writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptor_writes[1].descriptorCount = 1;
		descriptor_writes[1].pImageInfo = &image_info;
		descriptor_writes[1].pBufferInfo = NULL;			// Optional.
		descriptor_writes[1].pTexelBufferView = NULL;	// Optional.
		
		
		vkUpdateDescriptorSets(vk->logical_device, 2, &descriptor_writes[0], 0, NULL);
	}
	
	return result;
}

void wsVulkanFramebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	vk->swapchain.framebuffer_hasresized = true;
}

VkResult wsVulkanCreateFrameBuffers()
{
	
	VkResult result;
	vk->swapchain.framebuffers = malloc(vk->swapchain.num_images * sizeof(VkFramebuffer));
	
	for(int i = 0; i < vk->swapchain.num_images; i++) {
		VkImageView attachments[2] = {vk->swapchain.image_views[i], vk->depthimage_view};
		
		VkFramebufferCreateInfo framebuffer_info = {};
		framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebuffer_info.renderPass = vk->renderpass;
		framebuffer_info.attachmentCount = 2;
		framebuffer_info.pAttachments = attachments;
		framebuffer_info.width = vk->swapchain.extent.width;
		framebuffer_info.height = vk->swapchain.extent.height;
		framebuffer_info.layers = 1;
		
		result = vkCreateFramebuffer(vk->logical_device, &framebuffer_info, NULL, &vk->swapchain.framebuffers[i]);
		wsVulkanPrintQuiet("framebuffer creation", NONE, (i + 1), vk->swapchain.num_images, result);
		if(result != VK_SUCCESS)
			break;
	}
	
	printf("INFO: %i/%i Vulkan framebuffers created!\n", vk->swapchain.num_images, vk->swapchain.num_images);
	return result;
}

VkResult wsVulkanCreateRenderPass()
{
	// Specify color attachment details for render pass object.
	VkAttachmentDescription color_attachment = {};
	color_attachment.format	= vk->swapchain.image_format;
	color_attachment.samples= VK_SAMPLE_COUNT_1_BIT;
	color_attachment.loadOp	= VK_ATTACHMENT_LOAD_OP_CLEAR;
	color_attachment.storeOp= VK_ATTACHMENT_STORE_OP_STORE;
	color_attachment.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	color_attachment.stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
	color_attachment.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;
	color_attachment.finalLayout	= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	VkAttachmentReference colorattachment_reference = {};
	colorattachment_reference.attachment= 0;
	colorattachment_reference.layout	= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	
	// Everybody likes depth buffering!
	VkAttachmentDescription depth_attachment = {};
	depth_attachment.format = wsVulkanFindDepthFormat();
	depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depth_attachment.loadOp	= VK_ATTACHMENT_LOAD_OP_CLEAR;
	depth_attachment.storeOp= VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depth_attachment.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depth_attachment.stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depth_attachment.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;
	depth_attachment.finalLayout	= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	VkAttachmentReference depthattachment_reference = {};
	depthattachment_reference.attachment = 1;
	depthattachment_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	
	// Subpass time B-)
	VkSubpassDescription subpass_desc = {};
	subpass_desc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass_desc.colorAttachmentCount = 1;
	subpass_desc.pColorAttachments = &colorattachment_reference;
	subpass_desc.pDepthStencilAttachment = &depthattachment_reference;
	
	// Create render pass!
	VkRenderPassCreateInfo renderpass_info = {};
	renderpass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderpass_info.attachmentCount = 2;
	VkAttachmentDescription attachments[2] = {color_attachment, depth_attachment};
	renderpass_info.pAttachments = &attachments[0];
	renderpass_info.subpassCount= 1;
	renderpass_info.pSubpasses	= &subpass_desc;
	
	// Create render pass subpass dependency, so when we render we must wait for the finished swapchain image to render said image.
	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	renderpass_info.dependencyCount = 1;
	renderpass_info.pDependencies = &dependency;
	
	// Self-explanatory copypasta code.	
	VkResult result = vkCreateRenderPass(vk->logical_device, &renderpass_info, NULL, &vk->renderpass);
	wsVulkanPrint("render pass creation", NONE, NONE, NONE, result);
	return result;
}

VkShaderModule wsVulkanCreateShaderModule(uint8_t shaderID)
{
	// Specify shader module creation info.
	VkShaderModuleCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	
	create_info.codeSize = vk->shader.shader_size[shaderID];
	create_info.pCode = (const uint32_t*)vk->shader.shader_data[shaderID];

	// Create and return shader module!
	VkShaderModule module;
	VkResult result = vkCreateShaderModule(vk->logical_device, &create_info, NULL, &module);
	wsVulkanPrint("shader module creation", shaderID, NONE, NONE, result);
	return module;
}

VkResult wsVulkanCreateDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding layoutbinding_ubo = {};
	layoutbinding_ubo.binding = 0;
	layoutbinding_ubo.descriptorCount = 1;
	layoutbinding_ubo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	layoutbinding_ubo.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	layoutbinding_ubo.pImmutableSamplers = NULL;
	
	VkDescriptorSetLayoutBinding layoutbinding_sampler = {};
	layoutbinding_sampler.binding = 1;
	layoutbinding_sampler.descriptorCount = 1;
	layoutbinding_sampler.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	layoutbinding_sampler.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	layoutbinding_sampler.pImmutableSamplers = NULL;
	
	VkDescriptorSetLayoutBinding bindings[2] = {layoutbinding_ubo, layoutbinding_sampler};
	VkDescriptorSetLayoutCreateInfo layout_info = {};
	layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layout_info.bindingCount = 2;
	layout_info.pBindings = &bindings[0];
	
	VkResult result = vkCreateDescriptorSetLayout(vk->logical_device, &layout_info, NULL, &vk->descriptorset_layout);
	wsVulkanPrint("descriptor set layouts creation", NONE, NONE, NONE, result);
	return result;
}

VkResult wsVulkanCreateGraphicsPipeline()
{
	// ---------------------
	// CREATE SHADER MODULES
	// ---------------------

	// Shader data container inside of struct vk.
	wsShaderInit(&vk->shader);
	
	// Load shader bytecode into memory.
	uint8_t vertID = wsShaderLoad(&vk->shader, "shaders/spir-v/hellotriangle_vert.spv");
	uint8_t fragID = wsShaderLoad(&vk->shader, "shaders/spir-v/hellotriangle_frag.spv");
	
	// Convert shader bytecode into a shader module for Vulkan to work with.
	VkShaderModule vert_module = wsVulkanCreateShaderModule(vertID);
	VkShaderModule frag_module = wsVulkanCreateShaderModule(fragID);
	
	// Initialize vertex shader stage creation info.
	VkPipelineShaderStageCreateInfo vert_shaderstage_info = {};
	vert_shaderstage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vert_shaderstage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
	
	vert_shaderstage_info.pSpecializationInfo = NULL;
	vert_shaderstage_info.module = vert_module;
	vert_shaderstage_info.pName = "main";
	
	// Initialize fragment shader stage creation info.
	VkPipelineShaderStageCreateInfo frag_shaderstage_info = {};
	frag_shaderstage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	frag_shaderstage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	frag_shaderstage_info.pNext = NULL;
	frag_shaderstage_info.flags = 0;
	frag_shaderstage_info.pSpecializationInfo = NULL;
	frag_shaderstage_info.module = frag_module;
	frag_shaderstage_info.pName = "main";
	
	// NOTE: Modules destroyed at end of function.
	VkPipelineShaderStageCreateInfo shader_stages[] = {vert_shaderstage_info, frag_shaderstage_info};


	// ---------------------
	// INITIALIZE FIXED-FUNCTION VALUES
	// ---------------------
	
	// Configure vertex data receival info.
	VkPipelineVertexInputStateCreateInfo vertexinput_info = {};
	vertexinput_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	// Configure how vertices are stored and read through wsMesh.
	VkVertexInputBindingDescription* binding_desc = &vk->testMesh.binding_descs;
	VkVertexInputAttributeDescription* attribute_desc = &vk->testMesh.attribute_descs[0];
	vertexinput_info.vertexBindingDescriptionCount	= 1;
	vertexinput_info.pVertexBindingDescriptions		= binding_desc;
	vertexinput_info.vertexAttributeDescriptionCount= vk->testMesh.num_attribute_descs;
	vertexinput_info.pVertexAttributeDescriptions	= &attribute_desc[0];
	
	// Configure vertex data assembly method.
	VkPipelineInputAssemblyStateCreateInfo inputassembly_info = {};
	inputassembly_info.sType	= VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputassembly_info.topology	= VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;	// We are using triangles for our models here.
	inputassembly_info.primitiveRestartEnable = VK_FALSE;	// Allows for element buffers for splitting up geometry, reusing vertices, reusing indices, etc.
	
	// Configure dynamic states that should be allowed to change without reconstructing the whole pipeline.  No performance impact.
	VkDynamicState dynamic_states[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
	VkPipelineDynamicStateCreateInfo dynamicstate_info = {};
	dynamicstate_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicstate_info.dynamicStateCount	= 2;
	dynamicstate_info.pDynamicStates	= dynamic_states;
	
	// Configure viewport info.
	VkPipelineViewportStateCreateInfo viewport_info = {};
	viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewport_info.viewportCount	= 1;
	viewport_info.scissorCount	= 1;
	
	// Configure rasterizer info.
	VkPipelineRasterizationStateCreateInfo rasterizer_info = {};
	rasterizer_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer_info.depthClampEnable = VK_FALSE;		// Instead of discarding OOB fragments, should we clamp them?
	rasterizer_info.rasterizerDiscardEnable = VK_FALSE;	// If this is true, geometry will never be passed through the rasterizing stage.
	rasterizer_info.polygonMode = VK_POLYGON_MODE_FILL;	// Can also be VK_POLYGON_MODE_LINE, or VK_POLYGON_MODE_POINT.
	rasterizer_info.lineWidth = 1.0f;						// Enabling the wideLines GPU feature is required for any value above 1.0f.
	rasterizer_info.cullMode = VK_CULL_MODE_BACK_BIT;	// Back-face culling, baybey!
	rasterizer_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;// Specifies vertex order for faced to be considered front-facing.
	/* Controls whether rasterizer should modify depth values of a fragment by a constant, based on the fragment's slope, or not at all.  
		Allegedly, this is sometimes used for shadow mapping. */
	rasterizer_info.depthBiasEnable			= VK_FALSE;
	rasterizer_info.depthBiasConstantFactor	= 0.0f;
	rasterizer_info.depthBiasClamp			= 0.0f;
	rasterizer_info.depthBiasSlopeFactor	= 0.0f;
	
	// Multisampling!  Great for reducing aliasing artifacting along edges of polygons where more than one poly may occupy a single pixel.
	VkPipelineMultisampleStateCreateInfo multisampling_info = {};
	multisampling_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling_info.sampleShadingEnable	= VK_FALSE;
	multisampling_info.rasterizationSamples	= VK_SAMPLE_COUNT_1_BIT;
	multisampling_info.minSampleShading		= 1.0f;
	multisampling_info.pSampleMask = NULL;
	multisampling_info.alphaToCoverageEnable = VK_FALSE;
	multisampling_info.alphaToOneEnable = VK_FALSE;
	
	// Configure color blending attachment state for the fragment shader.
	VkPipelineColorBlendAttachmentState colorblend_attachment = {};
	colorblend_attachment.colorWriteMask= VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorblend_attachment.blendEnable	= VK_TRUE;
	colorblend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorblend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorblend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorblend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorblend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorblend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;
	
	// For bitwise color blending.  Automagically disables the first color-blending stage above.
	VkPipelineColorBlendStateCreateInfo colorblend_info = {};
	colorblend_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorblend_info.logicOpEnable = VK_FALSE;
	colorblend_info.logicOp = VK_LOGIC_OP_COPY;
	colorblend_info.attachmentCount = 1;
	colorblend_info.pAttachments = &colorblend_attachment;
	colorblend_info.blendConstants[0] = 0.0f;
	colorblend_info.blendConstants[1] = 0.0f;
	colorblend_info.blendConstants[2] = 0.0f;
	colorblend_info.blendConstants[3] = 0.0f;
	
	VkPipelineDepthStencilStateCreateInfo depthstencil_info = {};
	depthstencil_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthstencil_info.depthTestEnable = VK_TRUE;
	depthstencil_info.depthWriteEnable = VK_TRUE;
	depthstencil_info.depthCompareOp = VK_COMPARE_OP_LESS;
	depthstencil_info.depthBoundsTestEnable = VK_FALSE;
	depthstencil_info.minDepthBounds = 0.0f;
	depthstencil_info.maxDepthBounds = 1.0f;
	depthstencil_info.stencilTestEnable = VK_FALSE;
	// depthstencil_info.front = {};
	// depthstencil_info.back = {};
	
	
	// ---------------------
	// CREATE PIPELINE & LAYOUT.
	// ---------------------
	
	VkPipelineLayoutCreateInfo pipelinelayout_info = {};
	pipelinelayout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelinelayout_info.setLayoutCount = 1;
	pipelinelayout_info.pSetLayouts	= &vk->descriptorset_layout;
	pipelinelayout_info.pushConstantRangeCount = 0;
	pipelinelayout_info.pPushConstantRanges	= NULL;
	VkResult result = vkCreatePipelineLayout(vk->logical_device, &pipelinelayout_info, NULL, &vk->pipeline_layout);
	wsVulkanPrint("pipeline layout creation", NONE, NONE, NONE, result);
	
	VkGraphicsPipelineCreateInfo pipeline_info = {};
	pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_info.stageCount = 2;
	pipeline_info.pStages = &shader_stages[0];
	pipeline_info.pVertexInputState = &vertexinput_info;
	pipeline_info.pInputAssemblyState = &inputassembly_info;
	pipeline_info.pViewportState = &viewport_info;
	pipeline_info.pRasterizationState = &rasterizer_info;
	pipeline_info.pMultisampleState = &multisampling_info;
	pipeline_info.pDepthStencilState = NULL;
	pipeline_info.pColorBlendState = &colorblend_info;
	pipeline_info.pDynamicState = &dynamicstate_info;
	pipeline_info.pDepthStencilState = &depthstencil_info;
	pipeline_info.layout = vk->pipeline_layout;
	pipeline_info.renderPass = vk->renderpass;
	pipeline_info.subpass = 0;
	// Specify if we should "derive" from a parent pipeline for creation.
	pipeline_info.basePipelineHandle = NULL;
	pipeline_info.basePipelineIndex = -1;
	result = vkCreateGraphicsPipelines(vk->logical_device, VK_NULL_HANDLE, 1, &pipeline_info, NULL, &vk->pipeline);
	
	// Destroy shader modules cause WE DON NEED EM NO MO
	vkDestroyShaderModule(vk->logical_device, vert_module, NULL);
	vkDestroyShaderModule(vk->logical_device, frag_module, NULL);
	
	wsVulkanPrint("graphics pipeline creation", NONE, NONE, NONE, result);
	return result;
}

// Returns number of image views created successfully.
uint32_t wsVulkanCreateImageViews()
{
	uint32_t num_created = 0;

	// Allocate space for our image views within struct vk.
	vk->swapchain.image_views = malloc(vk->swapchain.num_images * sizeof(VkImageView));

	// Create image view for each swap chain image.
	for(uint32_t i = 0; i < vk->swapchain.num_images; i++)
	{
		VkResult result = wsVulkanCreateImageView(&vk->swapchain.images[i], &vk->swapchain.image_views[i], vk->swapchain.image_format, VK_IMAGE_ASPECT_COLOR_BIT);
		if(result == VK_SUCCESS)
			num_created++;
	}
	
	if(num_created != vk->swapchain.num_images)
		{ printf("ERROR: Only %i/%i image views created!\n", num_created, vk->swapchain.num_images); }
	else 
		{ printf("INFO: %i/%i Vulkan image views created!\n", num_created, vk->swapchain.num_images); }
	
	return num_created;
}
VkResult wsVulkanCreateSwapChain()
{
	// Initialize swap chain within struct vk.
	wsVulkanQuerySwapChainSupport();
	wsVulkanChooseSwapSurfaceFormat(&vk->swapchain);
	wsVulkanChooseSwapPresentMode(&vk->swapchain);
	wsVulkanChooseSwapExtent();

	// Recommended to add at least 1 extra image to swap chain buffer to reduce wait times during rendering.
	uint32_t num_images = vk->swapchain.capabilities.minImageCount + 2;
	vk->swapchain.num_images = num_images;

	// Specify swap chain creation info.
	VkSwapchainCreateInfoKHR create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	create_info.surface = vk->surface;
	create_info.minImageCount = num_images;
	create_info.imageFormat = vk->swapchain.surface_format.format;
	create_info.imageColorSpace = vk->swapchain.surface_format.colorSpace;
	create_info.imageExtent = vk->swapchain.extent;
	create_info.imageArrayLayers = 1;	// Always 1 unless this is a stereoscopic 3D application.
	create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	// Check if queue family indices are unique.
	if(vk->queues.num_unique_queue_families > 1)
	{
		create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		create_info.queueFamilyIndexCount = vk->queues.num_unique_queue_families;
		create_info.pQueueFamilyIndices = &vk->queues.unique_queue_family_indices[0];
	}
	else
	{
		create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		create_info.queueFamilyIndexCount = 0;	// Optional.
		create_info.pQueueFamilyIndices = NULL;	// Optional.
	}
	create_info.preTransform = vk->swapchain.capabilities.currentTransform;	// Do we want to apply any transformations to our swap chain content?  Nope!
	create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;	// Do we want to blend our program into other windows in the window system?  No siree!
	create_info.presentMode = vk->swapchain.present_mode;
	create_info.clipped = VK_TRUE;	// If another window obscures some pixels from our window, should we ignore drawing them?  Yeah, probably.
	create_info.oldSwapchain = VK_NULL_HANDLE;	// Reference to old swap chain in case we ever have to create a new one!  NULL for now.
	
	// Create swap chain!
	VkResult result = vkCreateSwapchainKHR(vk->logical_device, &create_info, NULL, &vk->swapchain.sc);
	
	// Store swap chain images in struct vk.
	vkGetSwapchainImagesKHR(vk->logical_device, vk->swapchain.sc, &num_images, NULL);
	vk->swapchain.images = malloc(num_images * sizeof(VkImage));
	vkGetSwapchainImagesKHR(vk->logical_device, vk->swapchain.sc, &num_images, vk->swapchain.images);
	
	// Set current swap chain image format for easier access.
	vk->swapchain.image_format = vk->swapchain.surface_format.format;

	printf("INFO: Creating swap chain with properties: \n\tExtent: %ix%i\n\tSurface format: %i\n\tPresentation mode: %i\n", 
		vk->swapchain.extent.width, vk->swapchain.extent.height, vk->swapchain.surface_format.format, vk->swapchain.present_mode);
	
	wsVulkanPrint("swap chain creation", NONE, NONE, NONE, result);
	return result;
}

void wsVulkanRecreateSwapChain()
{
	// If window is minimized, pause events and wait for something to draw to again!
	int32_t width = 0, height = 0;
	glfwGetFramebufferSize(wsWindowGetPtr(vk->windowID), &width, &height);
	while(width == 0 || height == 0)
	{
		glfwGetFramebufferSize(wsWindowGetPtr(vk->windowID), &width, &height);
		glfwWaitEvents();
	}
	
	// Wait until we can safely destroy and recreate resources.
	vkDeviceWaitIdle(vk->logical_device);
	
	// Destroy & recreate the swap chain and its derivative components.
	wsVulkanDestroySwapChain();
	wsVulkanCreateSwapChain();
	wsVulkanCreateImageViews();
	wsVulkanCreateDepthResources();
	wsVulkanCreateFrameBuffers();
}

void wsVulkanDestroySwapChain()
{
	// Destroy individual framebuffers within swapchain.
	for(uint32_t i = 0; i < vk->swapchain.num_images; i++)
		{ vkDestroyFramebuffer(vk->logical_device, vk->swapchain.framebuffers[i], NULL); }
	free(vk->swapchain.framebuffers);
	printf("INFO: Vulkan framebuffers destroyed!\n");
	
	// Destroy depth buffer.
	vkDestroyImageView(vk->logical_device, vk->depthimage_view, NULL);
	vkDestroyImage(vk->logical_device, vk->depthimage, NULL);
	vkFreeMemory(vk->logical_device, vk->depthimage_memory, NULL);
	printf("INFO: Vulkan depth buffer destroyed!\n");
	
	// Destroy swap chain image views.
	for(uint32_t i = 0; i < vk->swapchain.num_images; i++)
		{ vkDestroyImageView(vk->logical_device, vk->swapchain.image_views[i], NULL); }
	free(vk->swapchain.image_views);
	printf("INFO: Vulkan image views destroyed!\n");

	// Destroy swap chain.
	vkDestroySwapchainKHR(vk->logical_device, vk->swapchain.sc, NULL);
	free(vk->swapchain.formats);
	free(vk->swapchain.present_modes);
	free(vk->swapchain.images);
	printf("INFO: Vulkan swap chain destroyed!\n");
}

// Choose a nice resolution to draw swap chain images at.
void wsVulkanChooseSwapExtent()
{
	wsVulkanSwapChain* swapchain_info = &vk->swapchain;
	VkSurfaceCapabilitiesKHR* capabilities = &vk->swapchain.capabilities;
	
	if(capabilities->currentExtent.width != UINT32_MAX)
	{
		swapchain_info->extent = capabilities->currentExtent;
	}
	else
	{
		// Query GLFW Window framebuffer size.
		int width, height;
		glfwGetFramebufferSize(wsWindowGetPtr(vk->windowID), &width, &height);
		
		// Set extent width.
		swapchain_info->extent.width = clamp((uint32_t)width, capabilities->minImageExtent.width, capabilities->maxImageExtent.width);
		swapchain_info->extent.height = clamp((uint32_t)height, capabilities->minImageExtent.height, capabilities->maxImageExtent.height);
	}
}

// Choose a presentation mode for our swap chain drawing surface.
void wsVulkanChooseSwapPresentMode(wsVulkanSwapChain* swapchain_info)
{
	// If we find a presentation mode that works for us, select it and return.
	for(int32_t i = 0; i < swapchain_info->num_present_modes; i++)
	{
		VkPresentModeKHR* current_mode = &swapchain_info->present_modes[i];
		
		if(*current_mode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			swapchain_info->present_mode = VK_PRESENT_MODE_MAILBOX_KHR;
			return;
		}
	}

	// If we don't find one that matches our request, pick the only guaranteed presentation mode.
	swapchain_info->present_mode = VK_PRESENT_MODE_FIFO_KHR;
}

// Choose a format and color space for our swap chain drawing surface.
void wsVulkanChooseSwapSurfaceFormat(wsVulkanSwapChain* swapchain_info)
{
	// If we find a format that works for us, select it and return.
	for(int32_t i = 0; i < swapchain_info->num_formats; i++)
	{
		VkSurfaceFormatKHR* current_format = &swapchain_info->formats[i];
		
		if(current_format->format == VK_FORMAT_B8G8R8A8_SRGB && current_format->colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			swapchain_info->surface_format = *current_format;
			return;
		}
	}

	// If we don't find one that matches our request, pick the first available format.
	swapchain_info->surface_format = swapchain_info->formats[0];
}

void wsVulkanQuerySwapChainSupport()
{
	// Query surface details into vk->swapchain_deets.
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk->physical_device, vk->surface, &vk->swapchain.capabilities);

	// Query supported surface formats.
	uint32_t num_formats;
	vkGetPhysicalDeviceSurfaceFormatsKHR(vk->physical_device, vk->surface, &num_formats, NULL);
	vk->swapchain.num_formats = num_formats;

	if(num_formats != 0)
	{
		vk->swapchain.formats = malloc(num_formats * sizeof(VkSurfaceFormatKHR));
		vkGetPhysicalDeviceSurfaceFormatsKHR(vk->physical_device, vk->surface, &num_formats, vk->swapchain.formats);
	}

	// Query supported presentation modes.
	uint32_t num_present_modes;
	
	vkGetPhysicalDeviceSurfacePresentModesKHR(vk->physical_device, vk->surface, &num_present_modes, NULL);
	vk->swapchain.num_present_modes = num_present_modes;

	if(num_present_modes != 0)
	{
		vk->swapchain.present_modes = malloc(num_present_modes * sizeof(VkPresentModeKHR));
		vkGetPhysicalDeviceSurfacePresentModesKHR(vk->physical_device, vk->surface, &num_present_modes, vk->swapchain.present_modes);
	}
}

// Creates a surface bound to our GLFW window.
VkResult wsVulkanCreateSurface()
{
	VkResult result = glfwCreateWindowSurface(vk->instance, wsWindowGetPtr(vk->windowID), NULL, &vk->surface);
	wsVulkanPrint("surface creation for window", vk->windowID, NONE, NONE, result);
	return result;
}

// Creates a logical device to interface with the physical one.
VkResult wsVulkanCreateLogicalDevice(uint32_t num_validation_layers, const char* const* validation_layers)
{
	// Get required queue family indices and store them in vk.
	wsVulkanFindQueueFamilies(&vk->queues, &vk->physical_device, &vk->surface);
	
	// Store queue family creation infos for later use in binding to logical device creation info.
	uint32_t num_unique_queue_families = vk->queues.num_unique_queue_families;
	VkDeviceQueueCreateInfo* queue_create_infos = calloc(num_unique_queue_families, sizeof(VkDeviceQueueCreateInfo));
	
	// Specify creation info for queue families.
	for(uint8_t i = 0; i < num_unique_queue_families; i++)
	{
		queue_create_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_create_infos[i].queueFamilyIndex = vk->queues.unique_queue_family_indices[i];
		queue_create_infos[i].queueCount = 1;
		
		// Specify a queue priority from 0.0f-1.0f.  Required regardless of number of queues; influences scheduling of command buffer execution.
		float queue_priority = 1.0f;
		queue_create_infos[i].pQueuePriorities = &queue_priority;
	}
	
	// Used to specify device features that will be used.
	VkPhysicalDeviceFeatures device_features = {};
	vkGetPhysicalDeviceFeatures(vk->physical_device, &device_features);
	device_features.samplerAnisotropy = device_features.samplerAnisotropy;	// Request sampler anisotropy feature be enabled for texture sampling later on.
	
	// Specify creation info for logical_gpuVK.
	VkDeviceCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	create_info.queueCreateInfoCount = num_unique_queue_families;
	create_info.pQueueCreateInfos = queue_create_infos;
	create_info.pEnabledFeatures = &device_features;
	create_info.pNext = NULL;
	
	// Modern way of handling device-specific validation layers.
	const char* device_extensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	create_info.enabledExtensionCount = 1;
	create_info.ppEnabledExtensionNames = device_extensions;
	
	// Device-specific validation layers are deprecated for modern API versions, but required for older versions.
	if(debug)
	{
		create_info.enabledLayerCount = (uint32_t)num_validation_layers;
		create_info.ppEnabledLayerNames = validation_layers;
	}
	else
		{ create_info.enabledLayerCount = 0; }
	
	
	// Create logical_device and return result!
	VkResult result = vkCreateDevice(vk->physical_device, &create_info, NULL, &vk->logical_device);
	
	// Assign queue handles from logical_device.
	vkGetDeviceQueue(vk->logical_device, vk->queues.ndx_graphics_family, 0, &vk->queues.graphics_queue);
	vkGetDeviceQueue(vk->logical_device, vk->queues.ndx_transfer_family, 0, &vk->queues.transfer_queue);
	vkGetDeviceQueue(vk->logical_device, vk->queues.ndx_present_family, 0, &vk->queues.present_queue);
	
	printf("INFO: %i unique Vulkan queue families exist; indices are as specified:\n", num_unique_queue_families);
	printf("\tGraphics: %i\n\tTransfer: %i\n\tPresentation: %i\n", vk->queues.ndx_graphics_family, vk->queues.ndx_transfer_family, vk->queues.ndx_present_family);
	
	// FREE MEMORY!!!
	free(queue_create_infos);
	
	wsVulkanPrint("logical device creation", NONE, NONE, NONE, result);
	return result;
}

// Checks if all queue families have been found.
bool wsVulkanHasFoundAllQueueFamilies(wsVulkanQueueFamilies* indices)
{
	if(!indices->has_graphics_family)
		return false;
	if(!indices->has_transfer_family)
		return false;
	if(!indices->has_present_family)
		return false;
	
	return true;
}

uint8_t wsVulkanCountUniqueQueueIndices()
{
	uint8_t num_queue_families = 0;
	
	for(uint8_t i = 0; i < 3; i++)
	{
		if((vk->queues.ndx_graphics_family == i) || (vk->queues.ndx_present_family == i) || (vk->queues.ndx_transfer_family == i))
		{
			num_queue_families++;
			continue;
		}
	}
	
	vk->queues.num_unique_queue_families = num_queue_families;
	
	return num_queue_families;
}

// Make sure we have a list of all unique queue families stored for future buffer use!
void wsVulkanPopulateUniqueQueueFamiliesArray()
{
	vk->queues.unique_queue_family_indices = malloc(vk->queues.num_unique_queue_families * sizeof(uint32_t));
	if(vk->queues.has_graphics_family)	// If we have a graphics family, we likely support presentation, and we ALWAYS implicitly support transfer.
		vk->queues.unique_queue_family_indices[0] = vk->queues.ndx_graphics_family;
	if(vk->queues.ndx_graphics_family != vk->queues.ndx_transfer_family)
		vk->queues.unique_queue_family_indices[1] = vk->queues.ndx_transfer_family;
	if(vk->queues.ndx_graphics_family != vk->queues.ndx_present_family)
		vk->queues.unique_queue_family_indices[2] = vk->queues.ndx_present_family;
	
	/* printf("%i\n", wsVulkanGetUniqueQueueIndicesCount());
	for(uint8_t i = 0; i < wsVulkanGetUniqueQueueIndicesCount(); i++)
		{ printf("%i ", vk->queues.unique_queue_family_indices[i]); }
	printf("\n"); */
}

uint32_t wsVulkanFindQueueFamilies(wsVulkanQueueFamilies *indices, VkPhysicalDevice* physical_device, VkSurfaceKHR* surface)
{
	// Get list of queue families available to us.
	uint32_t num_queue_families = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(*physical_device, &num_queue_families, NULL);
	printf("INFO: Your device supports %i queue families!\n", num_queue_families);
	
	VkQueueFamilyProperties* queue_families = malloc(num_queue_families * sizeof(VkQueueFamilyProperties));
	vkGetPhysicalDeviceQueueFamilyProperties(*physical_device, &num_queue_families, queue_families);
	
	// These will end up as true once a family is found.
	indices->has_graphics_family = false;
	indices->has_transfer_family = false;
	indices->has_present_family = false;
	
	for(uint32_t i = 0; i < num_queue_families; i++)
	{
		VkQueueFamilyProperties queue_family = queue_families[i];
		
		if((queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) && !indices->has_graphics_family)
		{
			indices->ndx_graphics_family = i;
			indices->has_graphics_family = true;
			continue;
		}
		if((queue_family.queueFlags & VK_QUEUE_TRANSFER_BIT) && !indices->has_transfer_family)
		{
			indices->ndx_transfer_family = i;
			indices->has_transfer_family = true;
			continue;
		}
		
		VkBool32 has_present_support = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(*physical_device, i, *surface, &has_present_support);
		if(has_present_support && !indices->has_present_family)
		{
			indices->ndx_present_family = i;
			indices->has_present_family = true;
			continue;
		}
		
		if(wsVulkanHasFoundAllQueueFamilies(indices))
		{
			wsVulkanCountUniqueQueueIndices();
			wsVulkanPopulateUniqueQueueFamiliesArray();
			return num_queue_families;
		}
	}
	
	if(!indices->has_transfer_family)
	{
		indices->ndx_transfer_family = indices->ndx_graphics_family;
		indices->has_transfer_family = true;
	}
	if(!indices->has_present_family)
	{
		indices->ndx_present_family = indices->ndx_graphics_family;
		indices->has_present_family = true;
	}
	
	wsVulkanCountUniqueQueueIndices();
	wsVulkanPopulateUniqueQueueFamiliesArray();
	
	return num_queue_families;
}

bool wsVulkanPickPhysicalDevice()
{
	// Get list of physical devices and store in GPUs[].
	uint32_t num_GPUs = 0;
	vkEnumeratePhysicalDevices(vk->instance, &num_GPUs, NULL);
	
	if(num_GPUs <= 0)
	{
		printf("WARNING: Vulkan alleges your system has no GPUs installed...  How are you even seeing this...\n");
		return false;
	}
	
	VkPhysicalDevice* GPUs = malloc(num_GPUs * sizeof(VkPhysicalDevice));
	vkEnumeratePhysicalDevices(vk->instance, &num_GPUs, GPUs);
	
	// Pick GPU with highest score.
	int32_t max_score = -1;
	int32_t ndx_GPU = -1;
	
	printf("INFO: Rating %i GPU(s)...\n", num_GPUs);
	
	for(int32_t i = 0; i < num_GPUs; i++)
	{
		printf("GPU %i: \t", i);

		// Make sure we are checking current GPU's suitability;
		vk->physical_device = GPUs[i];
		int32_t current_score = wsVulkanRatePhysicalDevice(&(GPUs[i]));

		if(current_score > max_score)
		{
			max_score = current_score;
			ndx_GPU = i;
		}

		printf("\tGPU %i scored %i!\n", i, current_score);
	}
	
	// If the max GPU score is not able to function with the program, then we've got a problemo
	if(max_score != -1)
	{
		vk->physical_device = GPUs[ndx_GPU];
		
		// TODO: Make this list the properties of chosen GPU.
		VkPhysicalDeviceProperties device_properties;
		vkGetPhysicalDeviceProperties(vk->physical_device, &device_properties);
		printf("INFO: GPU %i: I CHOOSE YOU!!!  Details: \n", ndx_GPU);
		printf("\tDevice name: %s\n", device_properties.deviceName);
		printf("\tDevice type: %i\n", device_properties.deviceType);
		printf("\tDriver version: %i\n", device_properties.driverVersion);
		printf("\tVendor ID: %i\n", device_properties.vendorID);

		return true;
	}
	else
	{
		printf("ERROR: Failed to find GPUs with proper Vulkan support!\n");
		return false;
	}
	
	// Don't free GPU list so that we can allow user to swap GPUs in settings later.
}

int32_t wsVulkanRatePhysicalDevice(VkPhysicalDevice* physical_device)
{
	int32_t score = 0;
	
	// Queue for device properties.
	VkPhysicalDeviceProperties device_properties;
	vkGetPhysicalDeviceProperties(*physical_device, &device_properties);
	
	// Queue for device features.
	VkPhysicalDeviceFeatures device_features;
	vkGetPhysicalDeviceFeatures(*physical_device, &device_features);

	// Program cannot function without geometry shaders!
	if(!device_features.geometryShader)
		return NO_GEOMETRY_SHADER;
	// This is unfortunate if we cannot support anisotropy (as well as highly unlikely), but not the end of the world.
	if(!device_features.samplerAnisotropy)
		score -= 500;
	
	// Program cannot function without certain queue families.
	wsVulkanQueueFamilies indices;
	uint32_t num_queue_families = wsVulkanFindQueueFamilies(&indices, physical_device, &vk->surface);
	printf("Found %i queue families on your device!\n", num_queue_families);
	
	if(!indices.has_graphics_family)
		return NO_GRAPHICS_FAMILY;
	if(!indices.has_transfer_family)
		return NO_TRANSFER_FAMILY;
	if(!indices.has_present_family)
		return NO_PRESENTATION_FAMILY;
	
	// Program cannot function without certain device-specific extensions.
	if(!wsVulkanCheckDeviceExtensionSupport(physical_device))
		return NO_DEVICE_EXTENSION_SUPPORT;
	
	// Program cannot function without proper swapchain support!
	wsVulkanQuerySwapChainSupport();
	if(vk->swapchain.num_formats <= 0 || vk->swapchain.num_present_modes <= 0)
		return NO_SWAPCHAIN_SUPPORT;
	
	// If these are the same queue family, this could increase performance (unlikely to execute in parallel).
	score += indices.ndx_graphics_family == indices.ndx_present_family ? 1000 : 0;
	// If these are separate queue families, this WILL increase performance (likely to execute in parallal).
	score += indices.ndx_graphics_family != indices.ndx_transfer_family ? 2000 : 0;
	
	// Prefer discrete GPU.  If the max texture size is much lower on the discrete card, it might be too old and suck.
	score += device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? 1000 : 0;
	score += device_properties.limits.maxImageDimension2D;
	
	return score;
}

bool wsVulkanCheckDeviceExtensionSupport(VkPhysicalDevice* physical_device)
{
	// Query number of extensions available and their names.
	uint32_t num_available_extensions;
	vkEnumerateDeviceExtensionProperties(*physical_device, NULL, &num_available_extensions, NULL);
	
	VkExtensionProperties* available_extensions = malloc(num_available_extensions * sizeof(VkExtensionProperties));
	vkEnumerateDeviceExtensionProperties(*physical_device, NULL, &num_available_extensions, available_extensions);

	// Required device-specific extensions to verify we have access to.
	uint32_t num_required_extensions = 1;
	const char* required_extensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

	printf("\t%i device-specific Vulkan extensions required: ", num_required_extensions);
	for(int32_t i = 0; i < num_required_extensions; i++)
		{ printf("\t%s", required_extensions[i]); }
	printf("\n");

	// Search for each required extension.
	for(uint32_t i = 0; i < num_required_extensions; i++)
	{
		bool has_found_extension = false;
		
		for(uint32_t j = 0; j < num_available_extensions; j++)
		{
			if(strcmp(required_extensions[i], available_extensions[j].extensionName) == 0)
			{
				has_found_extension = true;
				break;
			}
		}

		if(!has_found_extension)
		{
			printf("\tERROR: Not all required device-specific Vulkan extension(s) are supported by your GPU!\n");
			return false;
		}
	}

	printf("\tAll required device-specific Vulkan extensions are supported by your GPU!\n");

	return true;
}

VkResult wsVulkanInitDebugMessenger()
{
	// Populate creation info for debug messenger.
	VkDebugUtilsMessengerCreateInfoEXT create_info = {};
	wsVulkanPopulateDebugMessengerCreationInfo(&create_info);
	
	// Create debug messenger!
	VkResult result = wsVulkanCreateDebugUtilsMessengerEXT(vk->instance, &create_info, NULL, &vk->debug_messenger);
	wsVulkanPrint("debug messenger creation", NONE, NONE, NONE, result);
	
	return result;
}

void wsVulkanStopDebugMessenger()
{
	wsVulkanDestroyDebugUtilsMessengerEXT(vk->instance, vk->debug_messenger, NULL);
	printf("INFO: Vulkan debug messenger destroyed!\n");
}

// Vulkan proxy function; Create debug messenger.
VkResult wsVulkanCreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* create_info, const VkAllocationCallbacks* allocator, VkDebugUtilsMessengerEXT* debug_messenger)
{
	PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != NULL)
	{
        return func(instance, create_info, allocator, debug_messenger);
    }
	else
	{
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

// Vulkan proxy function; Destroy debug messenger.
void wsVulkanDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debug_messenger, const VkAllocationCallbacks* allocator)
{
    PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != NULL)
	{
        func(instance, debug_messenger, allocator);
    }
}

// Vulkan proxy function; Callback for Vulkan debug messages.
static VKAPI_ATTR VkBool32 VKAPI_CALL wsVulkanDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT msg_severity, VkDebugUtilsMessageTypeFlagsEXT msg_type, const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data)
{
	if(msg_severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
		printf("ERROR: ");
	else if(msg_severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		printf("WARNING: ");
	else if(msg_severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
		printf("INFO: ");
	
	printf("%s\n", callback_data->pMessage);
	
	return VK_FALSE;
}

// Populate debug messenger creation info; used mainly for debugging vkCreateInstance().
void wsVulkanPopulateDebugMessengerCreationInfo(VkDebugUtilsMessengerCreateInfoEXT* create_info)
{
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
}

// Adds debug extensions to required extension list.
void wsVulkanAddDebugExtensions(const char*** extensions, uint32_t* num_extensions)
{
	// Copy given extension list.
	char** debug_extensions = malloc((*num_extensions + 1) * sizeof(char*));
	for(int32_t i = 0; i < *num_extensions; i++)
		{ debug_extensions[i] = (char*)(*extensions)[i]; }
	
	// Append Debug utility extension to list.
	debug_extensions[*num_extensions] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
	
	// FREE MEMORY!!!  In retrospect: This crashes...
	// free(extensions);
	
	// Increment original number of extensions and return new extension list.
	(*num_extensions)++;
	*extensions = (const char**)debug_extensions;
}

bool wsVulkanEnableRequiredExtensions(VkInstanceCreateInfo* create_info)
{
	// Get list of required Vulkan extensions from GLFW.
	uint32_t num_required_extensions = 0;
	const char** required_extensions = glfwGetRequiredInstanceExtensions(&num_required_extensions);
	if(debug)
		{ wsVulkanAddDebugExtensions(&required_extensions, &num_required_extensions); }
	
	// Set correct fields in create_info.  Cannot create instance without this.
	create_info->flags = 0;
	create_info->enabledExtensionCount = num_required_extensions;
	create_info->ppEnabledExtensionNames = required_extensions;
	
	// List all required extensions.
	printf("INFO: %i Vulkan extension(s) required by GLFW: \n", num_required_extensions);
	for(int32_t i = 0; i < num_required_extensions; i++)
		{ printf("\t%s\n", required_extensions[i]); }
	
	// Fetch required extensions & required extension count.
	uint32_t num_available_extensions = 0;
	vkEnumerateInstanceExtensionProperties(NULL, &num_available_extensions, NULL);
	
	VkExtensionProperties* available_extensions = malloc(num_available_extensions * sizeof(VkExtensionProperties));
	vkEnumerateInstanceExtensionProperties(NULL, &num_available_extensions, available_extensions);
	
	// Check that required extensions are supported.
	bool has_all_extensions = true;
	for(int32_t i = 0; i < num_required_extensions; i++)
	{
		bool extension_found = false;
		
		for(int32_t j = 0; j < num_available_extensions; j++)
		{
			if(strcmp(required_extensions[i], available_extensions[j].extensionName) == 0)
			{
				extension_found = true;
				break;
			}
		}
		
		if(!extension_found)
		{
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
	
	if(!has_all_extensions)
		{ printf("\tERROR: Not all GLFW-required Vulkan extensions are supported!\n"); }
	else
		{ printf("\tAll GLFW-required Vulkan extensions are supported!\n"); }
	
	return has_all_extensions;
}

bool wsVulkanEnableValidationLayers(VkInstanceCreateInfo* create_info)
{
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
	
	// Search for each required layer in our list available_layers.
	for(int32_t i = 0; i < num_required_layers; i++)
	{
		bool layer_found = false;
		
		for(int32_t j = 0; j < num_available_layers; j++)
		{
			if(strcmp(required_layers[i], available_layers[j].layerName) == 0)
			{
				layer_found = true;
				break;
			}
		}
		
		if(!layer_found)
		{
			printf("\tERROR: required Vulkan validation layers NOT supported!\n");
			return false;
		}
	}
	
	// If we have all required layers available for use.
	create_info->enabledLayerCount = num_required_layers;
	create_info->ppEnabledLayerNames = (const char**)required_layers;
	
	printf("INFO: %u Vulkan validation layer(s) required: \n", (unsigned int)num_required_layers);
	
	for(int32_t i = 0; i < num_required_layers; i++)
		{ printf("\t%s\n", required_layers[i]); }
	
	// FREE MEMORY!!!
	free(available_layers);
	// I believe this sometimes causes validation layers to cry (crash).
	// free(required_layers);
	
	printf("\tRequired Vulkan validation layers are supported!\n");
	return true;
}

// Print statements for things that return a VkResult.
void wsVulkanPrint(const char* str, int32_t ID, int32_t numerator, int32_t denominator, VkResult result)
{
	// If we have neither an ID nor fraction.
	if(ID == NONE && numerator == NONE)
	{
		if(result != VK_SUCCESS)
			printf("ERROR: Vulkan %s failed with result code %i!\n", str, result);
		else printf("INFO: Vulkan %s success!\n", str);
	}
	// If we have no ID but do have a fraction.
	else if(ID == NONE && numerator != NONE)
	{
		if(result != VK_SUCCESS)
			printf("ERROR: Vulkan %s %i/%i failed with result code %i!\n", str, numerator, denominator, result);
		else printf("INFO: Vulkan %s %i/%i success!\n", str, numerator, denominator);
	}
	// If we have an ID but no fraction.
	else if(ID != NONE && numerator == NONE)
	{
		if(result != VK_SUCCESS)
			printf("ERROR: Vulkan %s w/ ID %i failed with result code %i!\n", str, ID, result);
		else printf("INFO: Vulkan %s w/ ID %i success!\n", str, ID);
	}
	// If we have both an ID & fraction.
	else if(ID != NONE && numerator != NONE)
	{
		if(result != VK_SUCCESS)
			printf("ERROR: Vulkan %s %i/%i w/ ID %i failed with result code %i!\n", str, numerator, denominator, ID, result);
		else printf("INFO: Vulkan %s %i/%i w/ ID %i success!\n", str, numerator, denominator, ID);
	}
}
void wsVulkanPrintQuiet(const char* str, int32_t ID, int32_t numerator, int32_t denominator, VkResult result)
{
	// If we have neither an ID nor fraction.
	if(ID == NONE && numerator == NONE)
	{
		if(result != VK_SUCCESS)
			printf("ERROR: Vulkan %s failed with result code %i!\n", str, result);
	}
	// If we have no ID but do have a fraction.
	else if(ID == NONE && numerator != NONE)
	{
		if(result != VK_SUCCESS)
			printf("ERROR: Vulkan %s %i/%i failed with result code %i!\n", str, numerator, denominator, result);
	}
	// If we have an ID but no fraction.
	else if(ID != NONE && numerator == NONE)
	{
		if(result != VK_SUCCESS)
			printf("ERROR: Vulkan %s w/ ID %i failed with result code %i!\n", str, ID, result);
	}
	// If we have both an ID & fraction.
	else if(ID != NONE && numerator != NONE)
	{
		if(result != VK_SUCCESS)
			printf("ERROR: Vulkan %s %i/%i w/ ID %i failed with result code %i!\n", str, numerator, denominator, ID, result);
	}
}

float wsVulkanGetAspectRatio()
	{ return (vk->swapchain.extent.width / (float)vk->swapchain.extent.height); }