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
#include"h/mafs.h"


enum WS_VK_ID_STATE				{NONE = INT32_MAX};
enum WS_VK_COMMAND_TYPE			{COMMAND_GRAPHICS, COMMAND_TRANSFER, COMMAND_QUEUE_OWNERSHIP_TRANSFER, COMMAND_ANY};
enum GPU_INCOMPATIBILITY_CODES
{
	NO_GEOMETRY_SHADER = -100, 
	NO_GRAPHICS_FAMILY, 
	NO_TRANSFER_FAMILY, 
	NO_PRESENTATION_FAMILY, 
	NO_DEVICE_EXTENSION_SUPPORT, 
	NO_SWAPCHAIN_SUPPORT
};


// Contains pointer to all Vulkan data; instantiation happens in main.c.
wsVulkan* vk;


// The meaty bits.
void wsVulkanInit(wsVulkan* vulkan_data, uint8_t windowID, bool isDebug);
VkResult wsVulkanDrawFrame(double delta_time);
void wsVulkanTerminate();

// Debug & debug-messenger-related functions.
uint8_t debug;
void wsVulkanSetDebug(uint8_t debugMode) {debug = debugMode;}
VkResult wsVulkanInitDebugMessenger();
void wsVulkanStopDebugMessenger();
void wsVulkanPopulateDebugMessengerCreationInfo(VkDebugUtilsMessengerCreateInfoEXT* createInfo);

// Verify device and platform functionality with Vulkan.
bool wsVulkanEnableValidationLayers(VkInstanceCreateInfo* createInfo);					// Enabled validation layers for detailed debugging.  REDUCES PERFORMANCE HEAVILY!!!		
bool wsVulkanEnableRequiredExtensions(VkInstanceCreateInfo* createInfo);				// Enabled extensions required by GLFW, debug mode, etc.
void wsVulkanAddDebugExtensions(const char*** extensions, uint32_t* num_extensions);	// Adds debug extension name to extensions**.

// Queue family management.
uint8_t wsVulkanCountUniqueQueueIndices();
void wsVulkanPopulateUniqueQueueFamiliesArray();
uint32_t wsVulkanFindQueueFamilies(wsQueueFamilies* indices, VkPhysicalDevice* physicalDevice, VkSurfaceKHR* surface);
bool wsVulkanHasFoundAllQueueFamilies(wsQueueFamilies* indices);

// Choose a physical device and set up a logical device for interfacing.
bool wsVulkanPickPhysicalDevice();
int32_t wsVulkanRatePhysicalDevice(VkPhysicalDevice* physicalDevice);
uint32_t wsVulkanEnumerateDeviceExtentions(VkPhysicalDevice* physicalDevice, VkExtensionProperties** availableExtensions);
bool wsVulkanCheckDeviceExtensionSupport(VkPhysicalDevice* physicalDevice);
VkSampleCountFlagBits wsVulkanGetMaxMSAASampleCount();
void wsVulkanQuerySwapChainSupport();
VkResult wsVulkanCreateLogicalDevice(uint32_t num_validation_layers, const char* const* validation_layers);

// Did someone say swap meet?  How bazaar.
VkResult wsVulkanCreateSwapChain();
void wsVulkanRecreateSwapChain();	// Typically used when the swap chain is no longer compatible with the window surface (typically on resize).
void wsVulkanDestroySwapChain();
void wsVulkanChooseSwapSurfaceFormat(wsSwapChain* swapchain_info);
void wsVulkanChooseSwapExtent();
void wsVulkanChooseSwapPresentMode(wsSwapChain* swapchain_info);
bool wsVulkanVerifySwapChainConfiguration(VkResult lastImageOperationResult, bool ignoreSuboptimal, bool ignoreFramebufferResize);	// Returns if we need to stop drawing.
uint32_t wsVulkanCreateSwapChainImageViews();		// returns number of image views created successfully.
VkResult wsVulkanCreateSwapChainFramebuffers();		// Creates framebuffers that reference swap chain's image views representing image attachments (color, depth, etc.).
VkResult wsVulkanCreateSurface();					// Creates a surface for drawing to our GLFW window.
VkResult wsVulkanCreateRenderPass();
VkResult wsVulkanCreateGraphicsPipeline();

// TODO: Move into shader.c/h.
VkResult wsVulkanCreateDescriptorPool();
VkResult wsVulkanCreateDescriptorSets();
VkResult wsVulkanCreateDescriptorSetLayout();
VkShaderModule wsVulkanCreateShaderModule(uint8_t shaderID);

VkResult wsVulkanCreateSyncObjects();		// Creates semaphores (for GPU command execution syncing) & fence(s) (for GPU & CPU command execution syncing).
VkResult wsVulkanCreateCommandPool();		// Create command pool for queueing command buffers to Vulkan.
VkResult wsVulkanCreateCommandBuffers();	// Creates command buffer for holding commands.
VkResult wsVulkanBeginSingleTimeCommands(VkCommandBuffer* commandbuffer, int32_t commandtype);
VkResult wsVulkanEndSingleTimeCommands(VkCommandBuffer* commandbuffer, int32_t commandtype);
VkResult wsVulkanRecordCommandBuffer(VkCommandBuffer* buffer, void* pushConstantBlock, uint32_t img_ndx);

// Used to associate models and textures.
wsRenderObject* wsVulkanCreateRenderObject(const char* meshPath, const char* texPath);
void wsVulkanDestroyRenderObject(wsRenderObject* renderObject);

// Color & depth buffering to allow for MSAA and properly rendered triangles.
VkResult wsVulkanCreateColorResources();
VkResult wsVulkanCreateDepthResources();
VkFormat wsVulkanFindDepthFormat();
bool wsVulkanHasStencilComponent(VkFormat format);

// Functions for creating and handling images and their memory.
VkResult wsVulkanCreateImageView(VkImage* image, VkImageView* image_view, VkFormat format, VkImageAspectFlags aspect_flags, uint32_t mipLevels);
VkResult wsVulkanGenerateMipMaps(wsTexture* texture, VkFormat imageFormat, int32_t textureWidth, int32_t textureHeight);
VkResult wsVulkanCreateTextureSampler();
VkResult wsVulkanCreateTextureImage(wsTexture* texture, const char* path);
VkFormat wsVulkanFindSupportedImageFormat(VkFormat* candidates, uint8_t num_candidates, VkImageTiling desired_tiling, VkFormatFeatureFlags desired_features);
void wsVulkanTransitionImageLayout(VkImage image, VkFormat format, VkImageLayout layout_old, VkImageLayout layout_new, uint32_t mipLevels);
void wsVulkanCopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

// Buffer stuffer.
VkResult wsVulkanCopyBuffer(VkBuffer buffer_src, VkBuffer buffer_dst, VkDeviceSize size);
VkResult wsVulkanCreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer *buffer, VkDeviceMemory *buffer_memory);
VkResult wsVulkanCreateVertexBuffer(wsMesh* mesh);
VkResult wsVulkanCreateIndexBuffer(wsMesh* mesh);
VkResult wsVulkanCreateUniformBuffers();
void wsVulkanUpdateUniformBuffer(uint32_t currentFrame, double delta_time);

// Who needs shadow maps?
bool wsVulkanCheckDeviceRayTracingExtensionSupport(VkPhysicalDevice* physicalDevice);

// Vulkan proxy functions.
VkResult wsVulkanCreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* createInfo, const VkAllocationCallbacks* allocator, VkDebugUtilsMessengerEXT* debugMessenger);
void wsVulkanDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* allocator);
static VKAPI_ATTR VkBool32 VKAPI_CALL wsVulkanDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT msg_severity, VkDebugUtilsMessageTypeFlagsEXT msg_type, const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data);

// Extras.
void wsVulkanFramebufferResizeCallback(GLFWwindow* window, int width, int height);	// Passed to GLFW for the window resize event.
float wsVulkanGetAspectRatio();
void wsVulkanPrint(const char* str, int32_t ID, int32_t numerator, int32_t denominator, VkResult result);		// Always prints.
void wsVulkanPrintQuiet(const char* str, int32_t ID, int32_t numerator, int32_t denominator, VkResult result);	// Only prints on error.


// Call after wsWindowInit().
void wsVulkanInit(wsVulkan* vulkan_data, uint8_t windowID, bool isDebug)
{
	wsVulkanSetDebug(isDebug);
	
	// Non-Vulkan-specific initialization stuff.
	vk = vulkan_data;								// Initialized in main.c.
	vk->swapchain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	vk->swapchain.framebufferHasResized = false;	// Ensure framebuffer is not immediately and unnecessarily resized.
	vk->supportsRayTracing = false;
	vk->windowID = windowID;																		// Specify which window we will be rendering to.
	glfwSetWindowUserPointer(wsWindowGetPtr(windowID), vk);											// Set the window user to our vulkan data.
	glfwSetFramebufferSizeCallback(wsWindowGetPtr(windowID), wsVulkanFramebufferResizeCallback);	// Allow Vulkan to resize its framebuffer when the window resizes.

	printf("\n---BEGIN VULKAN INITIALIZATION---\n");

	// Specify application info and store inside struct createInfo.
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Westy";
	appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 0, 0, 1);
	appInfo.pEngineName = "Westy Vulkan";
	appInfo.engineVersion = VK_MAKE_API_VERSION(0, 0, 0, 1);
	appInfo.apiVersion = VK_API_VERSION_1_3;
	appInfo.pNext = NULL;
	
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	
	wsVulkanEnableRequiredExtensions(&createInfo);
	
	// If debug, we will need this struct in a moment for vkCreateInstance().
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
	if(debug)
	{
		wsVulkanEnableValidationLayers(&createInfo);
		wsVulkanPopulateDebugMessengerCreationInfo(&debugCreateInfo);	// Allow the debug messenger to debug vkCreateInstance().
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	
	// Create Vulkan instance!
	VkResult result = vkCreateInstance(&createInfo, NULL, &vk->instance);
	wsVulkanPrint("instance creation", NONE, NONE, NONE, result);
	
	if(debug)	{wsVulkanInitDebugMessenger();}
	wsVulkanCreateSurface();					// Creates window surface for drawing.
	wsVulkanPickPhysicalDevice();				// Physical device == GPU.
	wsVulkanCreateLogicalDevice(createInfo.enabledLayerCount, createInfo.ppEnabledLayerNames);
	wsVulkanCreateSwapChain();					// Swap chain is in charge of swapping images to the screen when they are needed.
	wsVulkanCreateSwapChainImageViews();		// Image views describe how we will use, write-to, and read each image.
	wsVulkanCreateRenderPass();
	wsVulkanCreateDescriptorSetLayout();
	vk->testMesh = *wsMeshInit();
	wsVulkanCreateGraphicsPipeline();
	wsVulkanCreateColorResources();
	wsVulkanCreateDepthResources();
	wsVulkanCreateSwapChainFramebuffers();		// Used for interfacing with image view attachments.
	wsVulkanCreateCommandPool();				// Used for executing commands sent via command buffer.
	vk->testTexture = *wsTextureInit(&vk->logicalDevice);
	wsVulkanCreateTextureSampler();				// Creates a texture sampler for use with ALL textures!
	vk->testRenderObject = *wsVulkanCreateRenderObject("models/vikingroom.gltf", "textures/vikingroom.png");
	wsVulkanCreateVertexBuffer(&vk->testMesh);	// Must be done after the pipeline is created.
	wsVulkanCreateIndexBuffer(&vk->testMesh);	// "" "" "" "" "" "" "" "" "" "" "" "" "" "" 
	wsVulkanCreateUniformBuffers();
	wsVulkanCreateDescriptorPool();
	wsVulkanCreateDescriptorSets();
	wsVulkanCreateCommandBuffers();		// Used for queueing commands for the command pool to execute.
	wsVulkanCreateSyncObjects();		// Creates semaphores & fences for preventing CPU & GPU sync issues when passing image data around.
	
	printf("---END VULKAN INITIALIZATION---\n");
}

