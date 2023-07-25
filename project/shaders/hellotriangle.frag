#version 450


layout(location = 0) in vec3 frag_normal;
layout(location = 1) in vec2 frag_texcoord;

layout(binding = 1) uniform sampler2D texsampler;

layout(location = 0) out vec4 out_color;


void main()
{
    out_color = vec4(frag_normal * texture(texsampler, frag_texcoord).rgb, texture(texsampler, frag_texcoord).a);
}