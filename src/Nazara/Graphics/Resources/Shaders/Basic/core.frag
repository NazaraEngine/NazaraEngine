#if EARLY_FRAGMENT_TESTS && !ALPHA_TEST
layout(early_fragment_tests) in;
#endif

/********************Entrant********************/
in vec2 vTexCoord;

/********************Sortant********************/
out vec4 RenderTarget0;

/********************Uniformes********************/
uniform sampler2D MaterialAlphaMap;
uniform float MaterialAlphaThreshold;
uniform vec4 MaterialDiffuse;
uniform sampler2D MaterialDiffuseMap;
uniform vec2 InvTargetSize;

/********************Fonctions********************/
void main()
{
	vec4 fragmentColor = MaterialDiffuse;

#if AUTO_TEXCOORDS
	vec2 texCoord = gl_FragCoord.xy * InvTargetSize;
#else
	vec2 texCoord = vTexCoord;
#endif

#if DIFFUSE_MAPPING
	fragmentColor *= texture(MaterialDiffuseMap, texCoord);
#endif

#if ALPHA_MAPPING
	fragmentColor.a *= texture(MaterialAlphaMap, texCoord).r;
#endif

#if ALPHA_TEST
	if (fragmentColor.a < MaterialAlphaThreshold)
		discard;
#endif

	RenderTarget0 = fragmentColor;
}