VkResult wsVulkanDrawFrame(double delta_time)
{
	// Wait for any GPU tasks for the current frame to finish up before continuing.
	vkWaitForFences(vk->logicalDevice, 1, &vk->inFlightFences[vk->swapchain.currentFrame], VK_TRUE, UINT64_MAX);
	
	uint32_t img_ndx;
	VkResult result = vkAcquireNextImageKHR
	(
		vk->logicalDevice, 
		vk->swapchain.sc, 
		UINT64_MAX, 
		vk->imageAvailableSemaphores[vk->swapchain.currentFrame], 
		VK_NULL_HANDLE, 
		&img_ndx
	);
	
	if(wsVulkanVerifySwapChainConfiguration(result, true, true))
		return result;
	
	wsVulkanUpdateUniformBuffer(vk->swapchain.currentFrame, delta_time);
	
	vkResetFences(vk->logicalDevice, 1, &vk->inFlightFences[vk->swapchain.currentFrame]);
	
	VkCommandBuffer* currentCmdBuffer = &vk->swapchain.cmdBuffers[vk->swapchain.currentFrame];
	vkResetCommandBuffer(*currentCmdBuffer, 0);
	vk->globalPushConstants.time += (float)delta_time;
	wsVulkanRecordCommandBuffer(currentCmdBuffer, (void*)&vk->globalPushConstants, img_ndx);
	
	// Create configuration for queue submission & synchronization.
	VkSubmitInfo submit_info			= {};
	VkSemaphore waitSemaphores[]		= {vk->imageAvailableSemaphores[vk->swapchain.currentFrame]};
	VkSemaphore signalSemaphores[]		= {vk->renderFinishSemaphores[vk->swapchain.currentFrame]};
	VkPipelineStageFlags waitStages		= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	submit_info.sType					= VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount		= 1;
	submit_info.waitSemaphoreCount		= 1;
	submit_info.signalSemaphoreCount	= 1;
	submit_info.pCommandBuffers			= &currentCmdBuffer[0];
	submit_info.pWaitSemaphores			= &waitSemaphores[0];
	submit_info.pSignalSemaphores		= &signalSemaphores[0];
	submit_info.pWaitDstStageMask		= &waitStages;
	result = vkQueueSubmit(vk->queues.graphicsQueue, 1, &submit_info, vk->inFlightFences[vk->swapchain.currentFrame]);
	wsVulkanPrintQuiet("draw command buffer submission", NONE, NONE, NONE, result);
	
	VkPresentInfoKHR present_info	= {};
	VkSwapchainKHR swapchains[]		= {vk->swapchain.sc};
	present_info.sType				= VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.waitSemaphoreCount	= 1;
	present_info.swapchainCount		= 1;
	present_info.pWaitSemaphores	= &signalSemaphores[0];
	present_info.pSwapchains		= &swapchains[0];
	present_info.pImageIndices		= &img_ndx;
	present_info.pResults			= NULL;
	result = vkQueuePresentKHR(vk->queues.presentQueue, &present_info);
	
	if(wsVulkanVerifySwapChainConfiguration(result, false, false))
		return result;
	
	// Loop around to the next frame in the swapchain.
	vk->swapchain.currentFrame = (vk->swapchain.currentFrame + 1) % WS_MAX_FRAMES_IN_FLIGHT;
		
	return VK_SUCCESS;
}

bool wsVulkanVerifySwapChainConfiguration(VkResult lastImageOperationResult, bool ignoreSuboptimal, bool ignoreFramebufferResize)
{
	if(!ignoreFramebufferResize && vk->swapchain.framebufferHasResized)
	{
		vk->swapchain.framebufferHasResized = false;
		printf("WARNING: Vulkan framebuffer found to be wrong size while presenting image; recreating!\n");
		wsVulkanRecreateSwapChain();
		return true;
	}
	
	switch(lastImageOperationResult)
	{
		case VK_SUCCESS: 
			break;
			
		case VK_ERROR_OUT_OF_DATE_KHR: 
			printf("WARNING: Vulkan swap chain configuration found to be out of date while presenting image; recreating!\n");
			wsVulkanRecreateSwapChain();
			return true;
		
		case VK_SUBOPTIMAL_KHR: 
			if(ignoreSuboptimal)
				return false;
			printf("WARNING: Vulkan swap chain configuration found to be suboptimal while presenting image; recreating!\n");
			wsVulkanRecreateSwapChain();
			return true;
		
		default: 
			printf("ERROR: Vulkan swap chain failed to present image with result code %i!", lastImageOperationResult);
			return true;
	}
	
	return false;
}

void wsVulkanTerminate()
{
	// Wait for all asynchronous elements to finish execution and return to an idle state before continuing on.sss
	vkDeviceWaitIdle(vk->logicalDevice);
	
	printf("\n---BEGIN VULKAN TERMINATION---\n");
	
	if(debug)
	{
		wsVulkanStopDebugMessenger();
	}
	
	// Destroy all sync objects.
	for(uint8_t i = 0; i < WS_MAX_FRAMES_IN_FLIGHT; i++)
		{ vkDestroySemaphore(vk->logicalDevice, vk->imageAvailableSemaphores[i], NULL);	printf("INFO: Vulkan \"image available?\" semaphore %i/%i destroyed!\n", (i + 1), WS_MAX_FRAMES_IN_FLIGHT); }
	for(uint8_t i = 0; i < WS_MAX_FRAMES_IN_FLIGHT; i++)
		{ vkDestroySemaphore(vk->logicalDevice, vk->renderFinishSemaphores[i], NULL);	printf("INFO: Vulkan \"render finished?\" semaphore %i/%i destroyed!\n", (i + 1), WS_MAX_FRAMES_IN_FLIGHT); }
	for(uint8_t i = 0; i < WS_MAX_FRAMES_IN_FLIGHT; i++)
		{ vkDestroyFence(vk->logicalDevice, vk->inFlightFences[i], NULL);					printf("INFO: Vulkan \"in flight?\" fence %i/%i destroyed!\n", (i + 1), WS_MAX_FRAMES_IN_FLIGHT); }
	
	// Free memory associated with queue families.
	free(vk->queues.uniqueQueueFamilyIndices);
	
	// Command queueing and pooling.
	vkDestroyCommandPool(vk->logicalDevice, vk->cmdPoolGraphics, NULL);	printf("INFO: Vulkan graphics/presentation command pool destroyed!\n");
	vkDestroyCommandPool(vk->logicalDevice, vk->cmdPoolTransfer, NULL);	printf("INFO: Vulkan transfer command pool destroyed!\n");
	
	vkDestroyPipeline(vk->logicalDevice, vk->pipeline, NULL);				printf("INFO: Vulkan graphics pipeline destroyed!\n");
	vkDestroyPipelineLayout(vk->logicalDevice, vk->pipelineLayout, NULL);	printf("INFO: Vulkan pipeline layout destroyed!\n");
	wsVulkanDestroySwapChain();												// Already prints info messages!
	
	vkDestroySampler(vk->logicalDevice, vk->texturesampler, NULL);			printf("INFO: Vulkan texture sampler destroyed!\n");
	
	for(uint8_t i = 0; i < WS_MAX_RENDER_OBJECTS; i++)
	{
		wsVulkanDestroyRenderObject(&vk->renderObjects[i]);
	}
	// wsVulkanDestroyRenderObject(&vk->testRenderObject);
	wsTextureDestroy(&vk->testTexture);
	wsMeshDestroy(&vk->testMesh, &vk->logicalDevice);
	
	// Free UBO, vertex, & index buffer memory.
	for(uint8_t i = 0; i < WS_MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroyBuffer(vk->logicalDevice, vk->uniformBuffers[i], NULL);
		vkFreeMemory(vk->logicalDevice, vk->uniformBuffersMemory[i], NULL);
	}
	printf("INFO: All Vulkan UBOs destroyed!\n");
	// TODO: Install Valgrind to double check that I am not leaking memory here.  I don't think I am...
	
	// Destroying the pool will implicitly destroy all descriptor sets allocated from it.
	vkDestroyDescriptorPool(vk->logicalDevice, vk->descriptorPool, NULL);			printf("INFO: Vulkan descriptor pool destroyed!\n");
	vkDestroyDescriptorSetLayout(vk->logicalDevice, vk->descriptorSetLayout, NULL);	printf("INFO: Vulkan descriptor set layout destroyed!\n");
	
	vkDestroyRenderPass(vk->logicalDevice, vk->renderPass, NULL);	printf("INFO: Vulkan render pass destroyed!\n");
	wsShaderUnloadAll(&vk->shader);
	vkDestroyDevice(vk->logicalDevice, NULL);				printf("INFO: Vulkan logical device destroyed!\n");
	vkDestroySurfaceKHR(vk->instance, vk->surface, NULL);	printf("INFO: Vulkan surface destroyed!\n");
	vkDestroyInstance(vk->instance, NULL);					printf("INFO: Vulkan instance destroyed!\n");
	
	printf("---END VULKAN TERMINATION---\n\n");
}

