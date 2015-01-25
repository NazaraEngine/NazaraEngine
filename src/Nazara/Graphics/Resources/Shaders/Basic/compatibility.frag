/********************Entrant********************/
varying vec2 vTexCoord;
varying vec4 vColor;

/********************Uniformes********************/
uniform sampler2D MaterialAlphaMap;
uniform float MaterialAlphaThreshold;
uniform vec4 MaterialDiffuse;
uniform sampler2D MaterialDiffuseMap;
uniform vec2 InvTargetSize;

/********************Fonctions********************/
void main()
{
	vec4 fragmentColor = MaterialDiffuse; * vColor;

#if AUTO_TEXCOORDS
	vec2 texCoord = gl_FragCoord.xy * InvTargetSize;
#else
	vec2 texCoord = vTexCoord;
#endif

#if DIFFUSE_MAPPING
	fragmentColor *= texture2D(MaterialDiffuseMap, texCoord);
#endif

#if ALPHA_MAPPING
	fragmentColor.a *= texture2D(MaterialAlphaMap, texCoord).r;
#endif

#if ALPHA_TEST
	if (fragmentColor.a < MaterialAlphaThreshold)
		discard;
#endif

	gl_FragColor = fragmentColor;
}