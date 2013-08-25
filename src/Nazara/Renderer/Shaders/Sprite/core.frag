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
	vec4 fragmentColor = MaterialDiffuse;
#if DIFFUSE_MAPPING
	fragmentColor *= texture(MaterialDiffuseMap, vTexCoord);
#endif

#if ALPHA_MAPPING
	fragmentColor.a *= texture(MaterialAlphaMap, vTexCoord).r;
#endif

#if ALPHA_TEST
	if (fragmentColor.a < MaterialAlphaThreshold)
		discard;
#endif

#if FLAG_DEFERRED
	RenderTarget0 = vec4(fragmentColor.rgb, 0.0);
#else
	RenderTarget0 = fragmentColor;
#endif
}