VkResult wsVulkanCreateSyncObjects()
{
	// Resize arrays to hold all required semaphores & fences.
	vk->imageAvailableSemaphores= malloc(WS_MAX_FRAMES_IN_FLIGHT * sizeof(VkSemaphore));
	vk->renderFinishSemaphores	= malloc(WS_MAX_FRAMES_IN_FLIGHT * sizeof(VkSemaphore));
	vk->inFlightFences			= malloc(WS_MAX_FRAMES_IN_FLIGHT * sizeof(VkFence));
	
	VkSemaphoreCreateInfo semaphore_info = {};
	semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	
	VkFenceCreateInfo fence_info = {};
	fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;		// Prevents first call to wsVulkanDrawFrame() from hanging up on an unsignaled fence.
	
	// Create all semaphores & fences!
	for(uint8_t i = 0; i < WS_MAX_FRAMES_IN_FLIGHT; i++)
		{ VkResult result = vkCreateSemaphore(vk->logicalDevice, &semaphore_info, NULL, &vk->imageAvailableSemaphores[i]);	wsVulkanPrint("\"image available?\" semaphore creation", NONE, (i + 1), WS_MAX_FRAMES_IN_FLIGHT, result); }
	for(uint8_t i = 0; i < WS_MAX_FRAMES_IN_FLIGHT; i++)
		{ VkResult result = vkCreateSemaphore(vk->logicalDevice, &semaphore_info, NULL, &vk->renderFinishSemaphores[i]);	wsVulkanPrint("\"render finished?\" semaphore creation", NONE, (i + 1), WS_MAX_FRAMES_IN_FLIGHT, result); }
	for(uint8_t i = 0; i < WS_MAX_FRAMES_IN_FLIGHT; i++)
		{ VkResult result = vkCreateFence(vk->logicalDevice, &fence_info, NULL, &vk->inFlightFences[i]);	wsVulkanPrint("\"in flight?\" semaphore creation", NONE, (i + 1), WS_MAX_FRAMES_IN_FLIGHT, result); }
	
	return VK_SUCCESS;
}

VkResult wsVulkanRecordCommandBuffer(VkCommandBuffer* commandbuffer, void* pushConstantBlock, uint32_t img_ndx)
{
	// Begin recording to the command buffer!
	VkCommandBufferBeginInfo begin_info = {};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = 0;
	begin_info.pInheritanceInfo = NULL;
	
	VkResult result = vkBeginCommandBuffer(*commandbuffer, &begin_info);
	wsVulkanPrintQuiet("command buffer recording begin", NONE, NONE, NONE, result);
	
	// Begin render pass.
	VkRenderPassBeginInfo renderPass_info = {};
	renderPass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPass_info.renderPass = vk->renderPass;
	renderPass_info.framebuffer = vk->swapchain.framebuffers[img_ndx];
	renderPass_info.renderArea.offset.x = 0;
	renderPass_info.renderArea.offset.y = 0;
	renderPass_info.renderArea.extent = vk->swapchain.extent;
	
	VkClearValue clear_color = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
	VkClearValue clear_depth = {{{1.0f, 0}}};
	VkClearValue clear_values[3] = {clear_color, clear_color, clear_depth};
	renderPass_info.clearValueCount = 3;
	renderPass_info.pClearValues = &clear_values[0];
	
	vkCmdBeginRenderPass(*commandbuffer, &renderPass_info, VK_SUBPASS_CONTENTS_INLINE);
	
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
	VkBuffer vertexbuffers[1]		= {vk->testRenderObject.mesh.vertexBuffer.buffer};
	VkDeviceSize offsets[1]			= {0};
	vkCmdBindVertexBuffers(*commandbuffer, 0, 1, vertexbuffers, offsets);
	vkCmdBindIndexBuffer(*commandbuffer, vk->testRenderObject.mesh.indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT16);
	vkCmdBindDescriptorSets(*commandbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk->pipelineLayout, 
								0, 1, &vk->descriptorSets[vk->swapchain.currentFrame], 0, NULL);
	vkCmdPushConstants(*commandbuffer, vk->pipelineLayout, vk->globalPushConstantRange.stageFlags,
						vk->globalPushConstantRange.offset, vk->globalPushConstantRange.size, pushConstantBlock);
	vkCmdDrawIndexed(*commandbuffer, (uint32_t)vk->testRenderObject.mesh.num_indices, 1, 0, 0, 0);
	
	vkCmdEndRenderPass(*commandbuffer);
	
	// End command buffer recording.
	result = vkEndCommandBuffer(*commandbuffer);
	wsVulkanPrintQuiet("command buffer recording end", NONE, NONE, NONE, result);
	return result;
}

VkResult wsVulkanCreateCommandBuffers()
{
	// Resize command buffers array to contain enough command buffers for each in-flight frame.
	vk->swapchain.cmdBuffers = malloc(WS_MAX_FRAMES_IN_FLIGHT * sizeof(VkCommandBuffer));
	
	// Specify command buffer allocation configuration.
	VkCommandBufferAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	alloc_info.commandPool = vk->cmdPoolGraphics;
	alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	alloc_info.commandBufferCount = (uint32_t)WS_MAX_FRAMES_IN_FLIGHT;
	
	// Allocate all command buffers.
	for(uint8_t i = 0; i < WS_MAX_FRAMES_IN_FLIGHT; i++)
	{
		VkResult result = vkAllocateCommandBuffers(vk->logicalDevice, &alloc_info, vk->swapchain.cmdBuffers);
		wsVulkanPrint("command buffer creation", NONE, (i + 1), WS_MAX_FRAMES_IN_FLIGHT, result);
	}
	
	return VK_SUCCESS;
}

VkResult wsVulkanCreateCommandPool()
{
	VkCommandPoolCreateInfo graphicspool_info = {};
	graphicspool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	graphicspool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	graphicspool_info.queueFamilyIndex = vk->queues.graphicsFamilyIndex;
	
	VkResult result = vkCreateCommandPool(vk->logicalDevice, &graphicspool_info, NULL, &vk->cmdPoolGraphics);
	wsVulkanPrint("graphics/presentation command pool creation", NONE, NONE, NONE, result);
	if(result != VK_SUCCESS)
		return result;
	
	VkCommandPoolCreateInfo transferpool_info = {};
	transferpool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	transferpool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	transferpool_info.queueFamilyIndex = vk->queues.transferFamilyIndex;
	
	result = vkCreateCommandPool(vk->logicalDevice, &transferpool_info, NULL, &vk->cmdPoolTransfer);
	wsVulkanPrint("transfer command pool creation", NONE, NONE, NONE, result);
	return result;
}

uint32_t wsVulkanFindMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memory_properties;
	vkGetPhysicalDeviceMemoryProperties(vk->physicalDevice, &memory_properties);
	
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
	for(uint8_t i = 0; i < WS_MAX_RENDER_OBJECTS; i++)
	{
		if(!vk->renderObjects[i].isActive)
		{
			vk->renderObjects[i].isActive = true;
			
			wsTextureCreate(texPath, &vk->renderObjects[i].texture);
			
			wsMeshCreate(meshPath, &vk->renderObjects[i].mesh);
			
			/* In order to create our initial descriptor set, we need a mesh.  However, we cannot 
				generate its vertex and index buffers until AFTER the pipeline has been created.  
				Therefore, we must do this part here, and not as a part of wsMeshCreate(). */
			wsVulkanCreateVertexBuffer(&vk->renderObjects[i].mesh);
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
		wsMeshDestroy(&renderObject->mesh, &vk->logicalDevice);
	}
}

VkResult wsVulkanCreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits MSAASamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage* image, VkDeviceMemory *image_memory)
{
	// Initialize VkImage inside of struct vk.
	VkImageCreateInfo image_info = {};
	image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	image_info.imageType = VK_IMAGE_TYPE_2D;
	image_info.extent.width = width;
	image_info.extent.height = height;
	image_info.extent.depth = 1;
	image_info.mipLevels = mipLevels;
	image_info.arrayLayers = 1;
	image_info.format = format;
	image_info.tiling = tiling;
	image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	image_info.usage = usage;
	image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	image_info.samples = MSAASamples;
	image_info.flags = 0;	// Optional.
	VkResult result = vkCreateImage(vk->logicalDevice, &image_info, NULL, image);
	wsVulkanPrintQuiet("image creation", (uintptr_t)image, NONE, NONE, result);
	
	// Specify memory requirements for image.
	VkMemoryRequirements memory_requirements;
	vkGetImageMemoryRequirements(vk->logicalDevice, *image, &memory_requirements);
	
	VkMemoryAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	alloc_info.allocationSize = memory_requirements.size;
	alloc_info.memoryTypeIndex = wsVulkanFindMemoryType(memory_requirements.memoryTypeBits, properties);
	result = vkAllocateMemory(vk->logicalDevice, &alloc_info, NULL, image_memory);
	wsVulkanPrintQuiet("image memory allocation", (uintptr_t)image_memory, NONE, NONE, result);
	
	result = vkBindImageMemory(vk->logicalDevice, *image, *image_memory, 0);
	return result;
}

VkResult wsVulkanCreateTextureImage(wsTexture* texture, const char* path)
{
	// First, load the image using stb_image.h.
	int tex_width;
	int tex_height;
	int tex_channels;
	stbi_uc* pixel_data = stbi_load(path, &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
	VkDeviceSize img_size = tex_width * tex_height * 4;	// For STBI_rgb_alpha, there will be 4 bytes needed per pixel.
	
	texture->mipLevels = (uint32_t)floor(log2(mfMaxUint32(tex_width, tex_height))) + 1;
	
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
	vkMapMemory(vk->logicalDevice, stagingbuffer_memory, 0, img_size, 0, &data);
		memcpy(data, pixel_data, (size_t)img_size);
	vkUnmapMemory(vk->logicalDevice, stagingbuffer_memory);
	stbi_image_free(pixel_data);
	
	// Create the image!
	VkResult result = wsVulkanCreateImage(tex_width, tex_height, texture->mipLevels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, 
											VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 
											VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &texture->image, &texture->memory);
	wsVulkanPrint("image memory binding", NONE, NONE, NONE, result);
	
	// Copy our staging buffer to a GPU/shader-suitable VkImage format.
	wsVulkanTransitionImageLayout(texture->image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, texture->mipLevels);
	wsVulkanCopyBufferToImage(stagingbuffer, texture->image, (uint32_t)tex_width, (uint32_t)tex_height);
	if(wsVulkanGenerateMipMaps(texture, VK_FORMAT_R8G8B8A8_SRGB, tex_width, tex_height) != VK_SUCCESS)
	{
		texture->mipLevels = 0;	// Setting this here after already transitioning the image layout may cause issues; for now it's fine.
		wsVulkanTransitionImageLayout(texture->image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, texture->mipLevels);
	}
	
	// Clean up!
	vkDestroyBuffer(vk->logicalDevice, stagingbuffer, NULL);
	vkFreeMemory(vk->logicalDevice, stagingbuffer_memory, NULL);
		
	return result;
}

VkResult wsVulkanCreateImageView(VkImage* image, VkImageView* image_view, VkFormat format, VkImageAspectFlags aspect_flags, uint32_t mipLevels)
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
	view_info.subresourceRange.levelCount = mipLevels;
	view_info.subresourceRange.baseArrayLayer = 0;
	view_info.subresourceRange.layerCount = 1;
	
	VkResult result = vkCreateImageView(vk->logicalDevice, &view_info, NULL, image_view);
	wsVulkanPrintQuiet("image view creation", (intptr_t)image_view, NONE, NONE, result);
	return result;
}

