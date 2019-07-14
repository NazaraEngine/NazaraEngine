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
uniform sampler2D MaterialDiffuseMap;
uniform sampler2D TextureOverlay;

layout (std140) uniform MaterialBasicSettings
{
	vec4 diffuseColor;
	float alphaThreshold;
};

/********************Fonctions********************/
void main()
{
	vec4 fragmentColor = diffuseColor * vColor;

#if AUTO_TEXCOORDS
	vec2 texCoord = gl_FragCoord.xy * InvTargetSize;
#elif TEXTURE_MAPPING
	vec2 texCoord = vTexCoord;
#endif

#if HAS_DIFFUSE_TEXTURE
	fragmentColor *= texture(MaterialDiffuseMap, texCoord);
#endif

#if HAS_ALPHA_TEXTURE
	fragmentColor.a *= texture(MaterialAlphaMap, texCoord).r;
#endif

#if FLAG_TEXTUREOVERLAY
	fragmentColor *= texture(TextureOverlay, texCoord);
#endif

#if ALPHA_TEST
	if (fragmentColor.a < alphaThreshold)
		discard;
#endif

	RenderTarget0 = fragmentColor;
}
