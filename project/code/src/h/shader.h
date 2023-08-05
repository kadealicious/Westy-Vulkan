#ifndef LOAD_SHADER_H_
#define LOAD_SHADER_H_

#include<stdint.h>
#include<stdbool.h>
#include<CGLM/cglm.h>

#include<vulkan/vulkan.h>

#define NUM_MAX_SHADERS 100
#define WS_MAX_DESCRIPTOR_SETS 4	// Minimum guaranteed descriptor set count.

typedef struct wsShader
{
    bool isloaded[NUM_MAX_SHADERS];             // Says whether or not shader is loaded into memory.
    char* shader_data[NUM_MAX_SHADERS];         // Holds bytecode shader data for all shaders.
    uint32_t shader_size[NUM_MAX_SHADERS];      // Stores size of each shader in bytes.
	
	VkDescriptorPool		descriptorPool;
	VkDescriptorSet*		descriptorSets;
	VkDescriptorSetLayout	descriptorSetLayout;

} wsShader;

void wsShaderInit(wsShader* shd);
uint8_t wsShaderLoad(wsShader* shd, const char* path);
void wsShaderUnloadAll(wsShader* shd);
void wsShaderUnload(wsShader* shd, uint8_t shaderID);

#endif