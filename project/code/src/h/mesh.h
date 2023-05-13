#ifndef MESH_H_
#define MESH_H_

#include<stdbool.h>
#include<vulkan/vulkan.h>
#include<CGLM/cglm.h>

#define WS_MESH_MAX_MESHES 100
#define WS_MESH_MAX_ATTRIBUTE_DESCRIPTIONS 3


typedef struct wsVertex
{
    vec3 position;
    vec3 color;
	vec2 texcoord;
    
} wsVertex;

enum WS_MESH_MESHID_STATES { WS_MESH_UNLOADED = 0, WS_MESH_LOADED = 1 };
typedef struct wsMesh {
	uint8_t num_active_meshes;
    uint8_t isloaded[WS_MESH_MAX_MESHES];
    wsVertex* vertices[WS_MESH_MAX_MESHES];
    uint32_t num_vertices[WS_MESH_MAX_MESHES];
	uint16_t* indices[WS_MESH_MAX_MESHES];
	uint32_t num_indices[WS_MESH_MAX_MESHES];
	
	uint8_t teximg_ndx[WS_MESH_MAX_MESHES];
    
    VkVertexInputBindingDescription binding_descs[WS_MESH_MAX_MESHES];
    VkVertexInputAttributeDescription* attribute_descs[WS_MESH_MAX_MESHES];
    uint8_t num_attribute_descs[WS_MESH_MAX_MESHES];
    
} wsMesh;


void wsMeshInit(wsMesh* mesh_data);
uint8_t wsMeshCreate();
void wsMeshStop();

uint32_t wsMeshGetCurrentVertexBufferSize();
uint32_t wsMeshGetCurrentIndexBufferSize();
wsVertex* wsMeshGetVerticesPtr();
uint16_t* wsMeshGetIndicesPtr();
void wsMeshPrintMeshData(uint8_t meshID);

uint8_t wsMeshGetNumAttributeDescriptions(uint8_t meshID);
VkVertexInputAttributeDescription* wsMeshGetAttributeDescriptions(uint8_t meshID);
VkVertexInputBindingDescription* wsMeshGetBindingDescription(uint8_t meshID);

#endif