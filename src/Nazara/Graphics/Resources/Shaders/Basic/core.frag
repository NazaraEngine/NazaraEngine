#if EARLY_FRAGMENT_TESTS && !ALPHA_TEST
layout(early_fragment_tests) in;
#endif

/********************Entrant********************/
in vec4 vColor;
in vec2 vTexCoord;

/********************Sortant********************/
out vec4 RenderTarget0;

/********************Uniformes********************/
uniform vec2 InvTargetSize;
uniform sampler2D MaterialAlphaMap;
uniform float MaterialAlphaThreshold;
uniform vec4 MaterialDiffuse;
uniform sampler2D MaterialDiffuseMap;
uniform sampler2D TextureOverlay;

/********************Fonctions********************/
void main()
{
	vec4 fragmentColor = MaterialDiffuse * vColor;

#if AUTO_TEXCOORDS
	vec2 texCoord = gl_FragCoord.xy * InvTargetSize;
#elif TEXTURE_MAPPING
	vec2 texCoord = vTexCoord;
#endif

#if DIFFUSE_MAPPING
	fragmentColor *= texture(MaterialDiffuseMap, texCoord);
#endif

#if ALPHA_MAPPING
	fragmentColor.a *= texture(MaterialAlphaMap, texCoord).r;
#endif

#if FLAG_TEXTUREOVERLAY
	fragmentColor *= texture(TextureOverlay, texCoord);
#endif

#if ALPHA_TEST
	if (fragmentColor.a < MaterialAlphaThreshold)
		discard;
#endif

	RenderTarget0 = fragmentColor;
}