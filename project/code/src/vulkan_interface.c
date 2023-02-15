#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>
#include<limits.h>

#include <vulkan/vulkan.h>
#include<GLFW/glfw3.h>

#include"h/vulkan_interface.h"
#include"h/window.h"
#include"h/shader.h"


// General Vulkan interfacing functions.
void wsVulkanInit(wsVulkan* vk, uint8_t windowID);
VkResult wsVulkanDrawFrame(wsVulkan* vk);
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
void wsVulkanChooseSwapSurfaceFormat(wsVulkanSwapChain* swapchain_info);	// Choose swap chain surface format.
void wsVulkanChooseSwapExtent(wsVulkan* vk);									// Choose swap chain image resolution.
void wsVulkanChooseSwapPresentMode(wsVulkanSwapChain* swapchain_info);		// Choose swap chain presentation mode.
VkResult wsVulkanCreateSwapChain(wsVulkan* vk);									// Creates a swap chain for image buffering.
uint32_t wsVulkanCreateImageViews(wsVulkan* vk);								// Creates image views viewing swap chain images; returns number of image views created successfully.
VkResult wsVulkanCreateLogicalDevice(wsVulkan* vk, uint32_t num_validation_layers, const char* const* validation_layers);	// Creates a logical device for interfacing with the GPU.
VkResult wsVulkanCreateSurface(wsVulkan* vk);			// Creates a surface for drawing to the screen.
VkResult wsVulkanCreateRenderPass(wsVulkan* vk);		// Creates a render pass.
VkResult wsVulkanCreateGraphicsPipeline(wsVulkan* vk);	// Creates a graphics pipeline and stores its ID inside of struct vk.
VkResult wsVulkanCreateFrameBuffers(wsVulkan* vk);		// Creates framebuffers that reference image views representing image attachments (color, depth, etc.).
VkResult wsVulkanCreateCommandPool(wsVulkan* vk);		// Create command pool for queueing commands to Vulkan.
VkResult wsVulkanCreateCommandBuffer(wsVulkan* vk);		// Creates command buffer for holding commands.
VkResult wsVulkanRecordCommandBuffer(wsVulkan* vk, uint32_t img_ndx);		// Records commands into a buffer.
VkResult wsVulkanCreateSyncObjects(wsVulkan* vk);							// Creates semaphores (for GPU command execution syncing) & fence(s) (for GPU & CPU command execution syncing).
VkShaderModule wsVulkanCreateShaderModule(wsVulkan* vk, uint8_t shaderID);	// Creates a shader module for the indicated shader.

