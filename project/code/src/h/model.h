#ifndef MODEL_H_
#define MODEL_H_

#include<stdbool.h>
#include<vulkan/vulkan.h>
#include<CGLM/cglm.h>

#define WS_MODEL_MAX_MODELS 100
#define WS_MODEL_MAX_ATTRIBUTE_DESCRIPTIONS 2


typedef struct wsVertex {
    vec2 position;
    vec3 color;
    
} wsVertex;

typedef struct wsModel {
    bool isloaded[WS_MODEL_MAX_MODELS];
    wsVertex* vertices[WS_MODEL_MAX_MODELS];
    
    VkVertexInputBindingDescription binding_descs[WS_MODEL_MAX_MODELS];
    VkVertexInputAttributeDescription* attribute_descs[WS_MODEL_MAX_MODELS];
    uint8_t num_attribute_descs[WS_MODEL_MAX_MODELS];
    
} wsModel;


void wsModelInit(wsModel* model_data);
void wsModelStop();

uint8_t wsModelGetNumAttributeDescriptions(uint8_t modelID);
VkVertexInputAttributeDescription* wsModelGetAttributeDescriptions(uint8_t modelID);
VkVertexInputBindingDescription* wsModelGetBindingDescription(uint8_t modelID);

#endif