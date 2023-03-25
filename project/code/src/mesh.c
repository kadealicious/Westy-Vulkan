#include<stdio.h>
#include<stdint.h>
#include"h/mesh.h"


// Contains all data for transfer to shaders for all meshes!
wsMesh* md;

uint8_t wsMeshCreate();
void wsMeshCopyData(uint8_t destID, uint8_t srcID);
void wsMeshMoveData(uint8_t destID, uint8_t srcID);
void wsMeshSetMap(uint8_t destID, uint8_t srcID);
void wsMeshUnload(uint8_t* meshIDs, uint8_t num_meshes);

wsVertex* wsMeshGetVerticesPtr();
uint32_t wsMeshGetCurrentBufferSize();
void wsMeshConsolidateBuffer();

void wsMeshSetBindingDescription(uint8_t meshID);
void wsMeshSetAttributeDescriptions(uint8_t meshID);

void wsMeshPrintMeshData(uint8_t meshID);


void wsMeshInit(wsMesh* mesh_data) {
    
    md = mesh_data;
	md->num_active_meshes = 0;
    
    printf("INFO: Meshes initialized!\n");
}
void wsMeshStop()
{
    wsMeshUnload(NULL, 0);
    
    printf("INFO: Meshes destroyed!\n");
}

uint8_t wsMeshCreate()
{
	// Find a vacant meshID to use!
    uint8_t meshID = (WS_MESH_MAX_MESHES+1);
	for(uint8_t i = 0; i < WS_MESH_MAX_MESHES; i++)
	{
		if(md->isloaded[i] == WS_MESH_UNLOADED)
		{
			meshID = i;
			break;
		}
	}
    if(meshID == (WS_MESH_MAX_MESHES+1))
	{
		printf("ERROR: No vacant mesh ID could be found; mesh not loaded!\n");
		return 0;
	}
	
	
    // Load vertices.
    md->num_vertices[meshID] = 4;
    md->vertices[meshID] = (wsVertex*)malloc(md->num_vertices[meshID] * sizeof(wsVertex));
	
	md->vertices[meshID][0].position[0] = -0.5f;
	md->vertices[meshID][0].position[1] = -0.5f;
	md->vertices[meshID][0].color[0] = 1.0f;
	md->vertices[meshID][0].color[1] = 0.0f;
	md->vertices[meshID][0].color[2] = 0.0f;
    
	md->vertices[meshID][1].position[0] = 0.5f;
	md->vertices[meshID][1].position[1] = -0.5f;
	md->vertices[meshID][1].color[0] = 0.0f;
	md->vertices[meshID][1].color[1] = 1.0f;
	md->vertices[meshID][1].color[2] = 0.0f;
	
	md->vertices[meshID][2].position[0] = 0.5f;
	md->vertices[meshID][2].position[1] = 0.5f;
	md->vertices[meshID][2].color[0] = 0.0f;
	md->vertices[meshID][2].color[1] = 0.0f;
	md->vertices[meshID][2].color[2] = 1.0f;
	
	md->vertices[meshID][3].position[0] = -0.5f;
	md->vertices[meshID][3].position[1] = 0.5f;
	md->vertices[meshID][3].color[0] = 1.0f;
	md->vertices[meshID][3].color[1] = 1.0f;
	md->vertices[meshID][3].color[2] = 1.0f;
	
	
	// Load indices.  Jesus Christ.
	md->num_indices[meshID] = 6;
	md->indices[meshID] = (uint16_t*)malloc(md->num_indices[meshID] * sizeof(uint16_t));
	md->indices[meshID][0] = 0;
	md->indices[meshID][1] = 1;
	md->indices[meshID][2] = 2;
	md->indices[meshID][3] = 2;
	md->indices[meshID][4] = 3;
	md->indices[meshID][5] = 0;
	
	
    // Configure vertex binding descriptions, then the more fundamental attribute descriptions.
    uint8_t num_attributes = WS_MESH_MAX_ATTRIBUTE_DESCRIPTIONS;
    md->num_attribute_descs[meshID] = num_attributes;
    md->attribute_descs[meshID] = malloc(num_attributes * sizeof(VkVertexInputAttributeDescription));
    wsMeshSetBindingDescription(meshID);
    wsMeshSetAttributeDescriptions(meshID);
    
    md->isloaded[meshID] = WS_MESH_LOADED;
	md->num_active_meshes++;
	
	// wsMeshPrintMeshData(meshID);
	
    printf("INFO: Mesh w/ ID %i initialized!\n", meshID);
	wsMeshConsolidateBuffer();
	return meshID;
}
void wsMeshUnload(uint8_t* meshIDs, uint8_t num_meshes)
{
	if(meshIDs == NULL)
	{	// If we need to free all meshes.
		num_meshes = md->num_active_meshes;
		
		for(uint8_t i = 0; i < num_meshes; i++)
			{ free(md->vertices[i]); }
		for(uint8_t i = 0; i < num_meshes; i++)
			{ free(md->indices[i]); }
		for(uint8_t i = 0; i < num_meshes; i++)
			{ free(md->attribute_descs[i]); }
		for(uint8_t i = 0; i < num_meshes; i++)
			{ md->isloaded[i] = WS_MESH_UNLOADED; }
		
		md->num_active_meshes = 0;
	} else
	{	// IF we need to free only a few meshes.
		for(uint8_t i = 0; i < num_meshes; i++)
			{ free(md->vertices[meshIDs[i]]); }
		for(uint8_t i = 0; i < num_meshes; i++)
			{ free(md->indices[meshIDs[i]]); }
		for(uint8_t i = 0; i < num_meshes; i++)
			{ free(md->attribute_descs[meshIDs[i]]); }
		for(uint8_t i = 0; i < num_meshes; i++)
			{ md->isloaded[meshIDs[i]] = WS_MESH_UNLOADED; }
		
		md->num_active_meshes -= num_meshes;
	}
	
	wsMeshConsolidateBuffer();
}
void wsMeshCopyData(uint8_t destID, uint8_t srcID)
{
	md->vertices[destID] = md->vertices[srcID];
	md->num_vertices[destID] = md->num_vertices[srcID];
	md->binding_descs[destID] = md->binding_descs[srcID];
	md->attribute_descs[destID] = md->attribute_descs[srcID];
	md->num_attribute_descs[destID] = md->num_attribute_descs[srcID];
	md->isloaded[destID] = md->isloaded[srcID];
}
void wsMeshMoveData(uint8_t destID, uint8_t srcID)
{
	wsMeshCopyData(destID, srcID);
	md->isloaded[srcID] = WS_MESH_UNLOADED;
}

