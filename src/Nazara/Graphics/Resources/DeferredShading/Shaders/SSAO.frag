#version 140

out vec4 RenderTarget0;

uniform float SSAOBias = 0.0;
uniform float SSAOIntensity = 3.0;
uniform float SSAOSampleScale = 0.1;
uniform float SSAOScale = 1.0;
uniform int NoiseTextureSize;
uniform mat4 InvViewProjMatrix;
uniform sampler2D GBuffer1;
uniform sampler2D NoiseTexture;
uniform vec2 TargetSize;
uniform vec2 InvTargetSize;

vec3 extractPosition(in float depth, in vec2 texCoord)
{
	depth = depth*2.0 - 1.0;

	vec3 viewSpace = vec3(texCoord*2.0 - 1.0, depth);

	vec4 worldPos = InvViewProjMatrix * vec4(viewSpace, 1.0);
	worldPos.xyz /= worldPos.w;

	return worldPos.xyz;
}

float doAmbientOcclusion(in vec2 texCoord, in vec3 original, in vec3 normal)
{
	vec3 newp = extractPosition(textureLod(GBuffer1, texCoord, 0.0).w, texCoord);
	vec3 diff = newp - original;
	float d = length(diff);
	vec3 v = diff * 1.0/d;
	d *= SSAOScale;

	return max(0.0, dot(normal, v) - SSAOBias) * (SSAOIntensity / (1.0 + d));
}

void main(void)
{
	const vec2 Kernel[16] = vec2[](
		vec2(0.53812504, 0.18565957),
		vec2(0.13790712, 0.24864247),
		vec2(0.33715037, 0.56794053),
		vec2(-0.6999805, -0.04511441),
		vec2(0.06896307, -0.15983082),
		vec2(0.056099437, 0.006954967),
		vec2(-0.014653638, 0.14027752),
		vec2(0.010019933, -0.1924225),
		vec2(-0.35775623, -0.5301969),
		vec2(-0.3169221, 0.106360726),
		vec2(0.010350345, -0.58698344),
		vec2(-0.08972908, -0.49408212),
		vec2(0.7119986, -0.0154690035),
		vec2(-0.053382345, 0.059675813),
		vec2(0.035267662, -0.063188605),
		vec2(-0.47761092, 0.2847911));

	vec2 texCoord = gl_FragCoord.xy * InvTargetSize;
	vec4 gVec1 = textureLod(GBuffer1, texCoord, 0.0);
	
	if (gVec1.w == 1.0)
	{
		RenderTarget0 = vec4(1.0, 0.0, 0.0, 0.0);
		return;
	}
	
	vec3 normal = gVec1.xyz*2.0 - 1.0;

	vec3 viewPos = extractPosition(gVec1.w, texCoord);
	vec2 randVec = normalize(textureLod(NoiseTexture, texCoord * (TargetSize/NoiseTextureSize), 0.0).xy * 2.0 - 1.0);

	float ao = 0.0;
	const int ITERATIONS = 16;
	for (int i = 0; i < ITERATIONS; ++i)
	{
		vec2 coord = reflect(Kernel[i], randVec) * SSAOSampleScale;
		ao += doAmbientOcclusion(texCoord + coord, viewPos, normal);
	}

	RenderTarget0 = vec4(1.0 - ao/ITERATIONS, 0.0, 0.0, 0.0);
}