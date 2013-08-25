#if FLAG_DEFERRED
	#error Deferred Shading needs core profile
#endif

/********************Entrant********************/
varying vec2 vTexCoord;

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
	fragmentColor *= texture2D(MaterialDiffuseMap, vTexCoord);
#endif

#if ALPHA_MAPPING
	fragmentColor.a *= texture2D(MaterialAlphaMap, vTexCoord).r;
#endif

#if ALPHA_TEST
	if (fragmentColor.a < MaterialAlphaThreshold)
		discard;
#endif

	gl_FragColor = fragmentColor;
}