#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(binding = 1) uniform sampler2D texSampler;

layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec2 inUV;

layout (location = 0) out vec4 outFragColor;

void main() 
{
	vec3 lightDir = vec3(0.0, -0.707, 0.707);
	float lightFactor = dot(inNormal, lightDir);

    float gamma = 2.2;
    outFragColor = lightFactor * vec4(pow(texture(texSampler, inUV).xyz, vec3(1.0/gamma)), 1.0);
}