// Queue family management.
void wsVulkanFindQueueFamilies(wsVulkanQueueFamilies* indices, VkPhysicalDevice* physical_device, VkSurfaceKHR* surface);	// Finds required queue families and stores them in indices.
bool wsVulkanHasFoundAllQueueFamilies(wsVulkanQueueFamilies* indices);	// Checks if all required queue families have been found.

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

	// Specify which window we will be rendering to.
	vk->windowID = windowID;


	// Specify application info and store inside struct create_info.
	VkApplicationInfo app_info;
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName = "Westy";
	app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	
	app_info.pEngineName = "Westy Vulkan";
	app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	
	app_info.apiVersion = VK_API_VERSION_1_0;
	app_info.pNext = NULL;
	
	VkInstanceCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_info.pApplicationInfo = &app_info;
	
	
	// Check that we have all the required extensions for Vulkan.
	wsVulkanEnableRequiredExtensions(&create_info);
	
	// Enable Vulkan validation layers if in debug mode.
	create_info.enabledLayerCount = 0;
	create_info.pNext = NULL;
	// If debug, do debug things.  If debug, we will need this struct in a moment for scope-related reasons in vkCreateInstance().
	VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {};
	if(debug) {
		wsVulkanEnableValidationLayers(&create_info);
		
		// This allows the debug messenger to debug vkCreateInstance(), as opposed to skipping that part.  Pretty important stuff!
		wsVulkanPopulateDebugMessengerCreationInfo(&debug_create_info);
		create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debug_create_info;
	}
	
	// Create Vulkan instance!
	VkResult result = vkCreateInstance(&create_info, NULL, &vk->instance);
	if(result != VK_SUCCESS)
		printf("ERROR: Vulkan instance creation failed with result code %i!\n", result);
	else printf("Vulkan instance created!\n");
	
	// Used for communicating when something has gone awry!
	if(debug) {
		wsVulkanInitDebugMessenger(vk);
	}
	
	wsVulkanCreateSurface(vk);
	
	// Physical device is interfaced with via the logical device.
	wsVulkanPickPhysicalDevice(vk);
	wsVulkanCreateLogicalDevice(vk, create_info.enabledLayerCount, create_info.ppEnabledLayerNames);
	
	wsVulkanCreateSwapChain(vk);		// Swap chain is in charge of swapping images to the screen when they are needed.
	wsVulkanCreateImageViews(vk);		// Image views describe how we will use, write-to, and read each image.
	wsVulkanCreateRenderPass(vk);
	wsVulkanCreateGraphicsPipeline(vk);	// Graphics pipeline combines all created objects and information into one abstraction for working with.
	wsVulkanCreateFrameBuffers(vk);		// Creates framebuffer objects for interfacing with image view attachments.
	
	wsVulkanCreateCommandPool(vk);		// Creates command pool, which is used for executing commands sent via command buffer.
	wsVulkanCreateCommandBuffer(vk);	// Creates command buffer, which is used for queueing commands for the command pool to execute.
	wsVulkanCreateSyncObjects(vk);
	
	printf("---End Vulkan Initialization!---\n");
}
VkResult wsVulkanDrawFrame(wsVulkan* vk) {
	
	// Wait for any important GPU tasks to finish up first.
	vkWaitForFences(vk->logical_device, 1, &vk->inflight_fence, VK_TRUE, UINT64_MAX);
	vkResetFences(vk->logical_device, 1, &vk->inflight_fence);
	
	// Acquire next swapchain image.
	uint32_t img_ndx;
	vkAcquireNextImageKHR(vk->logical_device, vk->swapchain.sc, UINT64_MAX, vk->img_available_semaphore, VK_NULL_HANDLE, &img_ndx);
	
	// Reset and record command buffer for submission to Vulkan.
	vkResetCommandBuffer(vk->commandbuffer, 0);
	wsVulkanRecordCommandBuffer(vk, img_ndx);
	
	
	// Create configuration for queue submission & synchronization.
	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	
	VkSemaphore wait_semaphores[]		= {vk->img_available_semaphore};
	VkPipelineStageFlags wait_stages[]	= {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = wait_semaphores;
	submit_info.pWaitDstStageMask = wait_stages;
	
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &vk->commandbuffer;
	
	VkSemaphore signal_semaphores[] = {vk->render_finish_semaphore};
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = signal_semaphores;
	
	
	VkResult result = vkQueueSubmit(vk->queues.graphics_queue, 1, &submit_info, vk->inflight_fence);
	if(result != VK_SUCCESS) {
		printf("ERROR: Vulkan draw command buffer submission failed with result code %i!\n", result);
		return result;
	}// else printf("Vulkan draw command buffer submitted!\n");
	
	
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
	vkQueuePresentKHR(vk->queues.present_queue, &present_info);
	
	
	return VK_SUCCESS;
}
void wsVulkanStop(wsVulkan* vk) {
	
	// Wait for all asynchronous elements to finish execution and return to an idle state before continuing on.
	vkDeviceWaitIdle(vk->logical_device);
	
	// If in debug mode, destroy debug messenger.
	if(debug) {
		wsVulkanStopDebugMessenger(vk);
	}
	
	// Destroy sync objects.
	vkDestroySemaphore(vk->logical_device, vk->img_available_semaphore, NULL);
	printf("Vulkan \"image available?\" semaphore destroyed!\n");
	vkDestroySemaphore(vk->logical_device, vk->render_finish_semaphore, NULL);
	printf("Vulkan \"render finished?\" semaphore destroyed!\n");
	vkDestroyFence(vk->logical_device, vk->inflight_fence, NULL);
	printf("Vulkan \"in flight?\" fence destroyed!\n");
	
	vkDestroyCommandPool(vk->logical_device, vk->commandpool, NULL);
	printf("Vulkan command pool destroyed!\n");
	
	// Destroy Graphics Pipeline & Pipeline Layout.
	vkDestroyPipeline(vk->logical_device, vk->pipeline, NULL);
	printf("Vulkan graphics pipeline destroyed!\n");
	vkDestroyPipelineLayout(vk->logical_device, vk->pipeline_layout, NULL);
	printf("Vulkan pipeline layout destroyed!\n");
	
	// Destroy individual framebuffers within swapchain.
	for(uint32_t i = 0; i < vk->swapchain.num_images; i++) {
		vkDestroyFramebuffer(vk->logical_device, vk->swapchain.framebuffers[i], NULL);
	}
	free(vk->swapchain.framebuffers);
	printf("Vulkan framebuffers destroyed!\n");
	
	vkDestroyRenderPass(vk->logical_device, vk->renderpass, NULL);
	printf("Vulkan render pass destroyed!\n");
	
	// Unload all shaders.
	wsShaderUnloadAll(&vk->shader);

	// Destroy swap chain image views.
	for(uint32_t i = 0; i < vk->swapchain.num_images; i++) {
		vkDestroyImageView(vk->logical_device, vk->swapchain.image_views[i], NULL);
	}
	free(vk->swapchain.image_views);
	printf("Vulkan image views destroyed!\n");

	// Destroy swap chain.
	vkDestroySwapchainKHR(vk->logical_device, vk->swapchain.sc, NULL);
	free(vk->swapchain.formats);
	free(vk->swapchain.present_modes);
	free(vk->swapchain.images);
	printf("Vulkan swap chain destroyed!\n");
	
	// Destroy logical device, surface, & instance!
	vkDestroyDevice(vk->logical_device, NULL);
	printf("Vulkan logical device destroyed!\n");
	vkDestroySurfaceKHR(vk->instance, vk->surface, NULL);
	printf("Vulkan surface destroyed!\n");
	vkDestroyInstance(vk->instance, NULL);
	printf("Vulkan instance destroyed!\n");
}

VkResult wsVulkanCreateSyncObjects(wsVulkan* vk) {
	
	VkSemaphoreCreateInfo semaphore_info = {};
	semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	
	VkFenceCreateInfo fence_info = {};
	fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;		// Prevents first call to wsVulkanDrawFrame() from hanging up on an unsignaled fence.
	
	
	VkResult result;
	
	result = vkCreateSemaphore(vk->logical_device, &semaphore_info, NULL, &vk->img_available_semaphore);
	if(result != VK_SUCCESS) {
		printf("ERROR: Vulkan \"image available?\" semaphore creation failed with result code %i!\n", result);
		return result;
	} else printf("Vulkan \"image available?\" semaphore created!\n");
	
	result = vkCreateSemaphore(vk->logical_device, &semaphore_info, NULL, &vk->render_finish_semaphore);
	if(result != VK_SUCCESS) {
		printf("ERROR: Vulkan \"render finished?\" semaphore creation failed with result code %i!\n", result);
		return result;
	} else printf("Vulkan \"render finished?\" semaphore created!\n");
	
	result = vkCreateFence(vk->logical_device, &fence_info, NULL, &vk->inflight_fence);
	if(result != VK_SUCCESS) {
		printf("ERROR: Vulkan \"in flight?\" fence creation failed with result code %i!\n", result);
		return result;
	} else printf("Vulkan \"in flight?\" fence created!\n");
	
	return VK_SUCCESS;
}
VkResult wsVulkanRecordCommandBuffer(wsVulkan* vk, uint32_t img_ndx) {
	
	// Begin recording to the command buffer!
	VkCommandBufferBeginInfo begin_info = {};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = 0;
	begin_info.pInheritanceInfo = NULL;
	
	VkResult result = vkBeginCommandBuffer(vk->commandbuffer, &begin_info);
	if(result != VK_SUCCESS) {
		printf("ERROR: Vulkan command buffer recording begin failed with result code %i!\n", result);
		return result;
	}// else printf("Vulkan command buffer recording has begun!\n");
	
	
	// Begin render pass.
	VkRenderPassBeginInfo renderpass_info = {};
	renderpass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderpass_info.renderPass = vk->renderpass;
	renderpass_info.framebuffer = vk->swapchain.framebuffers[img_ndx];
	
	renderpass_info.renderArea.offset.x = 0;
	renderpass_info.renderArea.offset.y = 0;
	renderpass_info.renderArea.extent = vk->swapchain.extent;
	
	VkClearValue clear_color = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
	renderpass_info.clearValueCount = 1;
	renderpass_info.pClearValues = &clear_color;
	
	vkCmdBeginRenderPass(vk->commandbuffer, &renderpass_info, VK_SUBPASS_CONTENTS_INLINE);
	
	
	// Bind graphics pipeline, then specify viewport & scissor states for this pipeline.
	vkCmdBindPipeline(vk->commandbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk->pipeline);
	
	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)vk->swapchain.extent.width;
	viewport.height = (float)vk->swapchain.extent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(vk->commandbuffer, 0, 1, &viewport);
	
	VkRect2D scissor = {};
	scissor.offset.x = 0;
	scissor.offset.y = 0;
	scissor.extent = vk->swapchain.extent;
	vkCmdSetScissor(vk->commandbuffer, 0, 1, &scissor);
	
	
	// !!!Draw triangle!!!
	vkCmdDraw(vk->commandbuffer, 3, 1, 0, 0);
	
	
	// End render pass.
	vkCmdEndRenderPass(vk->commandbuffer);
	
	// End command buffer recording.
	result = vkEndCommandBuffer(vk->commandbuffer);
	if(result != VK_SUCCESS) {
		printf("ERROR: Vulkan command buffer recording end failed with result code %i!\n", result);
	}// else printf("Vulkan command buffer recording has ended!\n");
	return result;
}
VkResult wsVulkanCreateCommandBuffer(wsVulkan* vk) {
	
	VkCommandBufferAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	alloc_info.commandPool = vk->commandpool;
	alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	alloc_info.commandBufferCount = 1;
	
	VkResult result = vkAllocateCommandBuffers(vk->logical_device, &alloc_info, &vk->commandbuffer);
	if(result != VK_SUCCESS) {
		printf("ERROR: Vulkan command buffer creation failed with result code %i!\n", result);
	} else printf("Vulkan command buffer created!\n");
	return result;
}
VkResult wsVulkanCreateCommandPool(wsVulkan* vk) {
	
	VkCommandPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	pool_info.queueFamilyIndex = vk->queues.ndx_graphics_family;
	
	VkResult result = vkCreateCommandPool(vk->logical_device, &pool_info, NULL, &vk->commandpool);
	if(result != VK_SUCCESS) {
		printf("ERROR: Vulkan command pool creation failed with result code %i!\n", result);
	} else printf("Vulkan command pool created!\n");
	return result;
}

