#include"h/camera.h"
#include"h/graphics.h"
#include"h/input.h"

void wsCameraUpdateUBOFields(wsCamera* camera, vec3* position, vec4* rotation, mat4* projection)
{
	glm_vec3_copy(*position, camera->position);
	glm_vec4_copy(*rotation, camera->rotation);
	glm_mat4_copy(*projection, camera->projection);
}

void wsCameraUpdateProjection(wsCamera* camera)
{
	float near = 0.1f;
	float far = 1000.0f;
	glm_perspective(glm_rad(camera->fov), wsVulkanGetAspectRatio(), near, far, camera->projection);
	
	/* GLM was designed to work with OpenGL, so we have to do this because 
		Vulkan has a slightly differed coord. system (reversed y-axis I think). */
	camera->projection[1][1] *= -1;
}

void wsCameraUpdateFPSLook(wsCamera* camera, vec3* position, float time_delta)
{
	glm_vec3_copy(*position, camera->position);
	
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
	
	// Camera direction vector, used for the look-at matrix.
	camera->forward[0] = cosf(yaw) * cosf(pitch);
	camera->forward[1] = sinf(pitch);
	camera->forward[2] = sinf(yaw) * cosf(pitch);
	glm_vec3_normalize(camera->forward);
	
	// Calculate camera right and up vectors.
	vec3 world_up = {0.0f, 1.0f, 0.0f};
	glm_vec3_cross(camera->forward, world_up, camera->right);
	glm_vec3_cross(camera->forward, camera->right, camera->up);
	glm_vec3_normalize(camera->right);
	glm_vec3_normalize(camera->up);
	
	// Zooming!
	float fov_new = (wsInputGetMouseScroll() * 0.5f) + 60.0f;
	camera->fov = fov_new;
}