void wsVulkanTransitionImageLayout(VkImage image, VkFormat format, VkImageLayout layout_old, VkImageLayout layout_new, uint32_t mipLevels)
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
	barrier_acquire.subresourceRange.levelCount = mipLevels;
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
		
		if(vk->queues.graphicsFamilyIndex != vk->queues.transferFamilyIndex)
		{
			commandtype = COMMAND_QUEUE_OWNERSHIP_TRANSFER;
			
			barrier_release.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier_release.oldLayout = layout_old;
			barrier_release.newLayout = layout_new;
			barrier_release.srcQueueFamilyIndex = vk->queues.transferFamilyIndex;
			barrier_release.dstQueueFamilyIndex = vk->queues.graphicsFamilyIndex;
			barrier_release.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier_release.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			barrier_release.image = image;
			barrier_release.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			barrier_release.subresourceRange.baseMipLevel = 0;
			barrier_release.subresourceRange.levelCount = mipLevels;
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

VkResult wsVulkanGenerateMipMaps(wsTexture* texture, VkFormat imageFormat, int32_t textureWidth, int32_t textureHeight)
{
	VkFormatProperties formatProperties;
	vkGetPhysicalDeviceFormatProperties(vk->physicalDevice, imageFormat, &formatProperties);
	
	if(!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
	{
		printf("ERROR: Vulkan texture image format %i does not support linear blitting!\n", imageFormat);
		return VK_ERROR_FORMAT_NOT_SUPPORTED;
	}
	
	VkCommandBuffer cmdBuffer;
	wsVulkanBeginSingleTimeCommands(&cmdBuffer, COMMAND_GRAPHICS);
	
	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.image = texture->image;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.subresourceRange.levelCount = 1;
	
	int32_t mipWidth = textureWidth;
	int32_t mipHeight = textureHeight;
	
	for(uint32_t i = 1; i < texture->mipLevels; i++)
	{
		barrier.subresourceRange.baseMipLevel = (i - 1);
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		
		vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 
								0, 0, NULL, 0, NULL, 1, &barrier);
		
		VkImageBlit blit = {};
		
		blit.srcOffsets[0] = (VkOffset3D){0, 0, 0};
		blit.srcOffsets[1] = (VkOffset3D){mipWidth, mipHeight, 1};
		blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.srcSubresource.mipLevel = (i - 1);
		blit.srcSubresource.baseArrayLayer = 0;
		blit.srcSubresource.layerCount = 1;
		
		blit.dstOffsets[0] = (VkOffset3D){0, 0, 0};
		int32_t dstMipWidth = (mipWidth > 1 ? mipWidth / 2 : 1);
		int32_t dstMipHeight = (mipHeight > 1 ? mipHeight / 2 : 1);
		blit.dstOffsets[1] = (VkOffset3D){dstMipWidth, dstMipHeight, 1};
		blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.dstSubresource.mipLevel = i;
		blit.dstSubresource.baseArrayLayer = 0;
		blit.dstSubresource.layerCount = 1;
		
		vkCmdBlitImage(cmdBuffer, texture->image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, 
						texture->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, 
						VK_FILTER_LINEAR);
		
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		
		vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 
								0, 0, NULL, 0, NULL, 1, &barrier);
		
		// In case an image is not perfectly square, we prevent an erroneous mip level of 0.
		if(mipWidth > 1)
			mipWidth /= 2;
		if(mipHeight > 1)
			mipHeight /= 2;
	}
	
	barrier.subresourceRange.baseMipLevel = (texture->mipLevels - 1);
	barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	
	vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 
							0, 0, NULL, 0, NULL, 1, &barrier);
	
	wsVulkanEndSingleTimeCommands(&cmdBuffer, COMMAND_GRAPHICS);
	
	printf("INFO: Swap chain \"Multum in Parvo\" maps have successfully been generated!\n");
	
	return VK_SUCCESS;
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
	vkGetPhysicalDeviceFeatures(vk->physicalDevice, &device_features);
	sampler_info.anisotropyEnable = device_features.samplerAnisotropy;
	
	// Get the maximum anisotropy level available to us, and use that!
	if(device_features.samplerAnisotropy)
	{
		VkPhysicalDeviceProperties device_properties = {};
		vkGetPhysicalDeviceProperties(vk->physicalDevice, &device_properties);
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
	sampler_info.maxLod = (float)(vk->testTexture.mipLevels);
	
	VkResult result = vkCreateSampler(vk->logicalDevice, &sampler_info, NULL, &vk->texturesampler);
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
		vkGetPhysicalDeviceFormatProperties(vk->physicalDevice, format, &properties);
		
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

VkResult wsVulkanCreateColorResources()
{
	VkFormat colorFormat = vk->swapchain.imageFormat;
	
	VkResult result = wsVulkanCreateImage(vk->swapchain.extent.width, vk->swapchain.extent.height, 1, vk->swapchain.MSAASamples, colorFormat, 
											VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, 
											VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vk->swapchain.colorTexture.image, &vk->swapchain.colorTexture.memory);
	result = wsVulkanCreateImageView(&vk->swapchain.colorTexture.image, &vk->swapchain.colorTexture.view, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	
	return result;
}

VkResult wsVulkanCreateDepthResources()
{
	VkFormat depthFormat = wsVulkanFindDepthFormat();
	VkResult result = wsVulkanCreateImage(vk->swapchain.extent.width, vk->swapchain.extent.height, 1, vk->swapchain.MSAASamples, depthFormat, VK_IMAGE_TILING_OPTIMAL, 
											VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vk->swapchain.depthTexture.image, 
											&vk->swapchain.depthTexture.memory);
	result = wsVulkanCreateImageView(&vk->swapchain.depthTexture.image, &vk->swapchain.depthTexture.view, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
	
	return result;
}

VkResult wsVulkanBeginSingleTimeCommands(VkCommandBuffer* commandbuffer, int32_t commandtype)
{
	VkCommandPool* commandpool;
	if(commandtype == COMMAND_GRAPHICS)
		{ commandpool = &vk->cmdPoolGraphics; }
	else { commandpool = &vk->cmdPoolTransfer; }
	
	VkCommandBufferAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	alloc_info.commandPool = *commandpool;	// I bet this will crash, because I forget if I initialized this command pool.  I guess we'll see, won't we!
	alloc_info.commandBufferCount = 1;
	
	vkAllocateCommandBuffers(vk->logicalDevice, &alloc_info, commandbuffer);
	
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
		queue = &vk->queues.graphicsQueue;
		commandpool = &vk->cmdPoolGraphics;
	} else
	{
		queue = &vk->queues.transferQueue;
		commandpool = &vk->cmdPoolTransfer;
	}
	
	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = commandbuffer;
	VkResult result = vkQueueSubmit(*queue, 1, &submit_info, VK_NULL_HANDLE);
	
	/* TODO: Make this use fences instead of waiting for the queue to idle; this 
		would (allegedly) allow for multiple transfer commands to execute in parallel. */
	vkQueueWaitIdle(*queue);
	
	vkFreeCommandBuffers(vk->logicalDevice, *commandpool, 1, commandbuffer);
	
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
	if(vk->queues.uniqueQueueFamilyCount > 1)
		buffer_info.sharingMode = VK_SHARING_MODE_CONCURRENT;
	else buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	buffer_info.queueFamilyIndexCount = vk->queues.uniqueQueueFamilyCount;
	buffer_info.pQueueFamilyIndices = &vk->queues.uniqueQueueFamilyIndices[0];
	
	VkResult result = vkCreateBuffer(vk->logicalDevice, &buffer_info, NULL, buffer);
	wsVulkanPrintQuiet("buffer creation", (intptr_t)buffer, NONE, NONE, result);
	
	// Specify vertex buffer memory requirements.
	VkMemoryRequirements memory_requirements;
	vkGetBufferMemoryRequirements(vk->logicalDevice, *buffer, &memory_requirements);
	
	VkMemoryAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	alloc_info.allocationSize = memory_requirements.size;
	VkMemoryPropertyFlags memory_properties = properties;
	alloc_info.memoryTypeIndex = wsVulkanFindMemoryType(memory_requirements.memoryTypeBits, memory_properties);
	
	// Allocate and bind buffer memory to appropriate vertex buffer.
	result = vkAllocateMemory(vk->logicalDevice, &alloc_info, NULL, buffer_memory);
	wsVulkanPrintQuiet("buffer memory allocation", (intptr_t)buffer_memory, NONE, NONE, result);
	if(result != VK_SUCCESS)
		{ return result; }
	
	result = vkBindBufferMemory(vk->logicalDevice, *buffer, *buffer_memory, 0);
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
	vkMapMemory(vk->logicalDevice, stagingbuffer_memory, 0, buffer_size, 0, &buffer_data);
		memcpy(buffer_data, &mesh->vertices[0], (size_t)buffer_size);
	vkUnmapMemory(vk->logicalDevice, stagingbuffer_memory);
	
	// Create actual buffer for GPU streaming.
	result = wsVulkanCreateBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &mesh->vertexBuffer.buffer, &mesh->vertexBuffer.memory);
	
	// Copy the staging buffer's data into the vertex buffer!
	result = wsVulkanCopyBuffer(stagingbuffer, mesh->vertexBuffer.buffer, buffer_size);
	
	vkDestroyBuffer(vk->logicalDevice, stagingbuffer, NULL);
	vkFreeMemory(vk->logicalDevice, stagingbuffer_memory, NULL);
	
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
	vkMapMemory(vk->logicalDevice, stagingbuffer_memory, 0, buffer_size, 0, &buffer_data);
		memcpy(buffer_data, &mesh->indices[0], (size_t)buffer_size);
	vkUnmapMemory(vk->logicalDevice, stagingbuffer_memory);
	
	// Create actual buffer for GPU streaming.
	result = wsVulkanCreateBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &mesh->indexBuffer.buffer, &mesh->indexBuffer.memory);
	
	// Copy the staging buffer's data into the vertex buffer!
	result = wsVulkanCopyBuffer(stagingbuffer, mesh->indexBuffer.buffer, buffer_size);
	
	vkDestroyBuffer(vk->logicalDevice, stagingbuffer, NULL);
	vkFreeMemory(vk->logicalDevice, stagingbuffer_memory, NULL);
	
	return result;
}

