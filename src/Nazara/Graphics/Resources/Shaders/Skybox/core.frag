#version 140

in vec3 vTexCoord;

out vec4 RenderTarget0;

uniform samplerCube SkyboxTexture;

layout (std140) uniform SkyboxData
{
	mat4 SkyboxMatrix;
	float VertexDepth;
};

void main()
{
	RenderTarget0 = texture(SkyboxTexture, vTexCoord);
	gl_FragDepth = VertexDepth;
}
