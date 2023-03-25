#ifndef CAMERA_H_
#define CAMERA_H_


#include<CGLM/cglm.h>


#define WS_CAMERA_MAX_CAMERAS 10


/* Because of the nature of how this data will be processed (all data per 
	camera is used without touching other data), it makes more sense to use 
	an OOP approach here.  I will not be doing that.  I probably won't regret
	it. */
typedef struct wsCamera
{
	bool is_active[WS_CAMERA_MAX_CAMERAS];
	float fov[WS_CAMERA_MAX_CAMERAS];
	vec3 position[WS_CAMERA_MAX_CAMERAS];
	vec4 rotation[WS_CAMERA_MAX_CAMERAS];
	mat4 projection[WS_CAMERA_MAX_CAMERAS];
	
} wsCamera;


void wsCameraInit(wsCamera* camera);	// Initialize cameras, so that we may store camera data in vulkan_interface.c for improved CPU (and potentially GPU) cache performance.
uint8_t wsCameraCreate();				// Returns cameraID of newly created camera!
void wsCameraUpdateUBOFields(uint8_t cameraID, vec3* position, vec4* rotation, mat4* projection);
void wsCameraDestroy(uint8_t cameraID);	// Destroys 1 (one) single (specified) camera.
void wsCameraStop();					// Destroys all cameras!

void wsCameraSetPosition(uint8_t cameraID, vec3 position_new);
void wsCameraSetRotation(uint8_t cameraID, vec4 rotation_new);
void wsCameraSetProjectionMatrix(uint8_t cameraID, mat4 projection_new);


#endif