VkResult wsVulkanCreateUniformBuffers()
{
	VkDeviceSize buffer_size = sizeof(wsUBO);
	vk->uniformBuffers			= (VkBuffer*)malloc(WS_MAX_FRAMES_IN_FLIGHT * sizeof(VkBuffer));
	vk->uniformBuffersMemory	= (VkDeviceMemory*)malloc(WS_MAX_FRAMES_IN_FLIGHT * sizeof(VkDeviceMemory));
	vk->uniformBuffersMapped	= (void**)malloc(WS_MAX_FRAMES_IN_FLIGHT * sizeof(void*));
	
	
	VkResult result;
	
	for(uint8_t i = 0; i < WS_MAX_FRAMES_IN_FLIGHT; i++)
	{
		wsVulkanCreateBuffer(buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | 
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &vk->uniformBuffers[i], &vk->uniformBuffersMemory[i]);
		
		// We may crash when we dereference a void* here.
		// vk->uniformBuffersMapped[i] = (void*)malloc(buffer_size);
		result = vkMapMemory(vk->logicalDevice, vk->uniformBuffersMemory[i], 0, buffer_size, 0, &vk->uniformBuffersMapped[i]);
		
		wsVulkanPrintQuiet("uniform buffer creation", NONE, (int32_t)i, (int32_t)WS_MAX_FRAMES_IN_FLIGHT, result);
	}
	
	wsVulkanPrint("uniform buffer creation", NONE, NONE, NONE, result);
	return result;
}

void wsVulkanUpdateUniformBuffer(uint32_t currentFrame, double delta_time)
{
	wsUBO ubo = {};
	
	// Model matrix.
	// static float rotation_amount = 0.0f;
	// rotation_amount += (delta_time / 15.0f);
	glm_mat4_copy(GLM_MAT4_IDENTITY, ubo.model);
	glm_rotate(ubo.model, M_PI, WS_XAXIS);
	glm_rotate(ubo.model, M_PI_2, WS_YAXIS);
	
	// View matrix.
	vec3 cameraTarget = GLM_VEC3_ZERO_INIT;
	glm_vec3_add(*vk->cameraPosition, *vk->cameraForward, cameraTarget);
	glm_lookat(*vk->cameraPosition, cameraTarget, *vk->cameraUp, ubo.view);
	
	// Projection matrix.
	glm_mat4_copy(*vk->cameraProjection, ubo.proj);
	
	// We may crash when we dereference a void* here.
	memcpy(vk->uniformBuffersMapped[currentFrame], &ubo, sizeof(ubo));
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
	
	VkResult result = vkCreateDescriptorSetLayout(vk->logicalDevice, &layout_info, NULL, &vk->descriptorSetLayout);
	wsVulkanPrint("descriptor set layouts creation", NONE, NONE, NONE, result);
	return result;
}

VkResult wsVulkanCreateDescriptorSets()
{
	/* Allocate 1 descriptor set layout for every 2 descriptor sets (1 descriptor set 
		per frame in-flight to allow parallel drawing commands on separate frames). */
	VkDescriptorSetLayout* layouts = (VkDescriptorSetLayout*)malloc(WS_MAX_DESCRIPTOR_SETS * sizeof(VkDescriptorSetLayout));
	for(uint8_t i = 0; i < WS_MAX_DESCRIPTOR_SETS; i++)
		{memcpy(&layouts[i], &vk->descriptorSetLayout, sizeof(VkDescriptorSetLayout));}
	
	VkDescriptorSetAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	alloc_info.descriptorPool = vk->descriptorPool;
	alloc_info.descriptorSetCount = WS_MAX_DESCRIPTOR_SETS;
	alloc_info.pSetLayouts = &layouts[0];
	
	VkResult result = vkAllocateDescriptorSets(vk->logicalDevice, &alloc_info, &vk->descriptorSets[0]);
	wsVulkanPrint("descriptor set allocation", NONE, NONE, NONE, result);
	if(result != VK_SUCCESS)
		{return result;}
	free(layouts);	// Causes crash?  Maybe!
	
	for(uint8_t i = 0; i < WS_MAX_DESCRIPTOR_SETS; i++)
	{
		VkWriteDescriptorSet descriptor_writes[2] = {};
		
		VkDescriptorBufferInfo buffer_info		= {};
		buffer_info.buffer						= vk->uniformBuffers[i];
		buffer_info.offset						= 0;
		buffer_info.range						= sizeof(wsUBO);
		descriptor_writes[0].sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptor_writes[0].dstSet				= vk->descriptorSets[i];
		descriptor_writes[0].dstBinding			= 0;
		descriptor_writes[0].dstArrayElement	= 0;
		descriptor_writes[0].descriptorType		= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptor_writes[0].descriptorCount	= 1;
		descriptor_writes[0].pBufferInfo		= &buffer_info;
		descriptor_writes[0].pImageInfo			= NULL;	// Optional.
		descriptor_writes[0].pTexelBufferView	= NULL;	// Optional.
		
		VkDescriptorImageInfo image_info		= {};
		image_info.imageLayout					= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		image_info.imageView					= vk->testRenderObject.texture.view;
		image_info.sampler						= vk->texturesampler;
		descriptor_writes[1].sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptor_writes[1].dstSet				= vk->descriptorSets[i];
		descriptor_writes[1].dstBinding			= 1;
		descriptor_writes[1].dstArrayElement	= 0;
		descriptor_writes[1].descriptorType		= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptor_writes[1].descriptorCount	= 1;
		descriptor_writes[1].pImageInfo			= &image_info;
		descriptor_writes[1].pBufferInfo		= NULL;	// Optional.
		descriptor_writes[1].pTexelBufferView	= NULL;	// Optional.
		
		
		vkUpdateDescriptorSets(vk->logicalDevice, 2, &descriptor_writes[0], 0, NULL);
	}
	
	return result;
}

VkResult wsVulkanCreateDescriptorPool()
{
	VkDescriptorPoolSize pool_sizes[2] = {};
	pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	pool_sizes[0].descriptorCount = (uint32_t)WS_MAX_FRAMES_IN_FLIGHT;
	pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	pool_sizes[1].descriptorCount = (uint32_t)WS_MAX_FRAMES_IN_FLIGHT;
	
	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.poolSizeCount = 2;
	pool_info.pPoolSizes = &pool_sizes[0];
	pool_info.maxSets = WS_MAX_FRAMES_IN_FLIGHT;
	pool_info.flags = 0;	// Optional.
	
	VkResult result = vkCreateDescriptorPool(vk->logicalDevice, &pool_info, NULL, &vk->descriptorPool);
	wsVulkanPrint("descriptor pool creation", NONE, NONE, NONE, result);
	return result;
}

void wsVulkanFramebufferResizeCallback(GLFWwindow* window, int width, int height)
	{vk->swapchain.framebufferHasResized = true;}

VkResult wsVulkanCreateSwapChainFramebuffers()
{
	
	VkResult result;
	vk->swapchain.framebuffers = malloc(vk->swapchain.imageCount * sizeof(VkFramebuffer));
	
	for(int i = 0; i < vk->swapchain.imageCount; i++)
	{
		VkFramebufferCreateInfo framebuffer_info = {};
		framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebuffer_info.renderPass = vk->renderPass;
		framebuffer_info.attachmentCount = 3;
		VkImageView attachments[3] = {vk->swapchain.colorTexture.view, vk->swapchain.imageViews[i], vk->swapchain.depthTexture.view};
		framebuffer_info.pAttachments = attachments;
		framebuffer_info.width = vk->swapchain.extent.width;
		framebuffer_info.height = vk->swapchain.extent.height;
		framebuffer_info.layers = 1;
		
		result = vkCreateFramebuffer(vk->logicalDevice, &framebuffer_info, NULL, &vk->swapchain.framebuffers[i]);
		wsVulkanPrintQuiet("framebuffer creation", NONE, (i + 1), vk->swapchain.imageCount, result);
		if(result != VK_SUCCESS)
			break;
	}
	
	// printf("INFO: %i/%i Vulkan framebuffers created!\n", vk->swapchain.imageCount, vk->swapchain.imageCount);
	return result;
}

VkResult wsVulkanCreateRenderPass()
{
	VkAttachmentDescription color_attachment = {};
	color_attachment.format	= vk->swapchain.imageFormat;
	color_attachment.samples= vk->swapchain.MSAASamples;
	color_attachment.loadOp	= VK_ATTACHMENT_LOAD_OP_CLEAR;
	color_attachment.storeOp= VK_ATTACHMENT_STORE_OP_STORE;
	color_attachment.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	color_attachment.stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
	color_attachment.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;
	color_attachment.finalLayout	= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	VkAttachmentReference colorattachment_reference = {};
	colorattachment_reference.attachment= 0;
	colorattachment_reference.layout	= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	
	// Necessary for resolving the multisampled color buffer into a format which is presentable.
	VkAttachmentDescription colorAttachmentResolve = {};
	colorAttachmentResolve.format = vk->swapchain.imageFormat;
	colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	VkAttachmentReference colorAttachmentResolveReference = {};
	colorAttachmentResolveReference.attachment = 1;
	colorAttachmentResolveReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	
	VkAttachmentDescription depth_attachment = {};
	depth_attachment.format = wsVulkanFindDepthFormat();
	depth_attachment.samples = vk->swapchain.MSAASamples;
	depth_attachment.loadOp	= VK_ATTACHMENT_LOAD_OP_CLEAR;
	depth_attachment.storeOp= VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depth_attachment.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depth_attachment.stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depth_attachment.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;
	depth_attachment.finalLayout	= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	VkAttachmentReference depthattachment_reference = {};
	depthattachment_reference.attachment = 2;
	depthattachment_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	
	VkSubpassDescription subpass_desc = {};
	subpass_desc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass_desc.colorAttachmentCount = 1;
	subpass_desc.pColorAttachments = &colorattachment_reference;
	subpass_desc.pResolveAttachments = &colorAttachmentResolveReference;
	subpass_desc.pDepthStencilAttachment = &depthattachment_reference;
	
	// Create render pass!
	VkRenderPassCreateInfo renderPass_info = {};
	renderPass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPass_info.attachmentCount = 3;
	VkAttachmentDescription attachments[3] = {color_attachment, colorAttachmentResolve, depth_attachment};
	renderPass_info.pAttachments = &attachments[0];
	renderPass_info.subpassCount= 1;
	renderPass_info.pSubpasses	= &subpass_desc;
	
	// Create render pass subpass dependency, so when we render we must wait for the finished swapchain image to render said image.
	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	renderPass_info.dependencyCount = 1;
	renderPass_info.pDependencies = &dependency;
	
	// Self-explanatory copypasta code.	
	VkResult result = vkCreateRenderPass(vk->logicalDevice, &renderPass_info, NULL, &vk->renderPass);
	wsVulkanPrint("render pass creation", NONE, NONE, NONE, result);
	return result;
}

