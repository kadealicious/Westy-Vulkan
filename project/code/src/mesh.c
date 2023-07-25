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

cgltf_data* wsMeshLoadGLTF(const char* path, cgltf_options options);	// data must be freed later using: cglft_free(data);

wsMesh* wsMeshInit()
{
	/* This must be done before much of the Vulkan initialization code, as 
		we need the attribute & binding descriptions to create an initial 
		descriptor set. */
	wsMeshCreate("models/cube.glb",  &meshMan.fallbackMesh);
	return &meshMan.fallbackMesh;
}

void wsMeshCreate(const char* meshPath, wsMesh* mesh)
{
	printf("\n");
	
	// Load model from path.
	cgltf_data* data = wsMeshLoadGLTF(meshPath, (cgltf_options){0});
	if(data == NULL)
		{ mesh = &meshMan.fallbackMesh; return; }
	
	struct cgltf_mesh* currentMesh = &data->meshes[0];
	
	printf("Mesh's buffer view count: %llu\n", data->buffer_views_count);
	printf("Loading mesh primitives into memory: \n");
	
	for(uint32_t i = 0; i < currentMesh->primitives_count; i++)
	{
		cgltf_primitive* currentPrimitive = &currentMesh->primitives[i];
		uint8_t positionIndex, texcoordIndex, normalIndex;
		
		printf("Mesh's accessor indices count: %llu\n", currentPrimitive->indices->count);
		printf("Mesh's primitive %i attribute accessor indices: \n", i);
		
		for(uint8_t j = 0; j < currentPrimitive->attributes_count; j++)
		{
			if(strcmp(currentPrimitive->attributes[j].name, "POSITION") == 0)
				positionIndex = j;
			else if(strcmp(currentPrimitive->attributes[j].name, "TEXCOORD_0") == 0)
				texcoordIndex = j;
			else if(strcmp(currentPrimitive->attributes[j].name, "NORMAL") == 0)
				normalIndex = j;
			
			printf("\t%s: index %i, type %i\n", currentPrimitive->attributes[j].name, currentPrimitive->attributes[j].index, currentPrimitive->attributes[j].type);
		}
		
		mesh->num_vertices = currentPrimitive->attributes->data[positionIndex].count;
		mesh->num_indices = currentPrimitive->indices->count;
		printf("\t%i vertices, %i indices, %i triangles\n", mesh->num_vertices, mesh->num_indices, (mesh->num_indices / 3));
		
		mesh->vertices = (wsVertex*)malloc(mesh->num_vertices * sizeof(wsVertex));
		mesh->indices = (uint32_t*)malloc(mesh->num_indices * sizeof(uint32_t));
		
		cgltf_accessor* accessors = data->accessors;
		cgltf_accessor* positionAccessor	= &accessors[positionIndex];
		cgltf_accessor* texcoordAccessor	= &accessors[texcoordIndex];
		cgltf_accessor* normalAccessor		= &accessors[normalIndex];
		cgltf_buffer_view* positionBufferView	= positionAccessor->buffer_view;
		cgltf_buffer_view* texcoordBufferView	= texcoordAccessor->buffer_view;
		cgltf_buffer_view* normalBufferView		= normalAccessor->buffer_view;
		cgltf_buffer* positionBuffer	= positionBufferView->buffer;
		cgltf_buffer* texcoordBuffer	= texcoordBufferView->buffer;
		cgltf_buffer* normalBuffer		= normalBufferView->buffer;
		vec3* positions = (vec3*)(&positionBuffer[positionBufferView->offset + positionAccessor->offset]);
		vec3* normals = (vec3*)(&normalBuffer[normalBufferView->offset + normalAccessor->offset]);
		vec2* texcoords = (vec2*)(&texcoordBuffer[texcoordBufferView->offset + texcoordAccessor->offset]);
		
		printf("Mesh's buffers' data: \n");
		for(uint32_t j = 0; j < mesh->num_vertices; j++)
		{
			vec3 currentPosition	= {positions[j * 3][0], positions[j * 3][1], positions[j * 3][2]};
			vec3 currentNormal		= {normals[j * 3][0], normals[j * 3][1], normals[j * 3][2]};
			vec2 currentTexcoord	= {texcoords[j * 2][0], texcoords[j * 2][1]};
			
			printf("\tPosition\t%i: (%.2f, %.2f, %.2f)\n", j, currentPosition[0], currentPosition[1], currentPosition[2]);
			printf("\tNormal\t%i: (%.2f, %.2f, %.2f)\n", j, currentNormal[0], currentNormal[1], currentNormal[2]);
			printf("\tTexcoord\t%i: (%.2f, %.2f)\n", j, currentTexcoord[0], currentTexcoord[1]);
			
			// mesh->vertices[j].position[0] = positionData;
		}
	}
	
	printf("INFO: Mesh \"%s\" loaded successfully!\n", meshPath);
	cgltf_free(data);
	
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
		mesh->vertices[0].normal[0] = 1.0f;
		mesh->vertices[0].normal[1] = 0.0f;
		mesh->vertices[0].normal[2] = 0.0f;
		mesh->vertices[0].texcoord[0] = 1.0f;
		mesh->vertices[0].texcoord[1] = 0.0f;
		
		mesh->vertices[1].position[0] = 0.5f;
		mesh->vertices[1].position[1] = -0.5f;
		mesh->vertices[1].position[2] = 0.0f;
		mesh->vertices[1].normal[0] = 0.0f;
		mesh->vertices[1].normal[1] = 1.0f;
		mesh->vertices[1].normal[2] = 0.0f;
		mesh->vertices[1].texcoord[0] = 0.0f;
		mesh->vertices[1].texcoord[1] = 0.0f;
		
		mesh->vertices[2].position[0] = 0.5f;
		mesh->vertices[2].position[1] = 0.5f;
		mesh->vertices[2].position[2] = 0.0f;
		mesh->vertices[2].normal[0] = 0.0f;
		mesh->vertices[2].normal[1] = 0.0f;
		mesh->vertices[2].normal[2] = 1.0f;
		mesh->vertices[2].texcoord[0] = 0.0f;
		mesh->vertices[2].texcoord[1] = 1.0f;
		
		mesh->vertices[3].position[0] = -0.5f;
		mesh->vertices[3].position[1] = 0.5f;
		mesh->vertices[3].position[2] = 0.0f;
		mesh->vertices[3].normal[0] = 1.0f;
		mesh->vertices[3].normal[1] = 1.0f;
		mesh->vertices[3].normal[2] = 1.0f;
		mesh->vertices[3].texcoord[0] = 1.0f;
		mesh->vertices[3].texcoord[1] = 1.0f;
		
		
		// Lol
		mesh->vertices[4].position[0] = -0.5f;
		mesh->vertices[4].position[1] = -0.5f;
		mesh->vertices[4].position[2] = -0.5f;
		mesh->vertices[4].normal[0] = 1.0f;
		mesh->vertices[4].normal[1] = 0.0f;
		mesh->vertices[4].normal[2] = 0.0f;
		mesh->vertices[4].texcoord[0] = 1.0f;
		mesh->vertices[4].texcoord[1] = 0.0f;
		
		mesh->vertices[5].position[0] = 0.5f;
		mesh->vertices[5].position[1] = -0.5f;
		mesh->vertices[5].position[2] = -0.5f;
		mesh->vertices[5].normal[0] = 0.0f;
		mesh->vertices[5].normal[1] = 1.0f;
		mesh->vertices[5].normal[2] = 0.0f;
		mesh->vertices[5].texcoord[0] = 0.0f;
		mesh->vertices[5].texcoord[1] = 0.0f;
		
		mesh->vertices[6].position[0] = 0.5f;
		mesh->vertices[6].position[1] = 0.5f;
		mesh->vertices[6].position[2] = -0.5f;
		mesh->vertices[6].normal[0] = 0.0f;
		mesh->vertices[6].normal[1] = 0.0f;
		mesh->vertices[6].normal[2] = 1.0f;
		mesh->vertices[6].texcoord[0] = 0.0f;
		mesh->vertices[6].texcoord[1] = 1.0f;
		
		mesh->vertices[7].position[0] = -0.5f;
		mesh->vertices[7].position[1] = 0.5f;
		mesh->vertices[7].position[2] = -0.5f;
		mesh->vertices[7].normal[0] = 1.0f;
		mesh->vertices[7].normal[1] = 1.0f;
		mesh->vertices[7].normal[2] = 1.0f;
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
	
	// Used for creating descriptor sets for shader data transmission.
    uint8_t num_attributes = WS_MESH_MAX_ATTRIBUTE_DESCRIPTIONS;
    mesh->num_attribute_descs = num_attributes;
    mesh->attribute_descs = malloc(num_attributes * sizeof(VkVertexInputAttributeDescription));
    wsMeshSetBindingDescription(mesh);
    wsMeshSetAttributeDescriptions(mesh);
	
    printf("INFO: Mesh %p initialized!\n", mesh);
	
	printf("\n");
}

