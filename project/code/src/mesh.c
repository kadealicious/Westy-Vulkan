#include<stdio.h>
#include<stdint.h>

#define CGLTF_IMPLEMENTATION
#include"h/cgltf.h"

#include"h/mesh.h"
// #include"h/graphics.h"

typedef struct wsMeshManager
{
	wsMesh fallbackMesh;
}
wsMeshMan;
wsMeshMan meshMan;

wsMesh* wsMeshInit()
{
	/* This must be done before much of the Vulkan initialization code, as 
		we need the attribute & binding descriptions to create an initial 
		descriptor set. */
	wsMeshCreate("models/vikingroom.glb",  &meshMan.fallbackMesh);
	return &meshMan.fallbackMesh;
}

void wsMeshCreate(const char* meshPath, wsMesh* mesh)
{
	// Load model from path.
	cgltf_options options = {0};
	cgltf_data* data = NULL;
	cgltf_result result = cgltf_parse_file(&options, meshPath, &data);
	if(result == cgltf_result_success)
	{
		printf("INFO: Mesh \"%s\" loaded successfully!\n", meshPath);
		cgltf_free(data);
	}
	else
	{
		printf("INFO: Mesh \"%s\" load failed w/ error code %i!\n", meshPath, result);
		mesh = &meshMan.fallbackMesh;
	}
	
	// Don't do or look at this.  Ever.
	bool writesBadCode = true;
	if(writesBadCode)
	{
		// Load vertices.
		mesh->num_vertices = 8;
		mesh->vertices = (wsVertex*)malloc(mesh->num_vertices * sizeof(wsVertex));
		
		mesh->vertices[0].position[0] = -0.5f;
		mesh->vertices[0].position[1] = -0.5f;
		mesh->vertices[0].position[2] = 0.0f;
		mesh->vertices[0].color[0] = 1.0f;
		mesh->vertices[0].color[1] = 0.0f;
		mesh->vertices[0].color[2] = 0.0f;
		mesh->vertices[0].texcoord[0] = 1.0f;
		mesh->vertices[0].texcoord[1] = 0.0f;
		
		mesh->vertices[1].position[0] = 0.5f;
		mesh->vertices[1].position[1] = -0.5f;
		mesh->vertices[1].position[2] = 0.0f;
		mesh->vertices[1].color[0] = 0.0f;
		mesh->vertices[1].color[1] = 1.0f;
		mesh->vertices[1].color[2] = 0.0f;
		mesh->vertices[1].texcoord[0] = 0.0f;
		mesh->vertices[1].texcoord[1] = 0.0f;
		
		mesh->vertices[2].position[0] = 0.5f;
		mesh->vertices[2].position[1] = 0.5f;
		mesh->vertices[2].position[2] = 0.0f;
		mesh->vertices[2].color[0] = 0.0f;
		mesh->vertices[2].color[1] = 0.0f;
		mesh->vertices[2].color[2] = 1.0f;
		mesh->vertices[2].texcoord[0] = 0.0f;
		mesh->vertices[2].texcoord[1] = 1.0f;
		
		mesh->vertices[3].position[0] = -0.5f;
		mesh->vertices[3].position[1] = 0.5f;
		mesh->vertices[3].position[2] = 0.0f;
		mesh->vertices[3].color[0] = 1.0f;
		mesh->vertices[3].color[1] = 1.0f;
		mesh->vertices[3].color[2] = 1.0f;
		mesh->vertices[3].texcoord[0] = 1.0f;
		mesh->vertices[3].texcoord[1] = 1.0f;
		
		
		// Lol
		mesh->vertices[4].position[0] = -0.5f;
		mesh->vertices[4].position[1] = -0.5f;
		mesh->vertices[4].position[2] = -0.5f;
		mesh->vertices[4].color[0] = 1.0f;
		mesh->vertices[4].color[1] = 0.0f;
		mesh->vertices[4].color[2] = 0.0f;
		mesh->vertices[4].texcoord[0] = 1.0f;
		mesh->vertices[4].texcoord[1] = 0.0f;
		
		mesh->vertices[5].position[0] = 0.5f;
		mesh->vertices[5].position[1] = -0.5f;
		mesh->vertices[5].position[2] = -0.5f;
		mesh->vertices[5].color[0] = 0.0f;
		mesh->vertices[5].color[1] = 1.0f;
		mesh->vertices[5].color[2] = 0.0f;
		mesh->vertices[5].texcoord[0] = 0.0f;
		mesh->vertices[5].texcoord[1] = 0.0f;
		
		mesh->vertices[6].position[0] = 0.5f;
		mesh->vertices[6].position[1] = 0.5f;
		mesh->vertices[6].position[2] = -0.5f;
		mesh->vertices[6].color[0] = 0.0f;
		mesh->vertices[6].color[1] = 0.0f;
		mesh->vertices[6].color[2] = 1.0f;
		mesh->vertices[6].texcoord[0] = 0.0f;
		mesh->vertices[6].texcoord[1] = 1.0f;
		
		mesh->vertices[7].position[0] = -0.5f;
		mesh->vertices[7].position[1] = 0.5f;
		mesh->vertices[7].position[2] = -0.5f;
		mesh->vertices[7].color[0] = 1.0f;
		mesh->vertices[7].color[1] = 1.0f;
		mesh->vertices[7].color[2] = 1.0f;
		mesh->vertices[7].texcoord[0] = 1.0f;
		mesh->vertices[7].texcoord[1] = 1.0f;
		
		
		// Load indices.  Jesus Christ.
		mesh->num_indices = 12;
		mesh->indices = (uint32_t*)malloc(mesh->num_indices * sizeof(uint32_t));
		
		mesh->indices[0] = 0;
		mesh->indices[1] = 1;
		mesh->indices[2] = 2;
		mesh->indices[3] = 2;
		mesh->indices[4] = 3;
		mesh->indices[5] = 0;
		
		// oMg
		mesh->indices[6] = 4;
		mesh->indices[7] = 5;
		mesh->indices[8] = 6;
		mesh->indices[9] = 6;
		mesh->indices[10] = 7;
		mesh->indices[11] = 4;
	}
	
	
    // Configure vertex binding descriptions, then the more fundamental attribute descriptions.
    uint8_t num_attributes = WS_MESH_MAX_ATTRIBUTE_DESCRIPTIONS;
    mesh->num_attribute_descs = num_attributes;
    mesh->attribute_descs = malloc(num_attributes * sizeof(VkVertexInputAttributeDescription));
    wsMeshSetBindingDescription(mesh);
    wsMeshSetAttributeDescriptions(mesh);
	
	// wsVulkanCreateVertexBuffer(mesh);// Creates vertex buffers which hold our vertex input data.
	// wsVulkanCreateIndexBuffer(mesh);
	
	// wsMeshPrintMeshData(meshID);
	
    printf("INFO: Mesh %p initialized!\n", mesh);
}

