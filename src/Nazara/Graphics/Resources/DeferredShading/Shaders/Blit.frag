#version 140

out vec4 RenderTarget0;

uniform sampler2D ColorTexture;
uniform vec2 InvTargetSize;

void main()
{
	vec2 texCoord = gl_FragCoord.xy * InvTargetSize;
	RenderTarget0 = textureLod(ColorTexture, texCoord, 0.0);
}
