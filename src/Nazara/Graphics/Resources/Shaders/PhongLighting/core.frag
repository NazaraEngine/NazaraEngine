#if EARLY_FRAGMENT_TESTS && !ALPHA_TEST
layout(early_fragment_tests) in;
#endif

#define LIGHT_DIRECTIONAL 0
#define LIGHT_POINT 1
#define LIGHT_SPOT 2

/********************Entrant********************/
in vec4 vColor;
in vec4 vLightSpacePos[3];
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
	bool shadowMapping;
};

// Lumières
uniform Light Lights[3];
uniform samplerCube PointLightShadowMap[3];
uniform sampler2D DirectionalSpotLightShadowMap[3];

// Matériau
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

// Autres
uniform float ParallaxBias = -0.03;
uniform float ParallaxScale = 0.02;
uniform vec2 InvTargetSize;
uniform vec3 EyePosition;
uniform vec4 SceneAmbient;

uniform sampler2D TextureOverlay;

/********************Fonctions********************/
vec3 FloatToColor(float f)
{
	vec3 color;

	f *= 256.0;
	color.x = floor(f);

	f = (f - color.x) * 256.0;
	color.y = floor(f);

	color.z = f - color.y;
	color.xy *= 0.00390625; // *= 1.0/256

	return color;
}

#define kPI 3.1415926536

vec4 EncodeNormal(in vec3 normal)
{
	//return vec4(normal*0.5 + 0.5, 0.0);
	return vec4(vec2(atan(normal.y, normal.x)/kPI, normal.z), 0.0, 0.0);
}

float VectorToDepthValue(vec3 vec, float zNear, float zFar)
{
	vec3 absVec = abs(vec);
	float localZ = max(absVec.x, max(absVec.y, absVec.z));

	float normZ = ((zFar + zNear) * localZ - (2.0*zFar*zNear)) / ((zFar - zNear)*localZ);
	return (normZ + 1.0) * 0.5;
}

#if SHADOW_MAPPING
float CalculateDirectionalShadowFactor(int lightIndex)
{
	vec4 lightSpacePos = vLightSpacePos[lightIndex];
	return (texture(DirectionalSpotLightShadowMap[lightIndex], lightSpacePos.xy).x >= (lightSpacePos.z - 0.0005)) ? 1.0 : 0.0;
}

float CalculatePointShadowFactor(int lightIndex, vec3 lightToWorld, float zNear, float zFar)
{
	return (texture(PointLightShadowMap[lightIndex], vec3(lightToWorld.x, -lightToWorld.y, -lightToWorld.z)).x >= VectorToDepthValue(lightToWorld, zNear, zFar)) ? 1.0 : 0.0;
}

float CalculateSpotShadowFactor(int lightIndex)
{
	vec4 lightSpacePos = vLightSpacePos[lightIndex];

	float visibility = 1.0;
	float x,y;
	for (y = -3.5; y <= 3.5; y+= 1.0)
		for (x = -3.5; x <= 3.5; x+= 1.0)
			visibility += (textureProj(DirectionalSpotLightShadowMap[lightIndex], lightSpacePos.xyw + vec3(x/1024.0 * lightSpacePos.w, y/1024.0 * lightSpacePos.w, 0.0)).x >= (lightSpacePos.z - 0.0005)/lightSpacePos.w) ? 1.0 : 0.0;

	visibility /= 64.0;
	
	return visibility;
}
#endif