void wsMeshDestroy(wsMesh* mesh, VkDevice* logicalDevice)
{
	free(mesh->vertices);
	free(mesh->indices);
	free(mesh->attribute_descs);
	
	vkDestroyBuffer(*logicalDevice, mesh->vertexBuffer, NULL);
	vkFreeMemory(*logicalDevice, mesh->vertexBufferMemory, NULL);
	vkDestroyBuffer(*logicalDevice, mesh->indexBuffer, NULL);
	vkFreeMemory(*logicalDevice, mesh->indexBufferMemory, NULL);
}

void wsMeshSetAttributeDescriptions(wsMesh* mesh)
{
    mesh->attribute_descs[0].binding = 0;
    mesh->attribute_descs[0].location = 0;
    mesh->attribute_descs[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    mesh->attribute_descs[0].offset = offsetof(wsVertex, position);
	
    mesh->attribute_descs[1].binding = 0;
    mesh->attribute_descs[1].location = 1;
    mesh->attribute_descs[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    mesh->attribute_descs[1].offset = offsetof(wsVertex, color);
	
	mesh->attribute_descs[2].binding = 0;
    mesh->attribute_descs[2].location = 2;
    mesh->attribute_descs[2].format = VK_FORMAT_R32G32_SFLOAT;
    mesh->attribute_descs[2].offset = offsetof(wsVertex, texcoord);
}
void wsMeshSetBindingDescription(wsMesh* mesh)
{
    mesh->binding_descs.binding = 0;
    mesh->binding_descs.stride = sizeof(wsVertex);
    mesh->binding_descs.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
}

uint32_t wsMeshGetCurrentVertexBufferSize(wsMesh* mesh)
	{ return (mesh->num_vertices * sizeof(wsVertex)); }

uint32_t wsMeshGetCurrentIndexBufferSize(wsMesh* mesh)
	{ return (mesh->num_indices * sizeof(mesh->indices[0])); }
