#include<stdio.h>
#include"h/graphics.h"
#include"h/camera.h"
#include"h/input.h"


wsCamera* cm;


void wsCameraInit(wsCamera* camera)
{
	cm = camera;
	
	printf("INFO: Initializing cameras!\n");
}

void wsCameraTerminate()
{
	for(uint8_t i = 0; i < WS_CAMERA_MAX_CAMERAS; i++)
	{
		cm->is_active[i] = false;
	}
	
	printf("INFO: All cameras deactivated!\n");
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
			
			cm->forward[cameraID][0] = 0.0f;
			cm->forward[cameraID][1] = 0.0f;
			cm->forward[cameraID][2] = -1.0f;
			
			return cameraID;
			printf("INFO: Camera created w/ ID %i!\n", i);
		}
	}
	
	printf("WARNING: No available camera ID was found; Assigning ID 0 instead!\n");
	return 0;
}

void wsCameraDestroy(uint8_t cameraID)
{
	cm->is_active[cameraID] = false;
	printf("INFO: Camera w/ ID %i deactivated!\n", cameraID);
}

void wsCameraUpdateUBOFields(uint8_t cameraID, vec3* position, vec4* rotation, mat4* projection)
{
	glm_vec3_copy(*position, cm->position[cameraID]);
	glm_vec4_copy(*rotation, cm->rotation[cameraID]);
	glm_mat4_copy(*projection, cm->projection[cameraID]);
}
void wsCameraUpdateProjection(uint8_t cameraID)
{
	float near = 0.1f;
	float far = 1000.0f;
	glm_perspective(glm_rad(wsCameraGetFOV(cameraID)), wsVulkanGetAspectRatio(), near, far, cm->projection[cameraID]);
	cm->projection[cameraID][1][1] *= -1;	// GLM was designed to work with OpenGL; Vulkan has a slightly differed coord. system (reversed y-axis I think).
	// printf("FOV: rad: %f,  deg: %f\n", glm_rad(wsCameraGetFOV(cameraID)), wsCameraGetFOV(cameraID));
}
void wsCameraUpdateFPSCamera(uint8_t cameraID, float time_delta)
{
	// Rotation angles.
	static float yaw = -M_PI_2;
	static float pitch = 0.0f;
	yaw -= wsInputGetMouseMoveX() * time_delta;
	pitch -= wsInputGetMouseMoveY() * time_delta;
	vec2 pitch_limits = {(-M_PI_2 + 0.01f), (M_PI_2 - 0.01f)};
	if(pitch > pitch_limits[1])
		pitch = pitch_limits[1];
	else if(pitch < pitch_limits[0])
		pitch = pitch_limits[0];
	
	// TODO: Make all this work!
	// Camera direction vector, used for the look-at matrix.
	cm->forward[cameraID][0] = cosf(yaw) * cosf(pitch);
	cm->forward[cameraID][1] = sinf(pitch);
	cm->forward[cameraID][2] = sinf(yaw) * cosf(pitch);
	glm_vec3_normalize(cm->forward[cameraID]);
	// printf("cam dir: %f %f %f\n", cm->forward[cameraID][0], cm->forward[cameraID][1], cm->forward[cameraID][2]);
	
	// Calculate camera right and up vectors.
	vec3 world_up = {0.0f, 1.0f, 0.0f};
	glm_vec3_cross(cm->forward[cameraID], world_up, cm->right[cameraID]);
	glm_vec3_cross(cm->forward[cameraID], cm->right[cameraID], cm->up[cameraID]);
	glm_vec3_normalize(cm->right[cameraID]);
	glm_vec3_normalize(cm->up[cameraID]);
	// printf("cam rot: %f %f %f %f\n", cm->rotation[cameraID][0], cm->rotation[cameraID][1], cm->rotation[cameraID][2], cm->rotation[cameraID][3]);
	
	// Calculate amounts to move on each axis based on keypresses.
	float move_speed = 2.0f;
	float keys_vertical = -(wsInputGetKeyHold(GLFW_KEY_SPACE) - wsInputGetKeyHold(GLFW_KEY_LEFT_CONTROL)) * time_delta * move_speed;
	float keys_horizontal = -(wsInputGetKeyHold(GLFW_KEY_D) - wsInputGetKeyHold(GLFW_KEY_A)) * time_delta * move_speed;
	float keys_forward = (wsInputGetKeyHold(GLFW_KEY_W) - wsInputGetKeyHold(GLFW_KEY_S)) * time_delta * move_speed;
	vec3 move_vertical;
	vec3 move_horizontal;
	vec3 move_forward;
	glm_vec3_scale(world_up, keys_vertical, move_vertical);
	glm_vec3_scale(cm->right[cameraID], keys_horizontal, move_horizontal);
	vec3 forward_xz_plane = {cm->forward[cameraID][0], 0.0f, cm->forward[cameraID][2]};
	glm_vec3_normalize(forward_xz_plane);
	glm_vec3_scale(forward_xz_plane, keys_forward, move_forward);
	// Move on axes.
	glm_vec3_add(move_vertical, cm->position[cameraID], cm->position[cameraID]);
	glm_vec3_add(move_horizontal, cm->position[cameraID], cm->position[cameraID]);
	glm_vec3_add(move_forward, cm->position[cameraID], cm->position[cameraID]);
	// printf("cam pos: %f %f %f\n", cm->position[cameraID][0], cm->position[cameraID][1], cm->position[cameraID][2]);
	
	float fov_new = (wsInputGetMouseScroll() * 0.5f) + 60.0f;
	wsCameraSetFOV(cameraID, fov_new);
}

float wsCameraGetFOV(uint8_t cameraID)
	{ return cm->fov[cameraID]; }
vec3* wsCameraGetPositionPtr(uint8_t cameraID)	{ return &cm->position[cameraID]; }
vec3* wsCameraGetForwardPtr(uint8_t cameraID)	{ return &cm->forward[cameraID]; }
vec3* wsCameraGetUpPtr(uint8_t cameraID)		{ return &cm->up[cameraID]; }
mat4* wsCameraGetProjectionPtr(uint8_t cameraID){ return &cm->projection[cameraID]; }
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