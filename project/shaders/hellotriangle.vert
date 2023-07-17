#version 450


layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;
layout(location = 2) in vec2 in_texcoord;
// TODO: How do?
// layout(location = 3) in float in_time;

layout(binding = 0) uniform UniformBufferObject
{
	mat4 model;
	mat4 view;
	mat4 proj;
}
ubo;

layout(location = 0) out vec3 frag_color;
layout(location = 1) out vec2 frag_texcoord;


void main()
{
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(in_position, 1.0);
    frag_color = in_color;
	frag_texcoord = in_texcoord;
}