#ifndef VULKAN_INTERFACE_H_
#define VULKAN_INTERFACE_H_

#include<stdbool.h>
#include <vulkan/vulkan.h>

// Vulkan interfacing functions.
void wsVulkanInit(VkInstance* instance, VkSurfaceKHR* surface, uint8_t windowID, VkPhysicalDevice* physical_device, 
	VkDevice* logical_device, VkDebugUtilsMessengerEXT* debug_messenger);
void wsVulkanStop(VkInstance* instance, VkDevice* logical_device, VkDebugUtilsMessengerEXT* debug_messenger);

// Self-explanatory.
void wsVulkanSetDebug(uint8_t debug_mode);

#endif