VkShaderModule wsVulkanCreateShaderModule(uint8_t shaderID)
{
	// Specify shader module creation info.
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	
	createInfo.codeSize = vk->shader.shader_size[shaderID];
	createInfo.pCode = (const uint32_t*)vk->shader.shader_data[shaderID];

	// Create and return shader module!
	VkShaderModule module;
	VkResult result = vkCreateShaderModule(vk->logicalDevice, &createInfo, NULL, &module);
	wsVulkanPrint("shader module creation", shaderID, NONE, NONE, result);
	return module;
}

VkResult wsVulkanCreateGraphicsPipeline()
{
	vk->globalPushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	vk->globalPushConstantRange.offset = 0;
	vk->globalPushConstantRange.size = sizeof(wsGPCB);	// TODO: Make this modular so you can have different push constant blocks.
	
	
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
	multisampling_info.sampleShadingEnable	= VK_TRUE;
	multisampling_info.rasterizationSamples	= vk->swapchain.MSAASamples;
	multisampling_info.minSampleShading		= 0.2f;	// Closer to one makes the sample rate shading smoother.
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
	pipelinelayout_info.pSetLayouts	= &vk->descriptorSetLayout;
	pipelinelayout_info.pushConstantRangeCount = 1;
	pipelinelayout_info.pPushConstantRanges	= &vk->globalPushConstantRange;
	VkResult result = vkCreatePipelineLayout(vk->logicalDevice, &pipelinelayout_info, NULL, &vk->pipelineLayout);
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
	pipeline_info.layout = vk->pipelineLayout;
	pipeline_info.renderPass = vk->renderPass;
	pipeline_info.subpass = 0;
	// Specify if we should "derive" from a parent pipeline for creation.
	pipeline_info.basePipelineHandle = NULL;
	pipeline_info.basePipelineIndex = -1;
	result = vkCreateGraphicsPipelines(vk->logicalDevice, VK_NULL_HANDLE, 1, &pipeline_info, NULL, &vk->pipeline);
	
	// Destroy shader modules cause WE DON NEED EM NO MO
	vkDestroyShaderModule(vk->logicalDevice, vert_module, NULL);
	vkDestroyShaderModule(vk->logicalDevice, frag_module, NULL);
	
	wsVulkanPrint("graphics pipeline creation", NONE, NONE, NONE, result);
	return result;
}

// Returns number of image views created successfully.
uint32_t wsVulkanCreateSwapChainImageViews()
{
	uint32_t num_created = 0;

	// Allocate space for our image views within struct vk.
	vk->swapchain.imageViews = malloc(vk->swapchain.imageCount * sizeof(VkImageView));

	// Create image view for each swap chain image.
	for(uint32_t i = 0; i < vk->swapchain.imageCount; i++)
	{
		VkResult result = wsVulkanCreateImageView(&vk->swapchain.images[i], &vk->swapchain.imageViews[i], vk->swapchain.imageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
		if(result == VK_SUCCESS)
			num_created++;
	}
	
	if(num_created != vk->swapchain.imageCount)
		{ printf("ERROR: Only %i/%i image views created!\n", num_created, vk->swapchain.imageCount); }
	// else { printf("INFO: %i/%i Vulkan image views created!\n", num_created, vk->swapchain.imageCount); }
	
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
	uint32_t imageCount = vk->swapchain.capabilities.minImageCount + 2;
	vk->swapchain.imageCount = imageCount;

	// Specify swap chain creation info.
	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = vk->surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = vk->swapchain.surfaceFormat.format;
	createInfo.imageColorSpace = vk->swapchain.surfaceFormat.colorSpace;
	createInfo.imageExtent = vk->swapchain.extent;
	createInfo.imageArrayLayers = 1;	// Always 1 unless this is a stereoscopic 3D application.
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	// Check if queue family indices are unique.
	if(vk->queues.uniqueQueueFamilyCount > 1)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = vk->queues.uniqueQueueFamilyCount;
		createInfo.pQueueFamilyIndices = &vk->queues.uniqueQueueFamilyIndices[0];
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;	// Optional.
		createInfo.pQueueFamilyIndices = NULL;	// Optional.
	}
	createInfo.preTransform = vk->swapchain.capabilities.currentTransform;	// Do we want to apply any transformations to our swap chain content?  Nope!
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;	// Do we want to blend our program into other windows in the window system?  No siree!
	createInfo.presentMode = vk->swapchain.presentMode;
	createInfo.clipped = VK_TRUE;	// If another window obscures some pixels from our window, should we ignore drawing them?  Yeah, probably.
	createInfo.oldSwapchain = VK_NULL_HANDLE;	// Reference to old swap chain in case we ever have to create a new one!  NULL for now.
	
	// Create swap chain!
	VkResult result = vkCreateSwapchainKHR(vk->logicalDevice, &createInfo, NULL, &vk->swapchain.sc);
	
	// Store swap chain images in struct vk.
	vkGetSwapchainImagesKHR(vk->logicalDevice, vk->swapchain.sc, &imageCount, NULL);
	vk->swapchain.images = malloc(imageCount * sizeof(VkImage));
	vkGetSwapchainImagesKHR(vk->logicalDevice, vk->swapchain.sc, &imageCount, vk->swapchain.images);
	
	// Set current swap chain image format for easier access.
	vk->swapchain.imageFormat = vk->swapchain.surfaceFormat.format;

	printf("INFO: Creating swap chain with properties: \n\tExtent: %ix%i\n\tSurface format: %i\n\tPresentation mode: %i\n", 
		vk->swapchain.extent.width, vk->swapchain.extent.height, vk->swapchain.surfaceFormat.format, vk->swapchain.presentMode);
	
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
	vkDeviceWaitIdle(vk->logicalDevice);
	
	// Destroy & recreate the swap chain and its derivative components.
	printf("\n");
	wsVulkanDestroySwapChain();
	wsVulkanCreateSwapChain();
	wsVulkanCreateSwapChainImageViews();
	wsVulkanCreateColorResources();
	wsVulkanCreateDepthResources();
	wsVulkanCreateSwapChainFramebuffers();
	printf("\n");
}

void wsVulkanDestroySwapChain()
{
	// Destroy individual framebuffers within swapchain.
	for(uint32_t i = 0; i < vk->swapchain.imageCount; i++)
		{ vkDestroyFramebuffer(vk->logicalDevice, vk->swapchain.framebuffers[i], NULL); }
	free(vk->swapchain.framebuffers);
	// printf("INFO: Vulkan framebuffers destroyed!\n");
	
	wsTextureDestroy(&vk->swapchain.colorTexture);
	wsTextureDestroy(&vk->swapchain.depthTexture);
	// printf("INFO: Vulkan depth buffer destroyed!\n");
	
	// Destroy swap chain image views.
	for(uint32_t i = 0; i < vk->swapchain.imageCount; i++)
		{ vkDestroyImageView(vk->logicalDevice, vk->swapchain.imageViews[i], NULL); }
	free(vk->swapchain.imageViews);
	// printf("INFO: Vulkan image views destroyed!\n");

	// Destroy swap chain.
	vkDestroySwapchainKHR(vk->logicalDevice, vk->swapchain.sc, NULL);
	free(vk->swapchain.supportedFormats);
	free(vk->swapchain.supportedPresentModes);
	free(vk->swapchain.images);
	
	printf("INFO: Vulkan swap chain destroyed!\n");
}

// Choose a nice resolution to draw swap chain images at.
void wsVulkanChooseSwapExtent()
{
	wsSwapChain* swapchain_info = &vk->swapchain;
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
		swapchain_info->extent.width = mfClampUint32((uint32_t)width, capabilities->minImageExtent.width, capabilities->maxImageExtent.width);
		swapchain_info->extent.height = mfClampUint32((uint32_t)height, capabilities->minImageExtent.height, capabilities->maxImageExtent.height);
	}
}

// Choose a presentation mode for our swap chain drawing surface.
void wsVulkanChooseSwapPresentMode(wsSwapChain* swapchain_info)
{
	// If we find a presentation mode that works for us, select it and return.
	for(int32_t i = 0; i < swapchain_info->supportedPresentModeCount; i++)
	{
		VkPresentModeKHR* current_mode = &swapchain_info->supportedPresentModes[i];
		
		if(*current_mode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			swapchain_info->presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
			return;
		}
	}

	// If we don't find one that matches our request, pick the only guaranteed presentation mode.
	swapchain_info->presentMode = VK_PRESENT_MODE_FIFO_KHR;
}

// Choose a format and color space for our swap chain drawing surface.
void wsVulkanChooseSwapSurfaceFormat(wsSwapChain* swapchain_info)
{
	// If we find a format that works for us, select it and return.
	for(int32_t i = 0; i < swapchain_info->supportedFormatCount; i++)
	{
		VkSurfaceFormatKHR* current_format = &swapchain_info->supportedFormats[i];
		
		if(current_format->format == VK_FORMAT_B8G8R8A8_SRGB && current_format->colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			swapchain_info->surfaceFormat = *current_format;
			return;
		}
	}

	// If we don't find one that matches our request, pick the first available format.
	swapchain_info->surfaceFormat = swapchain_info->supportedFormats[0];
}