VkResult wsVulkanCreateFrameBuffers(wsVulkan* vk) {
	
	VkResult result;
	vk->swapchain.framebuffers = malloc(vk->swapchain.num_images * sizeof(VkFramebuffer));
	
	for(int i = 0; i < vk->swapchain.num_images; i++) {
		VkImageView attachments[] = {vk->swapchain.image_views[i]};
		
		VkFramebufferCreateInfo framebuffer_info = {};
		framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebuffer_info.renderPass = vk->renderpass;
		framebuffer_info.attachmentCount = 1;
		framebuffer_info.pAttachments = attachments;
		framebuffer_info.width = vk->swapchain.extent.width;
		framebuffer_info.height = vk->swapchain.extent.height;
		framebuffer_info.layers = 1;
		
		result = vkCreateFramebuffer(vk->logical_device, &framebuffer_info, NULL, &vk->swapchain.framebuffers[i]);
		if(result != VK_SUCCESS) {
			printf("ERROR: Vulkan framebuffer creation %i/%i failed with result code %i!\n", i, vk->swapchain.num_images, result);
			break;
		}
	}
	
	printf("%i/%i Vulkan framebuffers created!\n", vk->swapchain.num_images, vk->swapchain.num_images);
	return result;
}

VkResult wsVulkanCreateRenderPass(wsVulkan* vk) {
	
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
	
	
	// Specify attachment reference for subpass(es).
	VkAttachmentReference colorattachment_reference = {};
	colorattachment_reference.attachment= 0;
	colorattachment_reference.layout	= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	
	VkSubpassDescription subpass_desc = {};
	subpass_desc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	
	subpass_desc.colorAttachmentCount = 1;
	subpass_desc.pColorAttachments = &colorattachment_reference;
	
	
	// Create render pass!
	VkRenderPassCreateInfo renderpass_info = {};
	renderpass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

	renderpass_info.attachmentCount = 1;
	renderpass_info.pAttachments = &color_attachment;
	
	renderpass_info.subpassCount= 1;
	renderpass_info.pSubpasses	= &subpass_desc;
	
	
	// Create render pass subpass dependency, so when we render we must wait for the finished swapchain image to render said image.
	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	
	renderpass_info.dependencyCount = 1;
	renderpass_info.pDependencies = &dependency;
	
	
	// Self-explanatory copypasta code.	
	VkResult result = vkCreateRenderPass(vk->logical_device, &renderpass_info, NULL, &vk->renderpass);
	if(result != VK_SUCCESS) {
		printf("ERROR: Vulkan render pass creation failed with result code %i!\n", result);
	} else printf("Vulkan render pass created!\n");
	return result;
}

