/********************Entrant********************/
in mat4 InstanceData0;
in vec3 VertexPosition;
in vec2 VertexTexCoord;

/********************Sortant********************/
out vec2 vTexCoord;

/********************Uniformes********************/
uniform sampler2D MaterialAlphaMap;
uniform float MaterialAlphaThreshold;
uniform vec4 MaterialDiffuse;
uniform sampler2D MaterialDiffuseMap;
uniform mat4 ViewProjMatrix;
uniform mat4 WorldViewProjMatrix;

/********************Fonctions********************/
void main()
{
#if FLAG_INSTANCING
	gl_Position = ViewProjMatrix * InstanceData0 * vec4(VertexPosition, 1.0);
#else
	gl_Position = WorldViewProjMatrix * vec4(VertexPosition, 1.0);
#endif

#if ALPHA_MAPPING || DIFFUSE_MAPPING
	#if FLAG_FLIP_UVS
	vTexCoord = vec2(VertexTexCoord.x, 1.0 - VertexTexCoord.y);
	#else
	vTexCoord = vec2(VertexTexCoord);
	#endif // FLAG_FLIP_UVS
#endif
}