void wsVulkanQuerySwapChainSupport()
{
	// Query surface details into vk->swapchain_deets.
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk->physicalDevice, vk->surface, &vk->swapchain.capabilities);

	// Query supported surface supportedFormats.
	uint32_t supportedFormatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(vk->physicalDevice, vk->surface, &supportedFormatCount, NULL);
	vk->swapchain.supportedFormatCount = supportedFormatCount;

	if(supportedFormatCount != 0)
	{
		vk->swapchain.supportedFormats = malloc(supportedFormatCount * sizeof(VkSurfaceFormatKHR));
		vkGetPhysicalDeviceSurfaceFormatsKHR(vk->physicalDevice, vk->surface, &supportedFormatCount, vk->swapchain.supportedFormats);
	}

	// Query supported presentation modes.
	uint32_t supportedPresentModeCount;
	
	vkGetPhysicalDeviceSurfacePresentModesKHR(vk->physicalDevice, vk->surface, &supportedPresentModeCount, NULL);
	vk->swapchain.supportedPresentModeCount = supportedPresentModeCount;

	if(supportedPresentModeCount != 0)
	{
		vk->swapchain.supportedPresentModes = malloc(supportedPresentModeCount * sizeof(VkPresentModeKHR));
		vkGetPhysicalDeviceSurfacePresentModesKHR(vk->physicalDevice, vk->surface, &supportedPresentModeCount, vk->swapchain.supportedPresentModes);
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
	wsVulkanFindQueueFamilies(&vk->queues, &vk->physicalDevice, &vk->surface);
	
	// Store queue family creation infos for later use in binding to logical device creation info.
	uint32_t uniqueQueueFamilyCount = vk->queues.uniqueQueueFamilyCount;
	VkDeviceQueueCreateInfo* queue_create_infos = calloc(uniqueQueueFamilyCount, sizeof(VkDeviceQueueCreateInfo));
	
	// Specify creation info for queue families.
	for(uint8_t i = 0; i < uniqueQueueFamilyCount; i++)
	{
		queue_create_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_create_infos[i].queueFamilyIndex = vk->queues.uniqueQueueFamilyIndices[i];
		queue_create_infos[i].queueCount = 1;
		
		// Specify a queue priority from 0.0f-1.0f.  Required regardless of number of queues; influences scheduling of command buffer execution.
		float queue_priority = 1.0f;
		queue_create_infos[i].pQueuePriorities = &queue_priority;
	}
	
	// Used to specify device features that will be used.
	VkPhysicalDeviceFeatures device_features = {};
	vkGetPhysicalDeviceFeatures(vk->physicalDevice, &device_features);
	device_features.samplerAnisotropy = device_features.samplerAnisotropy;	// Request sampler anisotropy feature be enabled for texture sampling later on.
	device_features.sampleRateShading = VK_TRUE;	// Smooths out textures within geometry to compliment MSAA features (impacts performance).
	
	// Specify creation info for logical_gpuVK.
	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = uniqueQueueFamilyCount;
	createInfo.pQueueCreateInfos = queue_create_infos;
	createInfo.pEnabledFeatures = &device_features;
	createInfo.pNext = NULL;
	
	// Modern way of handling device-specific validation layers.
	const char* device_extensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	createInfo.enabledExtensionCount = 1;
	createInfo.ppEnabledExtensionNames = device_extensions;
	
	// Device-specific validation layers are deprecated for modern API versions, but required for older versions.
	if(debug)
	{
		createInfo.enabledLayerCount = (uint32_t)num_validation_layers;
		createInfo.ppEnabledLayerNames = validation_layers;
	}
	else
		{ createInfo.enabledLayerCount = 0; }
	
	
	// Create logicalDevice and return result!
	VkResult result = vkCreateDevice(vk->physicalDevice, &createInfo, NULL, &vk->logicalDevice);
	
	// Assign queue handles from logicalDevice.
	vkGetDeviceQueue(vk->logicalDevice, vk->queues.graphicsFamilyIndex, 0, &vk->queues.graphicsQueue);
	vkGetDeviceQueue(vk->logicalDevice, vk->queues.transferFamilyIndex, 0, &vk->queues.transferQueue);
	vkGetDeviceQueue(vk->logicalDevice, vk->queues.presentFamilyIndex, 0, &vk->queues.presentQueue);
	
	printf("INFO: %i unique Vulkan queue families exist; indices are as specified:\n", uniqueQueueFamilyCount);
	printf("\tGraphics: %i\n\tTransfer: %i\n\tPresentation: %i\n", vk->queues.graphicsFamilyIndex, vk->queues.transferFamilyIndex, vk->queues.presentFamilyIndex);
	
	// FREE MEMORY!!!
	free(queue_create_infos);
	
	wsVulkanPrint("logical device creation", NONE, NONE, NONE, result);
	return result;
}

// Checks if all queue families have been found.
bool wsVulkanHasFoundAllQueueFamilies(wsQueueFamilies* indices)
{
	if(!indices->hasGraphicsFamily)
		return false;
	if(!indices->hasTransferFamily)
		return false;
	if(!indices->hasPresentFamily)
		return false;
	
	return true;
}

uint8_t wsVulkanCountUniqueQueueIndices()
{
	uint8_t num_queue_families = 0;
	
	for(uint8_t i = 0; i < 3; i++)
	{
		if((vk->queues.graphicsFamilyIndex == i) || (vk->queues.presentFamilyIndex == i) || (vk->queues.transferFamilyIndex == i))
		{
			num_queue_families++;
			continue;
		}
	}
	
	vk->queues.uniqueQueueFamilyCount = num_queue_families;
	
	return num_queue_families;
}

// Make sure we have a list of all unique queue families stored for future buffer use!
void wsVulkanPopulateUniqueQueueFamiliesArray()
{
	vk->queues.uniqueQueueFamilyIndices = malloc(vk->queues.uniqueQueueFamilyCount * sizeof(uint32_t));
	if(vk->queues.hasGraphicsFamily)	// If we have a graphics family, we likely support presentation, and we ALWAYS implicitly support transfer.
		vk->queues.uniqueQueueFamilyIndices[0] = vk->queues.graphicsFamilyIndex;
	if(vk->queues.graphicsFamilyIndex != vk->queues.transferFamilyIndex)
		vk->queues.uniqueQueueFamilyIndices[1] = vk->queues.transferFamilyIndex;
	if(vk->queues.graphicsFamilyIndex != vk->queues.presentFamilyIndex)
		vk->queues.uniqueQueueFamilyIndices[2] = vk->queues.presentFamilyIndex;
	
	/* printf("%i\n", wsVulkanGetUniqueQueueIndicesCount());
	for(uint8_t i = 0; i < wsVulkanGetUniqueQueueIndicesCount(); i++)
		{ printf("%i ", vk->queues.uniqueQueueFamilyIndices[i]); }
	printf("\n"); */
}

uint32_t wsVulkanFindQueueFamilies(wsQueueFamilies *indices, VkPhysicalDevice* physicalDevice, VkSurfaceKHR* surface)
{
	// Get list of queue families available to us.
	uint32_t num_queue_families = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(*physicalDevice, &num_queue_families, NULL);
	// printf("INFO: Your device supports %i queue families!\n", num_queue_families);
	
	VkQueueFamilyProperties* queue_families = malloc(num_queue_families * sizeof(VkQueueFamilyProperties));
	vkGetPhysicalDeviceQueueFamilyProperties(*physicalDevice, &num_queue_families, queue_families);
	
	// These will end up as true once a family is found.
	indices->hasGraphicsFamily = false;
	indices->hasTransferFamily = false;
	indices->hasPresentFamily = false;
	
	for(uint32_t i = 0; i < num_queue_families; i++)
	{
		VkQueueFamilyProperties queue_family = queue_families[i];
		
		if((queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) && !indices->hasGraphicsFamily)
		{
			indices->graphicsFamilyIndex = i;
			indices->hasGraphicsFamily = true;
			continue;
		}
		if((queue_family.queueFlags & VK_QUEUE_TRANSFER_BIT) && !indices->hasTransferFamily)
		{
			indices->transferFamilyIndex = i;
			indices->hasTransferFamily = true;
			continue;
		}
		
		VkBool32 has_present_support = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(*physicalDevice, i, *surface, &has_present_support);
		if(has_present_support && !indices->hasPresentFamily)
		{
			indices->presentFamilyIndex = i;
			indices->hasPresentFamily = true;
			continue;
		}
		
		if(wsVulkanHasFoundAllQueueFamilies(indices))
		{
			wsVulkanCountUniqueQueueIndices();
			wsVulkanPopulateUniqueQueueFamiliesArray();
			return num_queue_families;
		}
	}
	
	if(!indices->hasTransferFamily)
	{
		indices->transferFamilyIndex = indices->graphicsFamilyIndex;
		indices->hasTransferFamily = true;
	}
	if(!indices->hasPresentFamily)
	{
		indices->presentFamilyIndex = indices->graphicsFamilyIndex;
		indices->hasPresentFamily = true;
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
		printf("WARNING: Vulkan alleges your system has no GPUs installed...  You should get that looked at.\n");
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
		printf("INFO: Rating GPU %i...\n", i);

		// Make sure we are checking current GPU's suitability;
		vk->physicalDevice = GPUs[i];
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
		vk->physicalDevice = GPUs[ndx_GPU];
		vk->swapchain.MSAASamples = wsVulkanGetMaxMSAASampleCount();
		
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(vk->physicalDevice, &deviceProperties);
		printf("INFO: GPU %i - I CHOOSE YOU!!!\n", ndx_GPU);
		printf("\tDevice name: %s\n", deviceProperties.deviceName);
		printf("\tDevice type: %i\n", deviceProperties.deviceType);
		printf("\tDriver version: %i\n", deviceProperties.driverVersion);
		printf("\tDevice ID: %i\n", deviceProperties.deviceID);
		printf("\tVendor ID: %i\n", deviceProperties.vendorID);
		printf("\tAPI Version: %i\n", deviceProperties.apiVersion);
		
		printf("\tPipeline Cache UUID: ");
		for(uint8_t i = 0; i < 16; i++)	{printf("%i", deviceProperties.pipelineCacheUUID[i]);}
		printf("\n");
		
		printf("\tMax MSAA Sample Count: %i\n", vk->swapchain.MSAASamples);

		return true;
	}
	else
	{
		printf("ERROR: Failed to find GPUs with proper Vulkan support!\n");
		return false;
	}
	
	// Don't free GPU list so that we can allow user to swap GPUs in settings later.
}

