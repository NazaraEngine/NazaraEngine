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
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;

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

#if LIGHTING
	vec3 lightColor = vec3(0.0);

	#if SPECULAR_MAPPING
	vec3 specularColor = vec3(0.0);
	#endif

	#if NORMAL_MAPPING
	vec3 normal = normalize(vLightToWorld * (2.0 * vec3(texture2D(MaterialNormalMap, vTexCoord)) - 1.0));
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
				vec3 lightDir = normalize(-Lights[i].parameters1.xyz);
				
				// Ambient
				lightColor += Lights[i].ambient.rgb * (MaterialAmbient.rgb + SceneAmbient.rgb);

				// Diffuse
				float lambert = max(dot(normal, lightDir), 0.0);
				lightColor += lambert * Lights[i].diffuse.rgb * MaterialDiffuse.rgb;

				// Specular
				vec3 reflection = reflect(-lightDir, normal);
				float specular = pow(max(dot(reflection, eyeVec), 0.0), MaterialShininess);

	#if SPECULAR_MAPPING
				specularColor += specular * Lights[i].specular.rgb * MaterialSpecular.rgb;
	#else
				lightColor += specular * Lights[i].specular.rgb * MaterialSpecular.rgb;
	#endif
			}
			else if (Lights[i].type == LIGHT_POINT)
			{
				vec3 lightDir = Lights[i].parameters1.xyz - vWorldPos;
				float att = max(Lights[i].parameters1.w - Lights[i].parameters2.x*length(lightDir), 0.0);
				lightDir = normalize(lightDir);

				// Ambient
				lightColor += att * Lights[i].ambient.rgb * (MaterialAmbient.rgb + SceneAmbient.rgb);

				// Diffuse
				float lambert = max(dot(normal, lightDir), 0.0);
				lightColor += att * lambert * Lights[i].diffuse.rgb * MaterialDiffuse.rgb;

				// Specular
				vec3 reflection = reflect(-lightDir, normal);
				float specular = pow(max(dot(reflection, eyeVec), 0.0), MaterialShininess);

	#if SPECULAR_MAPPING
				specularColor += att * specular * Lights[i].specular.rgb * MaterialSpecular.rgb;
	#else
				lightColor += att * specular * Lights[i].specular.rgb * MaterialSpecular.rgb;
	#endif
			}
			else if (Lights[i].type == LIGHT_SPOT)
			{
				vec3 lightDir = Lights[i].parameters1.xyz - vWorldPos;
				float att = max(Lights[i].parameters1.w - Lights[i].parameters2.x*length(lightDir), 0.0);
				lightDir = normalize(lightDir);

				// Ambient
				lightColor += att * Lights[i].ambient.rgb * (MaterialAmbient.rgb + SceneAmbient.rgb);

				// Modification de l'atténuation
				float curAngle = dot(Lights[i].parameters2.xyz, -lightDir);
				float outerAngle = Lights[i].parameters3.y;
				float innerMinusOuterAngle = Lights[i].parameters3.x - outerAngle;
				float lambert = max(dot(normal, lightDir), 0.0);
				att *= max((curAngle - outerAngle) / innerMinusOuterAngle, 0.0);

				// Diffuse
				lightColor += att * lambert * Lights[i].diffuse.rgb * MaterialDiffuse.rgb;

				// Specular
				vec3 reflection = reflect(-lightDir, normal);
				float specular = pow(max(dot(reflection, eyeVec), 0.0), MaterialShininess);

	#if SPECULAR_MAPPING
				specularColor += att * specular * Lights[i].specular.rgb * MaterialSpecular.rgb;
	#else
				lightColor += att * specular * Lights[i].specular.rgb * MaterialSpecular.rgb;
	#endif
			}
		}
	}
	else
	{
		for (int i = 0; i < 3; ++i)
		{
			if (Lights[i].type == LIGHT_DIRECTIONAL)
			{
				vec3 lightDir = normalize(-Lights[i].parameters1.xyz);
				
				// Ambient
				lightColor += Lights[i].ambient.rgb * (MaterialAmbient.rgb + SceneAmbient.rgb);

				// Diffuse
				float lambert = max(dot(normal, lightDir), 0.0);
				lightColor += lambert * Lights[i].diffuse.rgb * MaterialDiffuse.rgb;
			}
			else if (Lights[i].type == LIGHT_POINT)
			{
				vec3 lightDir = Lights[i].parameters1.xyz - vWorldPos;
				float att = max(Lights[i].parameters1.w - Lights[i].parameters2.x*length(lightDir), 0.0);
				lightDir = normalize(lightDir);

				// Ambient
				lightColor += att * Lights[i].ambient.rgb * (MaterialAmbient.rgb + SceneAmbient.rgb);

				// Diffuse
				float lambert = max(dot(normal, lightDir), 0.0);
				lightColor += att * lambert * Lights[i].diffuse.rgb * MaterialDiffuse.rgb;
			}
			else if (Lights[i].type == LIGHT_SPOT)
			{
				vec3 lightDir = Lights[i].parameters1.xyz - vWorldPos;
				float att = max(Lights[i].parameters1.w - Lights[i].parameters2.x*length(lightDir), 0.0);
				lightDir = normalize(lightDir);

				// Ambient
				lightColor += att * Lights[i].ambient.rgb * (MaterialAmbient.rgb + SceneAmbient.rgb);

				// Modification de l'atténuation
				float curAngle = dot(Lights[i].parameters2.xyz, -lightDir);
				float outerAngle = Lights[i].parameters3.y;
				float innerMinusOuterAngle = Lights[i].parameters3.x - outerAngle;
				float lambert = max(dot(normal, lightDir), 0.0);
				att *= max((curAngle - outerAngle) / innerMinusOuterAngle, 0.0);

				// Diffuse
				lightColor += att * lambert * Lights[i].diffuse.rgb * MaterialDiffuse.rgb;
			}
		}
	}

	fragmentColor *= vec4(lightColor, 1.0);
	#if SPECULAR_MAPPING
	fragmentColor *= vec4(specularColor * texture2D(MaterialSpecularMap, vTexCoord).rgb, 1.0); // Utiliser l'alpha de MaterialSpecular n'aurait aucun sens
	#endif

	#if EMISSIVE_MAPPING
		#if SPECULAR_MAPPING
	float lightIntensity = dot(lightColor + specularColor, vec3(0.3, 0.59, 0.11));
		#else
	float lightIntensity = dot(lightColor, vec3(0.3, 0.59, 0.11));
		#endif // SPECULAR_MAPPING

	vec3 emissionColor = MaterialDiffuse.rgb * texture2D(MaterialEmissiveMap, vTexCoord).rgb;
	gl_FragColor = vec4(mix(fragmentColor.rgb, emissionColor, clamp(1.0 - 3.0*lightIntensity, 0.0, 1.0)), fragmentColor.a);
	#else
	gl_FragColor = fragmentColor;
	#endif // EMISSIVE_MAPPING
#else
	gl_FragColor = fragmentColor;
#endif // LIGHTING
}