VkShaderModule wsVulkanCreateShaderModule(wsVulkan* vk, uint8_t shaderID) {
	// Specify shader module creation info.
	VkShaderModuleCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	
	create_info.codeSize = vk->shader.shader_size[shaderID];
	create_info.pCode = (const uint32_t*)vk->shader.shader_data[shaderID];

	// Create and return shader module!
	VkShaderModule module;
	VkResult result = vkCreateShaderModule(vk->logical_device, &create_info, NULL, &module);
	if(result != VK_SUCCESS) {
		printf("ERROR: Shader module creation failed for shader ID %i with result code %i!\n", shaderID, result);
	} else printf("Shader module created for shader ID %i!\n", shaderID);
	return module;
}

VkResult wsVulkanCreateGraphicsPipeline(wsVulkan* vk) {

	// ---------------------
	// CREATE SHADER MODULES
	// ---------------------

	// Shader data container inside of struct vk.
	wsShaderInit(&vk->shader);
	
	// Load shader bytecode into memory.
	uint8_t vertID = wsShaderLoad(&vk->shader, "shaders/spir-v/hellotriangle_vert.spv");
	uint8_t fragID = wsShaderLoad(&vk->shader, "shaders/spir-v/hellotriangle_frag.spv");
	
	// Convert shader bytecode into a shader module for Vulkan to work with.
	VkShaderModule vert_module = wsVulkanCreateShaderModule(vk, vertID);
	VkShaderModule frag_module = wsVulkanCreateShaderModule(vk, fragID);
	
	
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
	
	vertexinput_info.vertexBindingDescriptionCount	= 0;
	vertexinput_info.pVertexBindingDescriptions		= NULL;
	
	vertexinput_info.vertexAttributeDescriptionCount= 0;
	vertexinput_info.pVertexAttributeDescriptions	= NULL;
	
	
	// Configure vertex data assembly method.
	VkPipelineInputAssemblyStateCreateInfo inputassembly_info = {};
	inputassembly_info.sType	= VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputassembly_info.topology	= VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;	// We are using triangles for our models here.
	inputassembly_info.primitiveRestartEnable = VK_FALSE;	// Allows for element buffers for splitting up geometry, reusing vertices, reusing indices, etc.
	
	/*
	// Configure viewport.
	VkViewport viewport;// Viewport for stretching & rendering.
	
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	
	viewport.width	= (float)vk->swapchain.extent.width;
	viewport.height	= (float)vk->swapchain.extent.height;
	
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	
	
	// Configure scissor window for viewport.
	VkRect2D scissor;	// Scissor rectangle for the viewport.  Defines region in which pixels are stored.
	
	scissor.offset.x = 0;
	scissor.offset.y = 0;
	
	scissor.extent = vk->swapchain.extent;*/
	
	
	// May cause crash when out of scope.  We'll see.
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
	rasterizer_info.frontFace = VK_FRONT_FACE_CLOCKWISE;// Specifies vertex order for faced to be considered front-facing.
	
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
	
	// Should we blend at all?
	colorblend_attachment.blendEnable	= VK_TRUE;
	
	// Color blending.
	colorblend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorblend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorblend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
	
	// ALpha blending.
	colorblend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorblend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
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
	
	
	// ---------------------
	// CREATE PIPELINE & LAYOUT.
	// ---------------------
	
	// Specify pipeline layout creation info.
	VkPipelineLayoutCreateInfo pipelinelayout_info = {};
	pipelinelayout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	
	pipelinelayout_info.setLayoutCount = 0;
	pipelinelayout_info.pSetLayouts	= NULL;
	
	pipelinelayout_info.pushConstantRangeCount = 0;
	pipelinelayout_info.pPushConstantRanges	= NULL;
	
	// Create pipeline layout!  If unsuccessful, say so and return the result code as an integer.
	VkResult result = vkCreatePipelineLayout(vk->logical_device, &pipelinelayout_info, NULL, &vk->pipeline_layout);
	if(result != VK_SUCCESS) {
		printf("ERROR: Vulkan pipeline layout creation failed with result code %i!\n", result);
		return result;
	} else printf("Vulkan pipeline layout created!\n");
	
	// Specify graphics pipeline creation info.
	VkGraphicsPipelineCreateInfo pipeline_info = {};
	pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	
	// Shader stages.
	pipeline_info.stageCount = 2;
	pipeline_info.pStages = &shader_stages[0];
	
	// Pointers to creation info structs.
	pipeline_info.pVertexInputState = &vertexinput_info;
	pipeline_info.pInputAssemblyState = &inputassembly_info;
	pipeline_info.pViewportState = &viewport_info;
	pipeline_info.pRasterizationState = &rasterizer_info;
	pipeline_info.pMultisampleState = &multisampling_info;
	pipeline_info.pDepthStencilState = NULL;
	pipeline_info.pColorBlendState = &colorblend_info;
	pipeline_info.pDynamicState = &dynamicstate_info;
	
	// Set layout.
	pipeline_info.layout = vk->pipeline_layout;
	
	// Set render pass storage location within vk struct.
	pipeline_info.renderPass = vk->renderpass;
	pipeline_info.subpass = 0;
	
	// Specify if we should "derive" from a parent pipeline for creation.
	pipeline_info.basePipelineHandle = (int)NULL;
	pipeline_info.basePipelineIndex = -1;
	
	result = vkCreateGraphicsPipelines(vk->logical_device, VK_NULL_HANDLE, 1, &pipeline_info, NULL, &vk->pipeline);
	
	// Destroy shader modules cause WE DON NEED EM NO MO.
	vkDestroyShaderModule(vk->logical_device, vert_module, NULL);
	vkDestroyShaderModule(vk->logical_device, frag_module, NULL);
	
	if(result != VK_SUCCESS) {
		printf("ERROR: Vulkan graphics pipeline creation failed with result code %i!\n", result);
	} else printf("Vulkan graphics pipeline created!\n");
	return result;
}

