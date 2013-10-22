#version 140

out vec4 RenderTarget0;

uniform sampler2D BloomTexture;
uniform sampler2D ColorTexture;
uniform vec2 InvTargetSize;

void main()
{
	vec2 texCoord = gl_FragCoord.xy * InvTargetSize;

	vec3 bloomColor = textureLod(BloomTexture, texCoord, 0.0).rgb;
	vec3 originalColor = textureLod(ColorTexture, texCoord, 0.0).rgb;

	RenderTarget0 = vec4(originalColor + bloomColor, 1.0);
}
