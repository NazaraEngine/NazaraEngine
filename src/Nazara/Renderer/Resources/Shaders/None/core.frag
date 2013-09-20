/********************Entrant********************/
in vec2 vTexCoord;

/********************Sortant********************/
out vec4 RenderTarget0;

/********************Uniformes********************/
uniform sampler2D MaterialAlphaMap;
uniform float MaterialAlphaThreshold;
uniform vec4 MaterialDiffuse;
uniform sampler2D MaterialDiffuseMap;

/********************Fonctions********************/
void main()
{
#if FLAG_DEFERRED
	RenderTarget0 = vec4(MaterialDiffuse.rgb, 0.0);
#else
	RenderTarget0 = MaterialDiffuse;
#endif
}