#version 450


layout(location = 0) in vec3 frag_normal;
layout(location = 1) in vec2 frag_texcoord;

layout(binding = 1) uniform sampler2D texsampler;

layout(push_constant, std430) uniform GlobalBlock
{
	float time;
}
pushConstants;

layout(location = 0) out vec4 out_color;


void main()
{
	float alphaPulse = (sin(pushConstants.time) * 0.35) + 0.55;
	float alpha = alphaPulse * texture(texsampler, frag_texcoord).a;
	
    out_color = vec4(texture(texsampler, frag_texcoord).rgb, alpha);
}