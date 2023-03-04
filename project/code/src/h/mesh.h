#ifndef MESH_H_
#define MESH_H_

#include<stdbool.h>
#include<vulkan/vulkan.h>
#include<CGLM/cglm.h>

#define WS_MESH_MAX_MESHS 100
#define WS_MESH_MAX_ATTRIBUTE_DESCRIPTIONS 2


typedef struct wsVertex {
    vec2 position;
    vec3 color;
    
} wsVertex;

enum WS_MESH_MESHID_STATES { WS_MESH_UNLOADED = 0, WS_MESH_LOADED = 1 };
typedef struct wsMesh {
	uint8_t num_active_meshes;
    uint8_t isloaded[WS_MESH_MAX_MESHS];
    wsVertex* vertices[WS_MESH_MAX_MESHS];
    uint32_t num_vertices[WS_MESH_MAX_MESHS];
    
    VkVertexInputBindingDescription binding_descs[WS_MESH_MAX_MESHS];
    VkVertexInputAttributeDescription* attribute_descs[WS_MESH_MAX_MESHS];
    uint8_t num_attribute_descs[WS_MESH_MAX_MESHS];
    
} wsMesh;


void wsMeshInit(wsMesh* mesh_data);
void wsMeshStop();

uint32_t wsMeshGetCurrentBufferSize();
wsVertex* wsMeshGetVerticesPtr();
void wsMeshPrintMeshData(uint8_t meshID);

uint8_t wsMeshGetNumAttributeDescriptions(uint8_t meshID);
VkVertexInputAttributeDescription* wsMeshGetAttributeDescriptions(uint8_t meshID);
VkVertexInputBindingDescription* wsMeshGetBindingDescription(uint8_t meshID);

#endif