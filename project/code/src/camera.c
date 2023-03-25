#include<stdio.h>
#include"h/camera.h"
#include"h/input.h"


wsCamera* cm;


void wsCameraInit(wsCamera* camera)
{
	cm = camera;
	
	printf("INFO: Initializing cameras!\n");
}
uint8_t wsCameraCreate()
{
	static uint8_t cameraID = 0;
	for(uint8_t i = 0; i < WS_CAMERA_MAX_CAMERAS; i++)
	{
		if(!cm->is_active[i])
		{
			return cameraID;
			printf("INFO: Camera created w/ ID %i!\n", i);
		}
	}
	
	printf("WARNING: No available camera ID was found; Assigning ID 0 instead!\n");
	return 0;
}

void wsCameraUpdateUBOFields(uint8_t cameraID, vec3* position, vec4* rotation, mat4* projection)
{
	glm_vec3_copy(*position, cm->position[cameraID]);
	glm_vec4_copy(*rotation, cm->rotation[cameraID]);
	glm_mat4_copy(*projection, cm->projection[cameraID]);
}

void wsCameraDestroy(uint8_t cameraID)
{
	cm->is_active[cameraID] = false;
	printf("INFO: Camera w/ ID %i deactivated!\n", cameraID);
}
void wsCameraStop()
{
	for(uint8_t i = 0; i < WS_CAMERA_MAX_CAMERAS; i++)
	{
		cm->is_active[i] = false;
	}
	
	printf("INFO: All cameras deactivated!\n");
}

void wsCameraSetPosition(uint8_t cameraID, vec3 position_new)
	{ glm_vec3_copy(position_new, cm->position[cameraID]); }
void wsCameraSetRotation(uint8_t cameraID, vec4 rotation_new)
	{ glm_vec4_copy(rotation_new, cm->rotation[cameraID]); }
void wsCameraSetProjectionMatrix(uint8_t cameraID, mat4 projection_new)
	{ glm_mat4_copy(projection_new, cm->projection[cameraID]); }