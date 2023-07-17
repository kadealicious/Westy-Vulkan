#ifndef MESH_H_
#define MESH_H_

#include <vulkan/vulkan.h>
#include<CGLM/cglm.h>

#define WS_MESH_MAX_MESHES 100
#define WS_MESH_MAX_ATTRIBUTE_DESCRIPTIONS 3

typedef struct wsVertex
{
    vec3 position;
    vec3 color;
	vec2 texcoord;
}
wsVertex;

typedef struct wsMesh
{
    wsVertex* vertices;
    uint32_t num_vertices;
	uint32_t* indices;
	uint32_t num_indices;
    
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;
	
    VkVertexInputBindingDescription binding_descs;
    VkVertexInputAttributeDescription* attribute_descs;
    uint8_t num_attribute_descs;
}
wsMesh;

wsMesh* wsMeshInit();	// Returns a pointer to the fallback mesh.
void wsMeshCreate(const char* meshPath, wsMesh* mesh);
void wsMeshDestroy(wsMesh* mesh, VkDevice* logicalDevice);
void wsMeshSetAttributeDescriptions(wsMesh* mesh);
void wsMeshSetBindingDescription(wsMesh* mesh);
uint32_t wsMeshGetCurrentVertexBufferSize(wsMesh* mesh);
uint32_t wsMeshGetCurrentIndexBufferSize(wsMesh* mesh);

#endif