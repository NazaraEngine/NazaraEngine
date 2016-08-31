#version 140

out vec4 RenderTarget0;

uniform float exposure = 0.92;
uniform sampler2D AccumulationTexture;
uniform sampler2D ColorTexture;
uniform vec2 InvTargetSize;

void main()
{
	vec2 texCoord = gl_FragCoord.xy * InvTargetSize;

	vec4 color = texture(ColorTexture, texCoord);
	vec4 lightScattering = texture(AccumulationTexture, texCoord);

	RenderTarget0 = vec4(lightScattering.rgb * exposure, 1.0) + color * 1.1;
}
