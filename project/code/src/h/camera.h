#ifndef CAMERA_H_
#define CAMERA_H_


#include<CGLM/cglm.h>

typedef struct wsCamera
{
	vec3 position;
	vec4 rotation;
	mat4 projection;
	
	vec3 right;
	vec3 up;
	vec3 forward;
	vec3 forward_interpolated;
	
	float fov;
	
} wsCamera;

void wsCameraUpdateUBOFields(wsCamera* camera, vec3* position, vec4* rotation, mat4* projection);
void wsCameraUpdateProjection(wsCamera* camera);
void wsCameraUpdateFPSLook(wsCamera* camera, vec3* position, float time_delta);


#endif