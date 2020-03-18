#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(binding = 1) uniform sampler2D texSampler;

layout (location = 0) in vec3 inColor;
layout (location = 1) in vec2 outTexCoords;

layout (location = 0) out vec4 outFragColor;

void main() 
{
  outFragColor = texture(texSampler, outTexCoords);
}
