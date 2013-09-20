#if FLAG_DEFERRED
	#error Deferred Shading needs core profile
#endif

/********************Entrant********************/
in vec2 VertexPosition;

/********************Sortant********************/
out vec2 vTexCoord;

/********************Uniformes********************/
uniform sampler2D MaterialAlphaMap;
uniform float MaterialAlphaThreshold;
uniform vec4 MaterialDiffuse;
uniform sampler2D MaterialDiffuseMap;
uniform float VertexDepth;

/********************Fonctions********************/
void main()
{
	gl_Position = vec4(VertexPosition, VertexDepth, 1.0);

#if ALPHA_MAPPING || DIFFUSE_MAPPING
	#if FLAG_FLIP_UVS
	vTexCoord = vec2((VertexPosition.x + 1.0)*0.5, 0.5 - VertexPosition.y*0.5;
	#else
	vTexCoord = vec2((VertexPosition.x + 1.0)*0.5, (VertexPosition.y + 1.0)*0.5);
	#endif // FLAG_FLIP_UVS
#endif
}
