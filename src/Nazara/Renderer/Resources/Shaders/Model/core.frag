#define LIGHT_DIRECTIONAL 0
#define LIGHT_POINT 1
#define LIGHT_SPOT 2

/********************Entrant********************/
in mat3 vLightToWorld;
in vec3 vNormal;
in vec2 vTexCoord;
in vec3 vViewDir;
in vec3 vWorldPos;

/********************Sortant********************/
out vec4 RenderTarget0;
out vec4 RenderTarget1;
out vec4 RenderTarget2;

/********************Uniformes********************/
struct Light
{
	int type;
	vec4 color;
	vec2 factors;

	vec4 parameters1;
	vec4 parameters2;
	vec2 parameters3;
};

uniform vec3 EyePosition;
uniform Light Lights[3];

uniform sampler2D MaterialAlphaMap;
uniform float MaterialAlphaThreshold;
uniform vec4 MaterialAmbient;
uniform vec4 MaterialDiffuse;
uniform sampler2D MaterialDiffuseMap;
uniform sampler2D MaterialEmissiveMap;
uniform sampler2D MaterialHeightMap;
uniform sampler2D MaterialNormalMap;
uniform float MaterialShininess;
uniform vec4 MaterialSpecular;
uniform sampler2D MaterialSpecularMap;

uniform float ParallaxBias = -0.03;
uniform float ParallaxScale = 0.02;
uniform vec4 SceneAmbient;