void wsMeshDestroy(wsMesh* mesh, VkDevice* logicalDevice)
{
	free(mesh->vertices);
	free(mesh->indices);
	free(mesh->attribute_descs);
	
	vkDestroyBuffer(*logicalDevice, mesh->vertexBuffer.buffer, NULL);
	vkFreeMemory(*logicalDevice, mesh->vertexBuffer.memory, NULL);
	vkDestroyBuffer(*logicalDevice, mesh->indexBuffer.buffer, NULL);
	vkFreeMemory(*logicalDevice, mesh->indexBuffer.memory, NULL);
}

cgltf_data* wsMeshLoadGLTF(const char* path, cgltf_options options)
{
	cgltf_data* data = NULL;
	cgltf_result result = cgltf_parse_file(&options, path, &data);
	
	if(result != cgltf_result_success)
	{
		printf("INFO: File \"%s\" load failed w/ error code %i!\n", path, result);
		return NULL;
	}
	else printf("INFO: Mesh file found!\n");
	
	result = cgltf_load_buffers(&options, data, path);
	result = cgltf_validate(data);
	
	if(result != cgltf_result_success)
		{ printf("ERROR: Mesh file did not pass validation; send it back to your 3D artist!\n"); return NULL; }
	else { printf("INFO: Mesh file has passed validation!\n"); }
	if(data->meshes[0].primitives[0].type != cgltf_primitive_type_triangles)
		{ printf("ERROR: Mesh file primitive mode is not triangles!\n"); return NULL; }
	
	return data;
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
    mesh->attribute_descs[1].offset = offsetof(wsVertex, normal);
	
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

void wsMeshPrintMeshData(wsMesh* mesh)
{
	printf("INFO: Printing mesh data: \n");
	printf("\tVertex count: %i\n", mesh->num_vertices);
	printf("\tIndex count: %i\n", mesh->num_indices);
	printf("\tAttribute description count: %i\n", mesh->num_attribute_descs);
}