#version 140

out vec4 RenderTarget0;

uniform sampler2D ColorTexture;
uniform sampler2D SSAOTexture;
uniform vec2 InvTargetSize;

void main()
{
	vec2 texCoord = gl_FragCoord.xy * InvTargetSize;

	vec3 color = textureLod(ColorTexture, texCoord, 0.0).rgb;
	float ssao = textureLod(SSAOTexture, texCoord, 0.0).r;

	RenderTarget0 = vec4(color*ssao, 1.0);
}