void main()
{
	vec4 diffuseColor = MaterialDiffuse * vColor;

#if AUTO_TEXCOORDS
	vec2 texCoord = gl_FragCoord.xy * InvTargetSize;
#else
	vec2 texCoord = vTexCoord;
#endif

#if PARALLAX_MAPPING
	float height = texture(MaterialHeightMap, texCoord).r;
	float v = height*ParallaxScale + ParallaxBias;

	vec3 viewDir = normalize(vViewDir);
	texCoord += v * viewDir.xy;
#endif

#if DIFFUSE_MAPPING
	diffuseColor *= texture(MaterialDiffuseMap, texCoord);
#endif

#if FLAG_TEXTUREOVERLAY
	diffuseColor *= texture(TextureOverlay, texCoord);
#endif

#if FLAG_DEFERRED
	#if ALPHA_TEST
		// Inutile de faire de l'alpha-mapping sans alpha-test en Deferred (l'alpha n'est pas sauvegardé dans le G-Buffer)
		#if ALPHA_MAPPING
	diffuseColor.a *= texture(MaterialAlphaMap, texCoord).r;
		#endif
		
	if (diffuseColor.a < MaterialAlphaThreshold)
		discard;
	#endif // ALPHA_TEST

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
	Texture0: Diffuse Color + Specular
	Texture1: Normal + Specular
	Texture2: Encoded depth + Shininess
	*/
	RenderTarget0 = vec4(diffuseColor.rgb, dot(specularColor, vec3(0.3, 0.59, 0.11)));
	RenderTarget1 = vec4(EncodeNormal(normal));
	RenderTarget2 = vec4(FloatToColor(gl_FragCoord.z), (MaterialShininess == 0.0) ? 0.0 : max(log2(MaterialShininess), 0.1)/10.5); // http://www.guerrilla-games.com/publications/dr_kz2_rsx_dev07.pdf
#else // FLAG_DEFERRED
	#if ALPHA_MAPPING
	diffuseColor.a *= texture(MaterialAlphaMap, texCoord).r;
	#endif

	#if ALPHA_TEST
	if (diffuseColor.a < MaterialAlphaThreshold)
		discard;
	#endif

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
			vec4 lightColor = Lights[i].color;
			float lightAmbientFactor = Lights[i].factors.x;
			float lightDiffuseFactor = Lights[i].factors.y;

			switch (Lights[i].type)
			{
				case LIGHT_DIRECTIONAL:
				{
					vec3 lightDir = -Lights[i].parameters1.xyz;

					// Ambient
					lightAmbient += lightColor.rgb * lightAmbientFactor * (MaterialAmbient.rgb + SceneAmbient.rgb);

					float att = 1.0;

					#if SHADOW_MAPPING
					if (Lights[i].shadowMapping)
					{
						float shadowFactor = CalculateDirectionalShadowFactor(i);
						if (shadowFactor == 0.0)
							break;
							
						att *= shadowFactor;
					}
					#endif

					// Diffuse
					float lambert = max(dot(normal, lightDir), 0.0);

					lightDiffuse += att * lambert * lightColor.rgb * lightDiffuseFactor;

					// Specular
					vec3 reflection = reflect(-lightDir, normal);
					float specularFactor = max(dot(reflection, eyeVec), 0.0);
					specularFactor = pow(specularFactor, MaterialShininess);

					lightSpecular += att * specularFactor * lightColor.rgb;
					break;
				}

				case LIGHT_POINT:
				{
					vec3 lightPos = Lights[i].parameters1.xyz;
					float lightAttenuation = Lights[i].parameters1.w;
					float lightInvRadius = Lights[i].parameters2.w;
					
					vec3 worldToLight = lightPos - vWorldPos;
					float lightDirLength = length(worldToLight);
					vec3 lightDir = worldToLight / lightDirLength; // Normalisation
					
					float att = max(lightAttenuation - lightInvRadius * lightDirLength, 0.0);

					// Ambient
					lightAmbient += att * lightColor.rgb * lightAmbientFactor * (MaterialAmbient.rgb + SceneAmbient.rgb);

					#if SHADOW_MAPPING
					if (Lights[i].shadowMapping)
					{
						float shadowFactor = CalculatePointShadowFactor(i, vWorldPos - lightPos, 0.1, 50.0);
						if (shadowFactor == 0.0)
							break;
							
						att *= shadowFactor;
					}
					#endif

					// Diffuse
					float lambert = max(dot(normal, lightDir), 0.0);
					
					lightDiffuse += att * lambert * lightColor.rgb * lightDiffuseFactor;

					// Specular
					vec3 reflection = reflect(-lightDir, normal);
					float specularFactor = max(dot(reflection, eyeVec), 0.0);
					specularFactor = pow(specularFactor, MaterialShininess);

					lightSpecular += att * specularFactor * lightColor.rgb;
					break;
				}

				case LIGHT_SPOT:
				{
					vec3 lightPos = Lights[i].parameters1.xyz;
					vec3 lightDir = Lights[i].parameters2.xyz;
					float lightAttenuation = Lights[i].parameters1.w;
					float lightInvRadius = Lights[i].parameters2.w;
					float lightInnerAngle = Lights[i].parameters3.x;
					float lightOuterAngle = Lights[i].parameters3.y;

					vec3 worldToLight = lightPos - vWorldPos;
					float lightDistance = length(worldToLight);
					worldToLight /= lightDistance; // Normalisation
					
					float att = max(lightAttenuation - lightInvRadius * lightDistance, 0.0);

					// Ambient
					lightAmbient += att * lightColor.rgb * lightAmbientFactor * (MaterialAmbient.rgb + SceneAmbient.rgb);

					#if SHADOW_MAPPING
					if (Lights[i].shadowMapping)
					{
						float shadowFactor = CalculateSpotShadowFactor(i);
						if (shadowFactor == 0.0)
							break;
							
						att *= shadowFactor;
					}
					#endif

					// Modification de l'atténuation pour gérer le spot
					float curAngle = dot(lightDir, -worldToLight);
					float innerMinusOuterAngle = lightInnerAngle - lightOuterAngle;
					att *= max((curAngle - lightOuterAngle) / innerMinusOuterAngle, 0.0);

					// Diffuse
					float lambert = max(dot(normal, worldToLight), 0.0);

					lightDiffuse += att * lambert * lightColor.rgb * lightDiffuseFactor;

					// Specular
					vec3 reflection = reflect(-worldToLight, normal);
					float specularFactor = max(dot(reflection, eyeVec), 0.0);
					specularFactor = pow(specularFactor, MaterialShininess);

					lightSpecular += att * specularFactor * lightColor.rgb;
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
			vec4 lightColor = Lights[i].color;
			float lightAmbientFactor = Lights[i].factors.x;
			float lightDiffuseFactor = Lights[i].factors.y;

			switch (Lights[i].type)
			{
				case LIGHT_DIRECTIONAL:
				{
					vec3 lightDir = -Lights[i].parameters1.xyz;

					// Ambient
					lightAmbient += lightColor.rgb * lightAmbientFactor * (MaterialAmbient.rgb + SceneAmbient.rgb);

					float att = 1.0;

					#if SHADOW_MAPPING
					if (Lights[i].shadowMapping)
					{
						float shadowFactor = CalculateDirectionalShadowFactor(i);
						if (shadowFactor == 0.0)
							break;
							
						att *= shadowFactor;
					}
					#endif

					// Diffuse
					float lambert = max(dot(normal, lightDir), 0.0);

					lightDiffuse += att * lambert * lightColor.rgb * lightDiffuseFactor;
					break;
				}

				case LIGHT_POINT:
				{
					vec3 lightPos = Lights[i].parameters1.xyz;
					float lightAttenuation = Lights[i].parameters1.w;
					float lightInvRadius = Lights[i].parameters2.w;
					
					vec3 worldToLight = lightPos - vWorldPos;
					float lightDirLength = length(worldToLight);
					vec3 lightDir = worldToLight / lightDirLength; // Normalisation
					
					float att = max(lightAttenuation - lightInvRadius * lightDirLength, 0.0);

					// Ambient
					lightAmbient += att * lightColor.rgb * lightAmbientFactor * (MaterialAmbient.rgb + SceneAmbient.rgb);

					#if SHADOW_MAPPING
					if (Lights[i].shadowMapping)
					{
						float shadowFactor = CalculatePointShadowFactor(i, vWorldPos - lightPos, 0.1, 50.0);
						if (shadowFactor == 0.0)
							break;
							
						att *= shadowFactor;
					}
					#endif

					// Diffuse
					float lambert = max(dot(normal, lightDir), 0.0);
					
					lightDiffuse += att * lambert * lightColor.rgb * lightDiffuseFactor;
					break;
				}

				case LIGHT_SPOT:
				{
					vec3 lightPos = Lights[i].parameters1.xyz;
					vec3 lightDir = Lights[i].parameters2.xyz;
					float lightAttenuation = Lights[i].parameters1.w;
					float lightInvRadius = Lights[i].parameters2.w;
					float lightInnerAngle = Lights[i].parameters3.x;
					float lightOuterAngle = Lights[i].parameters3.y;

					vec3 worldToLight = lightPos - vWorldPos;
					float lightDistance = length(worldToLight);
					worldToLight /= lightDistance; // Normalisation
					
					float att = max(lightAttenuation - lightInvRadius * lightDistance, 0.0);

					// Ambient
					lightAmbient += att * lightColor.rgb * lightAmbientFactor * (MaterialAmbient.rgb + SceneAmbient.rgb);

					#if SHADOW_MAPPING
					if (Lights[i].shadowMapping)
					{
						float shadowFactor = CalculateSpotShadowFactor(i);
						if (shadowFactor == 0.0)
							break;
							
						att *= shadowFactor;
					}
					#endif

					// Modification de l'atténuation pour gérer le spot
					float curAngle = dot(lightDir, -worldToLight);
					float innerMinusOuterAngle = lightInnerAngle - lightOuterAngle;
					att *= max((curAngle - lightOuterAngle) / innerMinusOuterAngle, 0.0);

					// Diffuse
					float lambert = max(dot(normal, worldToLight), 0.0);

					lightDiffuse += att * lambert * lightColor.rgb * lightDiffuseFactor;
				}
				
				default:
					break;
			}
		}
	}
	
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
#endif // FLAG_DEFERRED
}