int32_t wsVulkanRatePhysicalDevice(VkPhysicalDevice* physicalDevice)
{
	int32_t score = 0;
	
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	VkPhysicalDeviceMemoryProperties memoryProperties;
	vkGetPhysicalDeviceProperties(*physicalDevice, &deviceProperties);
	vkGetPhysicalDeviceFeatures(*physicalDevice, &deviceFeatures);
	vkGetPhysicalDeviceMemoryProperties(*physicalDevice, &memoryProperties);
	
	wsQueueFamilies indices;
	uint32_t queueFamilyCount = wsVulkanFindQueueFamilies(&indices, physicalDevice, &vk->surface);
	printf("\t%i queue families, max push constant size of %iMB\n", queueFamilyCount, deviceProperties.limits.maxPushConstantsSize);
	
	printf("\tMemory types: \n");
	for(uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
	{
		printf("\t\tType %i -> Heap %i w/ property flags: ", i, memoryProperties.memoryTypes[i].heapIndex);
		printb(memoryProperties.memoryTypes[i].propertyFlags);
		printf("\n");
	}
	
	printf("\tMemory heaps: \n");
	for(uint32_t i = 0; i < memoryProperties.memoryHeapCount; i++)
	{
		printf("\t\tHeap %i: %0.2fGB w/ property flags: ", i, (memoryProperties.memoryHeaps[i].size / 1073741824.0f));
		printb(memoryProperties.memoryHeaps[i].flags);
		printf("\n");
	}
	
	if(!deviceFeatures.geometryShader)	return NO_GEOMETRY_SHADER;
	if(!indices.hasGraphicsFamily)		return NO_GRAPHICS_FAMILY;
	if(!indices.hasTransferFamily)		return NO_TRANSFER_FAMILY;
	if(!indices.hasPresentFamily)		return NO_PRESENTATION_FAMILY;
	
	if(!wsVulkanCheckDeviceExtensionSupport(physicalDevice))
		return NO_DEVICE_EXTENSION_SUPPORT;
	wsVulkanQuerySwapChainSupport();
	if(vk->swapchain.supportedFormatCount <= 0 || vk->swapchain.supportedPresentModeCount <= 0)
		return NO_SWAPCHAIN_SUPPORT;
	
	score += deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? 2000 : 0;
	score += wsVulkanCheckDeviceRayTracingExtensionSupport(physicalDevice) * 1500;
	score += indices.graphicsFamilyIndex != indices.transferFamilyIndex ? 2000 : 0;	// If these are separate queue families, this WILL increase performance (likely to execute in parallel).
	score += indices.graphicsFamilyIndex == indices.presentFamilyIndex ? 1000 : 0;	// If these are the same queue family, this could increase performance (unlikely to execute in parallel).
	score -= deviceFeatures.samplerAnisotropy ? 0 : 500;
	score += deviceFeatures.sampleRateShading ? 0 : 500;
	score += deviceProperties.limits.maxImageDimension2D;
	score += deviceProperties.limits.maxPushConstantsSize;
	
	return score;
}

uint32_t wsVulkanEnumerateDeviceExtentions(VkPhysicalDevice* physicalDevice, VkExtensionProperties** availableExtensions)
{
	uint32_t availableExtensionCount;
	vkEnumerateDeviceExtensionProperties(*physicalDevice, NULL, &availableExtensionCount, NULL);
	
	*availableExtensions = (VkExtensionProperties*)malloc(availableExtensionCount * sizeof(VkExtensionProperties));
	vkEnumerateDeviceExtensionProperties(*physicalDevice, NULL, &availableExtensionCount, *availableExtensions);
	
	return availableExtensionCount;
}

bool wsVulkanCheckDeviceExtensionSupport(VkPhysicalDevice* physicalDevice)
{
	VkExtensionProperties* availableExtensions;
	uint32_t availableExtensionCount = wsVulkanEnumerateDeviceExtentions(physicalDevice, &availableExtensions);
	
	uint32_t num_required_extensions = 1;
	const char* required_extensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
	bool requiredExtensionsSupported = true;
	
	printf("\t%i device-specific extensions required: ", num_required_extensions);
	for(int32_t i = 0; i < num_required_extensions; i++)
		{ printf("\t\t%s", required_extensions[i]); }
	printf("\n");
	
	for(uint32_t i = 0; i < num_required_extensions; i++)
	{
		bool has_found_extension = false;
		
		for(uint32_t j = 0; j < availableExtensionCount; j++)
		{
			if(strcmp(required_extensions[i], availableExtensions[j].extensionName) == 0)
				{has_found_extension = true; break;}
		}

		if(!has_found_extension)
		{
			printf("\t\tERROR: REQUIRED device extension \"%s\" is not supported by your GPU!\n", required_extensions[i]);
			requiredExtensionsSupported = false;
		}
	}
	
	if(requiredExtensionsSupported)
		{printf("\t\tAll required device-specific extensions are supported by your GPU!\n");}
	
	free(availableExtensions);	// BUG: Maybe a memory issue here?

	return requiredExtensionsSupported;
}

bool wsVulkanCheckDeviceRayTracingExtensionSupport(VkPhysicalDevice* physicalDevice)
{
	VkExtensionProperties* availableExtensions;
	uint32_t availableExtensionCount = wsVulkanEnumerateDeviceExtentions(physicalDevice, &availableExtensions);
	
	uint32_t rayTracingExtensionCount = 3;
	const char* rayTracingExtensions[] = 
	{
		VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME, 
		VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME, 
		VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME
	};
	bool rayTracingExtensionsSupported = true;
	vk->supportsRayTracing = true;
	
	printf("\t%i device-specific ray tracing extensions preferred: ", rayTracingExtensionCount);
	for(int32_t i = 0; i < rayTracingExtensionCount; i++)
		{ printf("\t\t%s", rayTracingExtensions[i]); }
	printf("\n");
	
	for(uint32_t i = 0; i < rayTracingExtensionCount; i++)
	{
		bool hasFoundExtension = false;
		
		for(uint32_t j = 0; j < availableExtensionCount; j++)
		{
			if(strcmp(rayTracingExtensions[i], availableExtensions[j].extensionName) == 0)
				{hasFoundExtension = true; break;}
		}

		if(!hasFoundExtension)
		{
			printf("\t\tWARNING: Optional device ray tracing extension \"%s\" is not supported by your GPU!\n", rayTracingExtensions[i]);
			vk->supportsRayTracing = false;
			rayTracingExtensionsSupported = false;
		}
	}
	
	if(rayTracingExtensionsSupported)
		{printf("\t\tAll device-specific ray tracing extensions are supported by your GPU!\n");}
	
	free(availableExtensions);	// Bug: Maybe a memory issue?
	
	return rayTracingExtensionsSupported;
}

VkSampleCountFlagBits wsVulkanGetMaxMSAASampleCount()
{
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(vk->physicalDevice, &deviceProperties);
	
	VkSampleCountFlags flagCounts = deviceProperties.limits.framebufferColorSampleCounts & deviceProperties.limits.framebufferDepthSampleCounts;
	if(flagCounts & VK_SAMPLE_COUNT_64_BIT)	{return VK_SAMPLE_COUNT_64_BIT;}
	if(flagCounts & VK_SAMPLE_COUNT_32_BIT)	{return VK_SAMPLE_COUNT_32_BIT;}
	if(flagCounts & VK_SAMPLE_COUNT_16_BIT)	{return VK_SAMPLE_COUNT_16_BIT;}
	if(flagCounts & VK_SAMPLE_COUNT_8_BIT)	{return VK_SAMPLE_COUNT_8_BIT;}
	if(flagCounts & VK_SAMPLE_COUNT_4_BIT)	{return VK_SAMPLE_COUNT_4_BIT;}
	if(flagCounts & VK_SAMPLE_COUNT_2_BIT)	{return VK_SAMPLE_COUNT_2_BIT;}
	
	return VK_SAMPLE_COUNT_1_BIT;
}

VkResult wsVulkanInitDebugMessenger()
{
	// Populate creation info for debug messenger.
	VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
	wsVulkanPopulateDebugMessengerCreationInfo(&createInfo);
	
	// Create debug messenger!
	VkResult result = wsVulkanCreateDebugUtilsMessengerEXT(vk->instance, &createInfo, NULL, &vk->debugMessenger);
	wsVulkanPrint("debug messenger creation", NONE, NONE, NONE, result);
	
	return result;
}

void wsVulkanStopDebugMessenger()
{
	wsVulkanDestroyDebugUtilsMessengerEXT(vk->instance, vk->debugMessenger, NULL);
	printf("INFO: Vulkan debug messenger destroyed!\n");
}

// Vulkan proxy function; Create debug messenger.
VkResult wsVulkanCreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* createInfo, const VkAllocationCallbacks* allocator, VkDebugUtilsMessengerEXT* debugMessenger)
{
	PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != NULL)
	{
        return func(instance, createInfo, allocator, debugMessenger);
    }
	else
	{
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

// Vulkan proxy function; Destroy debug messenger.
void wsVulkanDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* allocator)
{
    PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != NULL)
	{
        func(instance, debugMessenger, allocator);
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
void wsVulkanPopulateDebugMessengerCreationInfo(VkDebugUtilsMessengerCreateInfoEXT* createInfo)
{
	// Specify creation info for debug messenger.
	createInfo->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	
	// Specify which messages the debug callback function should be called for.
	createInfo->messageSeverity = // VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | 
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	
	// Specify which messages the debug callback function should be notified about.
	createInfo->messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | 
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | 
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	
	// Debug callback function pointer.
	createInfo->pfnUserCallback = &wsVulkanDebugCallback;
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

bool wsVulkanEnableRequiredExtensions(VkInstanceCreateInfo* createInfo)
{
	// Get list of required Vulkan extensions from GLFW.
	uint32_t num_required_extensions = 0;
	const char** required_extensions = glfwGetRequiredInstanceExtensions(&num_required_extensions);
	if(debug)
		{ wsVulkanAddDebugExtensions(&required_extensions, &num_required_extensions); }
	
	// Set correct fields in createInfo.  Cannot create instance without this.
	createInfo->flags = 0;
	createInfo->enabledExtensionCount = num_required_extensions;
	createInfo->ppEnabledExtensionNames = required_extensions;
	
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

bool wsVulkanEnableValidationLayers(VkInstanceCreateInfo* createInfo)
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
	createInfo->enabledLayerCount = num_required_layers;
	createInfo->ppEnabledLayerNames = (const char**)required_layers;
	
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