#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormals;
layout (location = 2) in vec2 inTexCoord;

layout (binding = 0) uniform UBO 
{
	mat4 projectionMatrix;
	mat4 modelMatrix;
	mat4 viewMatrix;
} ubo;

layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec2 outTexCoords;

out gl_PerVertex 
{
    vec4 gl_Position;   
};


void main() 
{
	outNormal = inNormals;
	outTexCoords = inTexCoord;
	gl_Position = ubo.projectionMatrix * ubo.viewMatrix * ubo.modelMatrix * vec4(inPos, 1.0);
	gl_Position.y = -gl_Position.y;
}
