#ifndef RAYTRACING_H_
#define RAYTRACING_H_

#include<stdbool.h>
#include<vulkan/vulkan.h>
#include"mesh.h"


typedef struct wsRayTracing
{
	bool supportsRayTracing;

	VkPhysicalDeviceAccelerationStructureFeaturesKHR	accelerationStructionFeatures;
	VkPhysicalDeviceRayTracingPipelineFeaturesKHR		rayTracingPipelineFeatures;
	VkPhysicalDeviceRayTracingPipelinePropertiesKHR		rayTracingPipelineProperties;
}
wsRayTracing;

VkResult wsRayTracingInit(wsRayTracing* rayTracingData);
VkResult wsRayTracingGenerateBLAS(wsMesh* mesh);
bool wsRayTracingCheckDeviceExtensionSupport(VkPhysicalDevice* physicalDevice);


#endif