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
	gl_FragColor = MaterialDiffuse;
}