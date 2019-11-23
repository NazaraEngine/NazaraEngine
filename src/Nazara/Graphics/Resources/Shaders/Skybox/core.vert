#version 140

in vec3 VertexPosition;

out vec3 vTexCoord;

layout (std140) uniform SkyboxData
{
	mat4 SkyboxMatrix;
	float VertexDepth;
};

void main()
{
    vec4 WVPVertex = SkyboxMatrix * vec4(VertexPosition, 1.0);
    gl_Position = WVPVertex.xyww;
    vTexCoord = VertexPosition;
}