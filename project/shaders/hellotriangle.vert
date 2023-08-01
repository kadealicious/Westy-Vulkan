#version 450


layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;

layout(push_constant, std430) uniform GlobalBlock
{
	float time;
}
pushConstants;

layout(binding = 0) uniform UniformBufferObject
{
	mat4 model;
	mat4 view;
	mat4 proj;
}
ubo;

layout(location = 0) out vec3 frag_normal;
layout(location = 1) out vec2 frag_texcoord;


void main()
{
	float sizePulse = (sin(pushConstants.time * 3) * 0.05) + 0.95;
	vec3 crazyPosition = in_position * vec3
	(
		sin(pushConstants.time * 3) * 0.1 + 0.9, 
		cos(pushConstants.time * 3) * 0.1 + 0.9, 
		sin(pushConstants.time * 3) * 0.1 + 0.9
	);
	
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(crazyPosition, 1.0);
    frag_normal = in_normal;
	frag_texcoord = in_texcoord;
}