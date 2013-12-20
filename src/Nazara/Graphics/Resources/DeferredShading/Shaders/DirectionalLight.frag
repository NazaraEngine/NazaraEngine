#version 140

out vec4 RenderTarget0;

struct Light
{
	int type;
	vec4 ambient;
	vec4 color;
	vec2 factors;

	vec4 parameters1;
	vec4 parameters2;
	vec2 parameters3;
};

uniform vec3 EyePosition;
uniform Light Lights[1];

uniform sampler2D GBuffer0;
uniform sampler2D GBuffer1;
uniform sampler2D GBuffer2;

uniform mat4 InvViewProjMatrix;
uniform vec2 InvTargetSize;
uniform vec4 SceneAmbient;

void main()
{
	vec2 texCoord = gl_FragCoord.xy * InvTargetSize;
	vec4 gVec0 = textureLod(GBuffer0, texCoord, 0.0);
	if (gVec0.w == 0.0)
	{
		RenderTarget0 = vec4(gVec0.xyz, 1.0);
		return;
	}

	vec4 gVec1 = textureLod(GBuffer1, texCoord, 0.0);
	vec4 gVec2 = textureLod(GBuffer2, texCoord, 0.0);

	vec3 diffuseColor = gVec0.xyz;
	vec3 normal = gVec1.xyz*2.0 - 1.0;
	vec3 specularColor = gVec2.xyz;
	float depth = gVec1.w*2.0 - 1.0;
	float shininess = (gVec2.w == 0.0) ? 0.0 : exp2(gVec2.w*10.5);

	vec3 lightDir = -Lights[0].parameters1.xyz;

	// Ambient
	vec3 lightAmbient = Lights[0].color.rgb * Lights[0].factors.x * (vec3(1.0) + SceneAmbient.rgb);

	// Diffuse
	float lambert = max(dot(normal, lightDir), 0.0);

	vec3 lightDiffuse = lambert * Lights[0].color.rgb * Lights[0].factors.y;

	// Specular
	vec3 lightSpecular = vec3(0.0);
	if (shininess > 0.0)
	{
		vec3 viewSpace = vec3(texCoord*2.0 - 1.0, depth);

		vec4 worldPos = InvViewProjMatrix * vec4(viewSpace, 1.0);
		worldPos.xyz /= worldPos.w;

		vec3 eyeVec = normalize(EyePosition - worldPos.xyz);

		vec3 reflection = reflect(-lightDir, normal);
		float specularFactor = max(dot(reflection, eyeVec), 0.0);
		specularFactor = pow(specularFactor, shininess);

		lightSpecular = specularFactor * Lights[0].color.rgb;
	}

	lightSpecular *= specularColor;

	vec3 fragmentColor = diffuseColor * (lightAmbient + lightDiffuse + lightSpecular);
	RenderTarget0 = vec4(fragmentColor, 1.0);
}