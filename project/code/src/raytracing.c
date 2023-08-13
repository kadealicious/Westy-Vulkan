#include<stdio.h>
#include<stdint.h>
#include<stdbool.h>
#include<string.h>
#include"h/raytracing.h"
#include"h/graphics.h"


wsRayTracing* rt;

VkResult wsRayTracingInit(wsRayTracing* rayTracingData)
{
	rt = rayTracingData;

	VkPhysicalDeviceProperties2 deviceProperties = {};
	deviceProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
	deviceProperties.pNext = &rt->rayTracingPipelineProperties;

	// TODO: Add RT builder here.  Not sure what that does.

	VkResult result;
	return result;
}

VkResult wsRayTracingGenerateBLAS(wsMesh* mesh)
{
	// TODO.  Pick up on pages 6 and 9 for a refresher.
}

bool wsRayTracingCheckDeviceExtensionSupport(VkPhysicalDevice* physicalDevice)
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
			rayTracingExtensionsSupported = false;
		}
	}
	
	if(rayTracingExtensionsSupported)
		{printf("\t\tAll device-specific ray tracing extensions are supported by your GPU!\n");}
	
	free(availableExtensions);	// Bug: Maybe a memory issue?
	
	return rayTracingExtensionsSupported;
}