void wsMeshPrintMeshData(uint8_t meshID)
{
	printf("INFO: Printing mesh %i's data: \n", meshID);
	for(uint32_t i = 0; i < md->num_vertices[meshID]; i++)
	{
		printf("\tVertex %i - Position: %f %f, Color: %f %f %f\n", i, md->vertices[meshID][i].position[0], 
			md->vertices[meshID][i].position[1], md->vertices[meshID][i].position[2], md->vertices[meshID][i].color[0], 
			md->vertices[meshID][i].color[1]);
	}
}
wsVertex* wsMeshGetVerticesPtr()
	{ return md->vertices[0]; }
uint16_t* wsMeshGetIndicesPtr()
	{ return md->indices[0]; }

void wsMeshConsolidateBuffer()
{
	uint8_t num_consolidated = 0;
	uint8_t num_meshes = 0;
	
	uint8_t j = 0;
	for(uint8_t i = (WS_MESH_MAX_MESHES-1); i > j; i--)
	{	// If we haven't loaded any meshes at index i, continue.
		if(md->isloaded[i] == WS_MESH_UNLOADED)
			{ continue; }
		else num_meshes++;
		
		for(uint8_t j = 0; j < i; j++)
		{	// If we have loaded data at index i and have none at index j, swap.
			if(md->isloaded[j] == WS_MESH_UNLOADED && md->isloaded[i] == WS_MESH_LOADED)
			{
				wsMeshMoveData(j, i);	// Copy all mesh data & pointers to data.
				num_consolidated++;
				num_meshes++;
				break;
			}
		}
	}
	
	/*printf("%i active meshes: \n", md->num_active_meshes);
	for(uint8_t i = 0; i < WS_MESH_MAX_MESHES; i++)
	{
		printf("\tMesh %i, isloaded = %i: %i vertices, %i indices\n", i, md->isloaded[i], md->num_vertices[i], md->num_indices[i]);
	}*/
	
	printf("INFO: %i meshes consolidated successfully!\n", num_consolidated);
}
uint32_t wsMeshGetCurrentVertexBufferSize()
{
	uint32_t buffer_size = 0;
	
	uint8_t i = 0;
	while(md->isloaded[i] == WS_MESH_LOADED && i < md->num_vertices[i])
	{
		buffer_size += md->num_vertices[i] * sizeof(wsVertex);
		i++;
	}
	
	return buffer_size;
}
uint32_t wsMeshGetCurrentIndexBufferSize()
{
	uint32_t buffer_size = 0;
	
	uint8_t i = 0;
	while(md->isloaded[i] == WS_MESH_LOADED && i < md->num_indices[i])
	{
		buffer_size += md->num_indices[i] * sizeof(uint16_t);
		i++;
	}
	
	return buffer_size;
}

void wsMeshSetAttributeDescriptions(uint8_t meshID) {
    
    md->attribute_descs[meshID][0].binding = 0;
    md->attribute_descs[meshID][0].location = 0;
    md->attribute_descs[meshID][0].format = VK_FORMAT_R32G32_SFLOAT;
    md->attribute_descs[meshID][0].offset = offsetof(wsVertex, position);
    md->attribute_descs[meshID][1].binding = 0;
    md->attribute_descs[meshID][1].location = 1;
    md->attribute_descs[meshID][1].format = VK_FORMAT_R32G32B32_SFLOAT;
    md->attribute_descs[meshID][1].offset = offsetof(wsVertex, color);
}
void wsMeshSetBindingDescription(uint8_t meshID) {
    
    md->binding_descs[meshID].binding = 0;
    md->binding_descs[meshID].stride = sizeof(wsVertex);
    md->binding_descs[meshID].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
}

uint8_t wsMeshGetNumAttributeDescriptions(uint8_t meshID) {
    return md->num_attribute_descs[meshID];
}
VkVertexInputAttributeDescription* wsMeshGetAttributeDescriptions(uint8_t meshID) {
    return &md->attribute_descs[meshID][0];
}
VkVertexInputBindingDescription* wsMeshGetBindingDescription(uint8_t meshID) {
    return &md->binding_descs[meshID];
}