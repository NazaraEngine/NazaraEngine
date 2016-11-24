#version 140

out vec4 RenderTarget0;

uniform vec3 EyePosition;

uniform vec4 LightColor;
uniform vec2 LightFactors;
uniform vec4 LightDirection;

uniform sampler2D GBuffer0;
uniform sampler2D GBuffer1;
uniform sampler2D GBuffer2;
uniform sampler2D DepthBuffer;

uniform mat4 InvViewProjMatrix;
uniform vec2 InvTargetSize;
uniform vec4 SceneAmbient;

#define kPI 3.1415926536

vec3 DecodeNormal(in vec4 encodedNormal)
{
	//return encodedNormal.xyz*2.0 - 1.0;	
	float a = encodedNormal.x * kPI;
    vec2 scth = vec2(sin(a), cos(a));

    vec2 scphi = vec2(sqrt(1.0 - encodedNormal.y*encodedNormal.y), encodedNormal.y);
    return vec3(scth.y*scphi.x, scth.x*scphi.x, scphi.y);
}

void main()
{
	vec2 texCoord = gl_FragCoord.xy * InvTargetSize;
	vec4 gVec0 = textureLod(GBuffer0, texCoord, 0.0);
	vec4 gVec1 = textureLod(GBuffer1, texCoord, 0.0);
	vec4 gVec2 = textureLod(GBuffer2, texCoord, 0.0);

	vec3 diffuseColor = gVec0.xyz;
	vec3 normal = DecodeNormal(gVec1);
	float specularMultiplier = gVec0.w;
	float depth = textureLod(DepthBuffer, texCoord, 0.0).r;
	float shininess = (gVec2.w == 0.0) ? 0.0 : exp2(gVec2.w*10.5);

	vec3 lightDir = -LightDirection.xyz;

	// Ambient
	vec3 lightAmbient = LightColor.rgb * LightFactors.x * (vec3(1.0) + SceneAmbient.rgb);

	// Diffuse
	float lambert = max(dot(normal, lightDir), 0.0);

	vec3 lightDiffuse = lambert * LightColor.rgb * LightFactors.y;

	// Specular
	vec3 lightSpecular;
	if (shininess > 0.0)
	{
		vec3 viewSpace = vec3(texCoord*2.0 - 1.0, depth*2.0 - 1.0);

		vec4 worldPos = InvViewProjMatrix * vec4(viewSpace, 1.0);
		worldPos.xyz /= worldPos.w;

		vec3 eyeVec = normalize(EyePosition - worldPos.xyz);

		vec3 reflection = reflect(-lightDir, normal);
		float specularFactor = max(dot(reflection, eyeVec), 0.0);
		specularFactor = pow(specularFactor, shininess);

		lightSpecular = specularFactor * LightColor.rgb * specularMultiplier;
	}
	else
		lightSpecular = vec3(0.0);

	vec3 fragmentColor = diffuseColor * (lightAmbient + lightDiffuse + lightSpecular);
	RenderTarget0 = vec4(fragmentColor, 1.0);
}