#include<stdio.h>
#include"h/vulkan_interface.h"
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
			cm->fov[i] = 45.0f;
			cm->is_active[i] = true;
			cm->position[cameraID][0] = 0.0f;
			cm->position[cameraID][1] = 0.0f;
			cm->position[cameraID][2] = 2.0f;
			
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
void wsCameraUpdateFPSCamera(uint8_t cameraID)
{
	vec3 camera_target;
	glm_vec3_copy(cm->forward[cameraID], camera_target);
	camera_target[0] += wsInputGetMouseMoveX();
	printf("%f\n", wsInputGetMouseMoveX());	// TODO: Not workie!
	glm_vec3_add(cm->position[cameraID], camera_target, cm->forward[cameraID]);
	glm_vec3_normalize(cm->forward[cameraID]);
	// printf("cam target: %f %f %f\n", camera_target[0], camera_target[1], camera_target[2]);
	
	cm->up[cameraID][0] = 0.0f;
	cm->up[cameraID][1] = 1.0f;
	cm->up[cameraID][2] = 0.0f;
	glm_vec3_cross(cm->up[cameraID], cm->forward[cameraID], cm->right[cameraID]);
	glm_vec3_normalize(cm->right[cameraID]);
	// printf("cam rot: %f %f %f %f\n", cm->rotation[cameraID][0], cm->rotation[cameraID][1], cm->rotation[cameraID][2], cm->rotation[cameraID][3]);
	
	float keys_vertical = (wsInputGetKeyHold(GLFW_KEY_SPACE) - wsInputGetKeyHold(GLFW_KEY_LEFT_CONTROL)) * 0.01f;
	float keys_horizontal = (wsInputGetKeyHold(GLFW_KEY_D) - wsInputGetKeyHold(GLFW_KEY_A)) * 0.01f;
	float keys_forward = (wsInputGetKeyHold(GLFW_KEY_S) - wsInputGetKeyHold(GLFW_KEY_W)) * 0.01f;
	vec3 move_vertical;
	vec3 move_horizontal;
	vec3 move_forward;
	glm_vec3_scale(cm->up[cameraID], keys_vertical, move_vertical);
	glm_vec3_scale(cm->right[cameraID], keys_horizontal, move_horizontal);
	glm_vec3_scale(cm->forward[cameraID], keys_forward, move_forward);
	
	glm_vec3_add(move_vertical, cm->position[cameraID], cm->position[cameraID]);
	glm_vec3_add(move_horizontal, cm->position[cameraID], cm->position[cameraID]);
	glm_vec3_add(move_forward, cm->position[cameraID], cm->position[cameraID]);
	// printf("cam pos: %f %f %f\n", cm->position[cameraID][0], cm->position[cameraID][1], cm->position[cameraID][2]);
	
	float near = 0.1f;
	float far = 1000.0f;
	glm_perspective(wsCameraGetFOV(cameraID), wsVulkanGetAspectRatio(), near, far, cm->projection[cameraID]);
	cm->projection[cameraID][1][1] *= -1;	// GLM was designed to work with OpenGL; Vulkan has a slightly differed coord. system (reversed y-axis I think).
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

float wsCameraGetFOV(uint8_t cameraID)
	{ return cm->fov[cameraID]; }
/*
vec3 wsCameraGetPosition(uint8_t cameraID)
	{ return cm->position[cameraID]; }
vec4 wsCameraGetRotation(uint8_t cameraID)
	{ return cm->rotation[cameraID]; }
mat4 wsCameraGetProjectionMatrix(uint8_t cameraID)
	{ return cm->projection[cameraID]; }
*/

void wsCameraSetFOV(uint8_t cameraID, float fov_new)
	{ cm->fov[cameraID] = fov_new; }
void wsCameraSetPosition(uint8_t cameraID, vec3 position_new)
	{ glm_vec3_copy(position_new, cm->position[cameraID]); }
void wsCameraSetRotation(uint8_t cameraID, vec4 rotation_new)
	{ glm_vec4_copy(rotation_new, cm->rotation[cameraID]); }
void wsCameraSetProjectionMatrix(uint8_t cameraID, mat4 projection_new)
	{ glm_mat4_copy(projection_new, cm->projection[cameraID]); }