// Returns number of image views created successfully.
uint32_t wsVulkanCreateImageViews(wsVulkan* vk) {
	uint32_t num_created = 0;

	// Allocate space for our image views within struct vk.
	vk->swapchain.image_views = malloc(vk->swapchain.num_images * sizeof(VkImageView));

	// Create image view for each swap chain image.
	for(uint32_t i = 0; i < vk->swapchain.num_images; i++) {
		VkImageViewCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		create_info.image = vk->swapchain.images[i];
		
		create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		create_info.format = vk->swapchain.image_format;
		
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
		
		// Create image view for each image view object!
		VkResult result = vkCreateImageView(vk->logical_device, &create_info, NULL, &vk->swapchain.image_views[i]);
		if(result != VK_SUCCESS)
			printf("ERROR: Image view %i not created successfully; code %i!\n", i, result);
		else num_created++;
	}

	if(num_created != vk->swapchain.num_images) {
		printf("ERROR: Only %i/%i image views created!\n", num_created, vk->swapchain.num_images);
	} else printf("%i/%i Vulkan image views created!\n", num_created, vk->swapchain.num_images);
	return num_created;
}

VkResult wsVulkanCreateSwapChain(wsVulkan* vk) {
	// Initialize swap chain within struct vk.
	// wsVulkanQuerySwapChainSupport(vk);
	wsVulkanChooseSwapSurfaceFormat(&vk->swapchain);
	wsVulkanChooseSwapPresentMode(&vk->swapchain);
	wsVulkanChooseSwapExtent(vk);

	// Recommended to add at least 1 extra image to swap chain buffer to reduce wait times during rendering.
	uint32_t num_images = vk->swapchain.capabilities.minImageCount + 2;
	vk->swapchain.num_images = num_images;

	// Specify swap chain creation info.
	VkSwapchainCreateInfoKHR create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	create_info.surface = vk->surface;
	
	// Specify swapchain image view details.
	create_info.minImageCount = num_images;
	create_info.imageFormat = vk->swapchain.surface_format.format;
	create_info.imageColorSpace = vk->swapchain.surface_format.colorSpace;
	create_info.imageExtent = vk->swapchain.extent;
	create_info.imageArrayLayers = 1;	// Always 1 unless this is a stereoscopic 3D application.
	create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	
	
	// Check if queue family indices are unique.
	uint32_t queue_family_indices[] = {vk->queues.ndx_graphics_family, vk->queues.ndx_present_family};
	
	if(vk->queues.ndx_graphics_family != vk->queues.ndx_present_family) {
		create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		create_info.queueFamilyIndexCount = 2;
		create_info.pQueueFamilyIndices = queue_family_indices;
	
	} else {
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

	printf("Creating swap chain with properties: \n\tExtent: %ix%i\n\tSurface format: %i\n\tPresentation mode: %i\n", 
		vk->swapchain.extent.width, vk->swapchain.extent.height, vk->swapchain.surface_format.format, vk->swapchain.present_mode);
	
	if(result != VK_SUCCESS) {
		printf("ERROR: Vulkan swap chain creation failed with result code %i!\n", result);
	} else printf("Vulkan swap chain created!\n");
	return result;
}

// Choose a nice resolution to draw swap chain images at.
uint32_t clamp(uint32_t num, uint32_t min, uint32_t max) {const uint32_t t = num < min ? min : num;return t > max ? max : t;}
void wsVulkanChooseSwapExtent(wsVulkan* vk) {
	
	wsVulkanSwapChain* swapchain_info = &vk->swapchain;
	VkSurfaceCapabilitiesKHR* capabilities = &vk->swapchain.capabilities;
	
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
void wsVulkanChooseSwapPresentMode(wsVulkanSwapChain* swapchain_info) {
	
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
void wsVulkanChooseSwapSurfaceFormat(wsVulkanSwapChain* swapchain_info) {
	
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
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk->physical_device, vk->surface, &vk->swapchain.capabilities);

	// Query supported surface formats.
	uint32_t num_formats;
	
	vkGetPhysicalDeviceSurfaceFormatsKHR(vk->physical_device, vk->surface, &num_formats, NULL);
	vk->swapchain.num_formats = num_formats;

	if(num_formats != 0) {
		vk->swapchain.formats = malloc(num_formats * sizeof(VkSurfaceFormatKHR));
		vkGetPhysicalDeviceSurfaceFormatsKHR(vk->physical_device, vk->surface, &num_formats, vk->swapchain.formats);
	}

	// Query supported presentation modes.
	uint32_t num_present_modes;
	
	vkGetPhysicalDeviceSurfacePresentModesKHR(vk->physical_device, vk->surface, &num_present_modes, NULL);
	vk->swapchain.num_present_modes = num_present_modes;

	if(num_present_modes != 0) {
		vk->swapchain.present_modes = malloc(num_present_modes * sizeof(VkPresentModeKHR));
		vkGetPhysicalDeviceSurfacePresentModesKHR(vk->physical_device, vk->surface, &num_present_modes, vk->swapchain.present_modes);
	}
}

// Creates a surface bound to our GLFW window.
VkResult wsVulkanCreateSurface(wsVulkan* vk) {
	
	VkResult result = glfwCreateWindowSurface(vk->instance, wsWindowGetPtr(vk->windowID), NULL, &vk->surface);
	if(result != VK_SUCCESS) {
		printf("ERROR: Vulkan surface creation for window %i failed with result code %i!\n", vk->windowID, result);
	} else printf("Vulkan surface created for window %i!\n", vk->windowID);
	
	return result;
}

// Creates a logical device to interface with the physical one.
VkResult wsVulkanCreateLogicalDevice(wsVulkan* vk, uint32_t num_validation_layers, const char* const* validation_layers) {
	
	// Get required queue family indices and store them in vk.
	wsVulkanFindQueueFamilies(&vk->queues, &vk->physical_device, &vk->surface);
	
	// Figure out how many unique queue families we need to initialize.
	uint8_t num_unique_queue_families;
	if(vk->queues.ndx_graphics_family == vk->queues.ndx_present_family)
		num_unique_queue_families = 1;
	else num_unique_queue_families = 2;
	
	// Store queue family creation infos for later use in binding to logical device creation info.
	uint32_t unique_queue_family_indices[2] = {vk->queues.ndx_graphics_family, vk->queues.ndx_present_family};
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
	vkGetDeviceQueue(vk->logical_device, vk->queues.ndx_graphics_family, 0, &vk->queues.graphics_queue);
	vkGetDeviceQueue(vk->logical_device, vk->queues.ndx_present_family, 0, &vk->queues.present_queue);
	
	printf("%i unique Vulkan queue families exist; indices are as specified:\n", num_unique_queue_families);
	printf("\tGraphics: %i\n\tPresentation: %i\n", vk->queues.ndx_graphics_family, vk->queues.ndx_present_family);
	
	// FREE MEMORY!!!
	free(queue_create_infos);
	
	
	if(result != VK_SUCCESS) {
		printf("ERROR: Vulkan logical device creation failed with result code %i!\n", result);
	} else printf("Vulkan logical device created!\n");
	return result;
}

// Checks if all queue families have been found.
bool wsVulkanHasFoundAllQueueFamilies(wsVulkanQueueFamilies* indices) {
	if(!indices->has_graphics_family)
		return false;
	if(!indices->has_present_family)
		return false;
	
	return true;
}

// Finds queue families and stores their indices in *indices.
void wsVulkanFindQueueFamilies(wsVulkanQueueFamilies *indices, VkPhysicalDevice* physical_device, VkSurfaceKHR* surface) {
	
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
			indices->ndx_graphics_family = i;
			indices->has_graphics_family = true;
		}
		
		// Check for presentation queue family support.
		VkBool32 has_present_support = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(*physical_device, i, *surface, &has_present_support);
		if(has_present_support) {
			indices->ndx_present_family = i;
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
		
		printf("Found GPU with proper Vulkan support!\n");
		return true;
		
	} else {
		printf("ERROR: Failed to find GPUs with proper Vulkan support!\n");
		return false;
	}
	
	// Don't free GPU list so that we can allow user to swap GPUs in settings later.
}

// Verify if physical device is suitable for our use-cases and score it based on functionality.
int32_t wsVulkanRatePhysicalDevice(wsVulkan* vk, VkPhysicalDevice* physical_device) {
	enum GPU_INCOMPATIBILITY_CODES {NO_GEOMETRY_SHADER = INT_MIN, NO_GRAPHICS_FAMILY, NO_PRESENTATION_FAMILY, NO_DEVICE_EXTENSION_SUPPORT, NO_SWAPCHAIN_SUPPORT};
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
	
	
	// Program cannot function without certain queue families.
	wsVulkanQueueFamilies indices;
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
	if(vk->swapchain.num_formats <= 0 || vk->swapchain.num_present_modes <= 0)
		return NO_SWAPCHAIN_SUPPORT;
	
	
	// If these are the same queue family, this could increase performance.
	score += indices.ndx_graphics_family == indices.ndx_present_family ? 500 : 0;
	
	// Prefer discrete GPU.  If the max texture size is much lower on the discrete card, it might be too old and suck.
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
	VkDebugUtilsMessengerCreateInfoEXT create_info = {};
	wsVulkanPopulateDebugMessengerCreationInfo(&create_info);
	
	// Create debug messenger!
	VkResult result = wsVulkanCreateDebugUtilsMessengerEXT(vk->instance, &create_info, NULL, &vk->debug_messenger);
	if(result != VK_SUCCESS) {
			printf("ERROR: Vulkan debug messenger creation failed with result code %i!\n", result);
	} else printf("Vulkan debug messenger created!\n");
	
	return result;
}

void wsVulkanStopDebugMessenger(wsVulkan* vk) {
	wsVulkanDestroyDebugUtilsMessengerEXT(vk->instance, vk->debug_messenger, NULL);
	printf("Vulkan debug messenger destroyed!\n");
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
	
	
	if(!has_all_extensions) {
		printf("\tERROR: Not all GLFW-required Vulkan extensions are supported!\n");
	} else printf("\tAll GLFW-required Vulkan extensions are supported!\n");
	
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
	
	// Search for each required layer in our list available_layers.
	for(int32_t i = 0; i < num_required_layers; i++) {
		bool layer_found = false;
		
		for(int32_t j = 0; j < num_available_layers; j++) {
			if(strcmp(required_layers[i], available_layers[j].layerName) == 0) {
				
				layer_found = true;
				break;
			}
		}
		
		if(!layer_found) {
			printf("\tERROR: required Vulkan validation layers NOT supported!\n");
			return false;
		}
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
	
	printf("\tRequired Vulkan validation layers are supported!\n");
	return true;
}