/********************Fonctions********************/
void main()
{
	vec4 diffuseColor = MaterialDiffuse;
	vec2 texCoord = vTexCoord;
#if LIGHTING && PARALLAX_MAPPING
	float height = texture(MaterialHeightMap, texCoord).r;
	float v = height*ParallaxScale + ParallaxBias;

	vec3 viewDir = normalize(vViewDir);
	texCoord += v * viewDir.xy;
#endif

#if DIFFUSE_MAPPING
	diffuseColor *= texture(MaterialDiffuseMap, texCoord);
#endif

#if FLAG_DEFERRED
	#if ALPHA_TEST
		#if ALPHA_MAPPING // Inutile de faire de l'alpha-mapping sans alpha-test en Deferred (l'alpha n'est pas sauvegardé)
	diffuseColor.a *= texture(MaterialAlphaMap, texCoord).r;
		#endif
		
	if (diffuseColor.a < MaterialAlphaThreshold)
		discard;
	#endif // ALPHA_TEST

	#if LIGHTING
		#if NORMAL_MAPPING
	vec3 normal = normalize(vLightToWorld * (2.0 * vec3(texture(MaterialNormalMap, texCoord)) - 1.0));
		#else
	vec3 normal = normalize(vNormal);
		#endif // NORMAL_MAPPING

	vec3 specularColor = MaterialSpecular.rgb;
		#if SPECULAR_MAPPING
	specularColor *= texture(MaterialSpecularMap, texCoord).rgb;
		#endif

	/*
	Texture0: Diffuse Color + Flags
	Texture1: Normal map + Depth
	Texture2: Specular color + Shininess
	*/
	RenderTarget0 = vec4(diffuseColor.rgb, 1.0);
	RenderTarget1 = vec4(normal*0.5 + 0.5, gl_FragCoord.z);
	RenderTarget2 = vec4(specularColor, (MaterialShininess == 0.0) ? 0.0 : log2(MaterialShininess)/10.5); // http://www.guerrilla-games.com/publications/dr_kz2_rsx_dev07.pdf
	#else // LIGHTING
	RenderTarget0 = vec4(diffuseColor.rgb, 0.0);
	#endif
#else // FLAG_DEFERRED
	#if ALPHA_MAPPING
	diffuseColor.a *= texture(MaterialAlphaMap, texCoord).r;
	#endif

	#if ALPHA_TEST
	if (diffuseColor.a < MaterialAlphaThreshold)
		discard;
	#endif

	#if LIGHTING
	vec3 lightAmbient = vec3(0.0);
	vec3 lightDiffuse = vec3(0.0);
	vec3 lightSpecular = vec3(0.0);

		#if NORMAL_MAPPING
	vec3 normal = normalize(vLightToWorld * (2.0 * vec3(texture(MaterialNormalMap, texCoord)) - 1.0));
		#else
	vec3 normal = normalize(vNormal);
		#endif

	if (MaterialShininess > 0.0)
	{
		vec3 eyeVec = normalize(EyePosition - vWorldPos);

		for (int i = 0; i < 3; ++i)
		{
			switch (Lights[i].type)
			{
				case LIGHT_DIRECTIONAL:
				{
					vec3 lightDir = -Lights[i].parameters1.xyz;

					// Ambient
					lightAmbient += Lights[i].color.rgb * Lights[i].factors.x;

					// Diffuse
					float lambert = max(dot(normal, lightDir), 0.0);

					lightDiffuse += lambert * Lights[i].color.rgb * Lights[i].factors.y;

					// Specular
					vec3 reflection = reflect(-lightDir, normal);
					float specularFactor = max(dot(reflection, eyeVec), 0.0);
					specularFactor = pow(specularFactor, MaterialShininess);

					lightSpecular += specularFactor * Lights[i].color.rgb;
					break;
				}

				case LIGHT_POINT:
				{
					vec3 lightDir = Lights[i].parameters1.xyz - vWorldPos;
					float lightDirLength = length(lightDir);
					lightDir /= lightDirLength; // Normalisation
					
					float att = max(Lights[i].parameters1.w - Lights[i].parameters2.x*lightDirLength, 0.0);

					// Ambient
					lightAmbient += att * Lights[i].color.rgb * Lights[i].factors.x;

					// Diffuse
					float lambert = max(dot(normal, lightDir), 0.0);
					
					lightDiffuse += att * lambert * Lights[i].color.rgb * Lights[i].factors.y;

					// Specular
					vec3 reflection = reflect(-lightDir, normal);
					float specularFactor = max(dot(reflection, eyeVec), 0.0);
					specularFactor = pow(specularFactor, MaterialShininess);

					lightSpecular += att * specularFactor * Lights[i].color.rgb;
					break;
				}

				case LIGHT_SPOT:
				{
					vec3 lightDir = Lights[i].parameters1.xyz - vWorldPos;
					float lightDirLength = length(lightDir);
					lightDir /= lightDirLength; // Normalisation
					
					float att = max(Lights[i].parameters1.w - Lights[i].parameters2.x*lightDirLength, 0.0);

					// Ambient
					lightAmbient += att * Lights[i].color.rgb * Lights[i].factors.x;

					// Diffuse
					float lambert = max(dot(normal, lightDir), 0.0);

					// Modification de l'atténuation pour gérer le spot
					float curAngle = dot(Lights[i].parameters2.xyz, -lightDir);
					float outerAngle = Lights[i].parameters3.y;
					float innerMinusOuterAngle = Lights[i].parameters3.x - outerAngle;
					att *= max((curAngle - outerAngle) / innerMinusOuterAngle, 0.0);

					lightDiffuse += att * lambert * Lights[i].color.rgb * Lights[i].factors.y;

					// Specular
					vec3 reflection = reflect(-lightDir, normal);
					float specularFactor = max(dot(reflection, eyeVec), 0.0);
					specularFactor = pow(specularFactor, MaterialShininess);

					lightSpecular += att * specularFactor * Lights[i].color.rgb;
					break;
				}
				
				default:
					break;
			}
		}
	}
	else
	{
		for (int i = 0; i < 3; ++i)
		{
			switch (Lights[i].type)
			{
				case LIGHT_DIRECTIONAL:
				{
					vec3 lightDir = normalize(-Lights[i].parameters1.xyz);

					// Ambient
					lightAmbient += Lights[i].color.rgb * Lights[i].factors.x;

					// Diffuse
					float lambert = max(dot(normal, lightDir), 0.0);

					lightDiffuse += lambert * Lights[i].color.rgb * Lights[i].factors.y;
					break;
				}

				case LIGHT_POINT:
				{
					vec3 lightDir = Lights[i].parameters1.xyz - vWorldPos;
					float lightDirLength = length(lightDir);
					lightDir /= lightDirLength; // Normalisation
					
					float att = max(Lights[i].parameters1.w - Lights[i].parameters2.x*lightDirLength, 0.0);

					// Ambient
					lightAmbient += att * Lights[i].color.rgb * Lights[i].factors.x;

					// Diffuse
					float lambert = max(dot(normal, lightDir), 0.0);
					
					lightDiffuse += att * lambert * Lights[i].color.rgb * Lights[i].factors.y;
					break;
				}

				case LIGHT_SPOT:
				{
					vec3 lightDir = Lights[i].parameters1.xyz - vWorldPos;
					float lightDirLength = length(lightDir);
					lightDir /= lightDirLength; // Normalisation
					
					float att = max(Lights[i].parameters1.w - Lights[i].parameters2.x*lightDirLength, 0.0);

					// Ambient
					lightAmbient += att * Lights[i].color.rgb * Lights[i].factors.x;

					// Diffuse
					float lambert = max(dot(normal, lightDir), 0.0);

					// Modification de l'atténuation pour gérer le spot
					float curAngle = dot(Lights[i].parameters2.xyz, -lightDir);
					float outerAngle = Lights[i].parameters3.y;
					float innerMinusOuterAngle = Lights[i].parameters3.x - outerAngle;
					att *= max((curAngle - outerAngle) / innerMinusOuterAngle, 0.0);

					lightDiffuse += att * lambert * Lights[i].color.rgb * Lights[i].factors.y;
				}
				
				default:
					break;
			}
		}
	}

	lightAmbient = (lightAmbient + SceneAmbient.rgb)*MaterialAmbient.rgb;
	lightSpecular *= MaterialSpecular.rgb;
		#if SPECULAR_MAPPING
	lightSpecular *= texture(MaterialSpecularMap, texCoord).rgb; // Utiliser l'alpha de MaterialSpecular n'aurait aucun sens
		#endif
		
	vec3 lightColor = (lightAmbient + lightDiffuse + lightSpecular);
	vec4 fragmentColor = vec4(lightColor, 1.0) * diffuseColor;

		#if EMISSIVE_MAPPING
	float lightIntensity = dot(lightColor, vec3(0.3, 0.59, 0.11));

	vec3 emissionColor = MaterialDiffuse.rgb * texture(MaterialEmissiveMap, texCoord).rgb;
	RenderTarget0 = vec4(mix(fragmentColor.rgb, emissionColor, clamp(1.0 - 3.0*lightIntensity, 0.0, 1.0)), fragmentColor.a);
		#else
	RenderTarget0 = fragmentColor;
		#endif // EMISSIVE_MAPPING
	#else
	RenderTarget0 = diffuseColor;
	#endif // LIGHTING
#endif // FLAG_DEFERRED
}
