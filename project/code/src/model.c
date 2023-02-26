#include<stdio.h>
#include<stdint.h>
#include"h/model.h"


// Contains all data for transfer to shaders for all models!
wsModel* md;


void wsModelSetBindingDescription(uint8_t modelID);
void wsModelSetAttributeDescriptions(uint8_t modelID);


void wsModelInit(wsModel* model_data) {
    
    md = model_data;
    
    
    // Create test triangle!
    uint8_t modelID = 0;
    
    // Load vertices.
    uint8_t num_vertices = 3;
    md->vertices[modelID] = malloc(num_vertices * sizeof(wsVertex));
    md->vertices[modelID] = (wsVertex[3]) {
        {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
    };
    
    // Configure vertex binding descriptions, then the more fundamental attribute descriptions.
    uint8_t num_attributes = WS_MODEL_MAX_ATTRIBUTE_DESCRIPTIONS;
    md->num_attribute_descs[modelID] = num_attributes;
    md->attribute_descs[modelID] = malloc(num_attributes * sizeof(VkVertexInputAttributeDescription));
    wsModelSetBindingDescription(modelID);
    wsModelSetAttributeDescriptions(modelID);
    
    md->isloaded[modelID] = true;
    printf("INFO: Model w/ ID %i initialized!\n", modelID);
    
    
    printf("INFO: Models initialized!\n");
}
void wsModelStop() {
    
    for(uint8_t i = 0; i < WS_MODEL_MAX_MODELS; i++) {
        if(md->isloaded[i]) {
            
            free(md->vertices[i]);
            free(md->attribute_descs[i]);
            
            md->num_attribute_descs[i] = 0;
            md->isloaded[i] = false;
        }
    }
    
    printf("INFO: Models destroyed!\n");
}

void wsModelSetAttributeDescriptions(uint8_t modelID) {
    
    md->attribute_descs[modelID][0].binding = 0;
    md->attribute_descs[modelID][0].location = 0;
    md->attribute_descs[modelID][0].format = VK_FORMAT_R32G32_SFLOAT;
    md->attribute_descs[modelID][0].offset = offsetof(wsVertex, position);
    md->attribute_descs[modelID][1].binding = 0;
    md->attribute_descs[modelID][1].location = 1;
    md->attribute_descs[modelID][1].format = VK_FORMAT_R32G32B32_SFLOAT;
    md->attribute_descs[modelID][1].offset = offsetof(wsVertex, color);
}
void wsModelSetBindingDescription(uint8_t modelID) {
    
    md->binding_descs[modelID].binding = 0;
    md->binding_descs[modelID].stride = sizeof(wsVertex);
    md->binding_descs[modelID].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
}

uint8_t wsModelGetNumAttributeDescriptions(uint8_t modelID) {
    return md->num_attribute_descs[modelID];
}
VkVertexInputAttributeDescription* wsModelGetAttributeDescriptions(uint8_t modelID) {
    return &md->attribute_descs[modelID][0];
}
VkVertexInputBindingDescription* wsModelGetBindingDescription(uint8_t modelID) {
    return &md->binding_descs[modelID];
}