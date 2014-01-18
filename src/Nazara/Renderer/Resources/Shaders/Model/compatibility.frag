#if FLAG_DEFERRED
	#error Deferred Shading needs core profile
#endif

#define LIGHT_DIRECTIONAL 0
#define LIGHT_POINT 1
#define LIGHT_SPOT 2

/********************Entrant********************/
varying mat3 vLightToWorld;
varying vec3 vNormal;
varying vec2 vTexCoord;
varying vec3 vWorldPos;

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
uniform sampler2D MaterialNormalMap;
uniform float MaterialShininess;
uniform vec4 MaterialSpecular;
uniform sampler2D MaterialSpecularMap;

uniform vec4 SceneAmbient;

/********************Fonctions********************/
void main()
{
	vec4 diffuseColor = MaterialDiffuse;
#if DIFFUSE_MAPPING
	diffuseColor *= texture(MaterialDiffuseMap, vTexCoord);
#endif

#if ALPHA_MAPPING
	diffuseColor.a *= texture(MaterialAlphaMap, vTexCoord).r;
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
	vec3 normal = normalize(vLightToWorld * (2.0 * vec3(texture(MaterialNormalMap, vTexCoord)) - 1.0));
	#else
	vec3 normal = normalize(vNormal);
	#endif

	if (MaterialShininess > 0.0)
	{
		vec3 eyeVec = normalize(EyePosition - vWorldPos);

		for (int i = 0; i < 3; ++i)
		{
			if (Lights[i].type == LIGHT_DIRECTIONAL)
			{
				vec3 lightDir = -Lights[i].parameters1.xyz;

				// Ambient
				lightAmbient += Lights[i].color.rgb * Lights[i].factors.x * (MaterialAmbient.rgb + SceneAmbient.rgb);

				// Diffuse
				float lambert = max(dot(normal, lightDir), 0.0);

				lightDiffuse += lambert * Lights[i].color.rgb * Lights[i].factors.y;

				// Specular
				vec3 reflection = reflect(-lightDir, normal);
				float specularFactor = max(dot(reflection, eyeVec), 0.0);
				specularFactor = pow(specularFactor, MaterialShininess);

				lightSpecular += specularFactor * Lights[i].color.rgb;
			}
			else if (Lights[i].type == LIGHT_POINT)
			{
				vec3 lightDir = Lights[i].parameters1.xyz - vWorldPos;
				float lightDirLength = length(lightDir);
				lightDir /= lightDirLength; // Normalisation
				
				float att = max(Lights[i].parameters1.w - Lights[i].parameters2.w*lightDirLength, 0.0);

				// Ambient
				lightAmbient += att * Lights[i].color.rgb * Lights[i].factors.x * (MaterialAmbient.rgb + SceneAmbient.rgb);

				// Diffuse
				float lambert = max(dot(normal, lightDir), 0.0);
				
				lightDiffuse += att * lambert * Lights[i].color.rgb * Lights[i].factors.y;

				// Specular
				vec3 reflection = reflect(-lightDir, normal);
				float specularFactor = max(dot(reflection, eyeVec), 0.0);
				specularFactor = pow(specularFactor, MaterialShininess);

				lightSpecular += att * specularFactor * Lights[i].color.rgb;
			}
			else if (Lights[i].type == LIGHT_SPOT)
			{
				vec3 lightDir = Lights[i].parameters1.xyz - vWorldPos;
				float lightDirLength = length(lightDir);
				lightDir /= lightDirLength; // Normalisation
				
				float att = max(Lights[i].parameters1.w - Lights[i].parameters2.w*lightDirLength, 0.0);

				// Ambient
				lightAmbient += att * Lights[i].color.rgb * Lights[i].factors.x * (MaterialAmbient.rgb + SceneAmbient.rgb);

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
			}
		}
	}
	else
	{
		for (int i = 0; i < 3; ++i)
		{
			if (Lights[i].type == LIGHT_DIRECTIONAL)
			{
				vec3 lightDir = -Lights[i].parameters1.xyz;

				// Ambient
				lightAmbient += Lights[i].color.rgb * Lights[i].factors.x * (MaterialAmbient.rgb + SceneAmbient.rgb);

				// Diffuse
				float lambert = max(dot(normal, lightDir), 0.0);

				lightDiffuse += lambert * Lights[i].color.rgb * Lights[i].factors.y;
			}
			else if (Lights[i].type == LIGHT_POINT)
			{
				vec3 lightDir = Lights[i].parameters1.xyz - vWorldPos;
				float lightDirLength = length(lightDir);
				lightDir /= lightDirLength; // Normalisation
				
				float att = max(Lights[i].parameters1.w - Lights[i].parameters2.w*lightDirLength, 0.0);

				// Ambient
				lightAmbient += att * Lights[i].color.rgb * Lights[i].factors.x * (MaterialAmbient.rgb + SceneAmbient.rgb);

				// Diffuse
				float lambert = max(dot(normal, lightDir), 0.0);
				
				lightDiffuse += att * lambert * Lights[i].color.rgb * Lights[i].factors.y;
			}
			else if (Lights[i].type == LIGHT_SPOT)
			{
				vec3 lightDir = Lights[i].parameters1.xyz - vWorldPos;
				float lightDirLength = length(lightDir);
				lightDir /= lightDirLength; // Normalisation
				
				float att = max(Lights[i].parameters1.w - Lights[i].parameters2.w*lightDirLength, 0.0);

				// Ambient
				lightAmbient += att * Lights[i].color.rgb * Lights[i].factors.x * (MaterialAmbient.rgb + SceneAmbient.rgb);

				// Diffuse
				float lambert = max(dot(normal, lightDir), 0.0);

				// Modification de l'atténuation pour gérer le spot
				float curAngle = dot(Lights[i].parameters2.xyz, -lightDir);
				float outerAngle = Lights[i].parameters3.y;
				float innerMinusOuterAngle = Lights[i].parameters3.x - outerAngle;
				att *= max((curAngle - outerAngle) / innerMinusOuterAngle, 0.0);

				lightDiffuse += att * lambert * Lights[i].color.rgb * Lights[i].factors.y;
			}
		}
	}

	lightSpecular *= MaterialSpecular.rgb;
	#if SPECULAR_MAPPING
	lightSpecular *= texture(MaterialSpecularMap, vTexCoord).rgb; // Utiliser l'alpha de MaterialSpecular n'aurait aucun sens
	#endif
		
	vec3 lightColor = (lightAmbient + lightDiffuse + lightSpecular);
	vec4 fragmentColor = vec4(lightColor, 1.0) * diffuseColor;

	#if EMISSIVE_MAPPING
	float lightIntensity = dot(lightColor, vec3(0.3, 0.59, 0.11));

	vec3 emissionColor = MaterialDiffuse.rgb * texture(MaterialEmissiveMap, vTexCoord).rgb;
	RenderTarget0 = vec4(mix(fragmentColor.rgb, emissionColor, clamp(1.0 - 3.0*lightIntensity, 0.0, 1.0)), fragmentColor.a);
	#else
	RenderTarget0 = fragmentColor;
	#endif // EMISSIVE_MAPPING
#else
	RenderTarget0 = diffuseColor;
#endif // LIGHTING
}
