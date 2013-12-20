#version 140

out vec4 RenderTarget0;

uniform float BrightLuminance = 0.8;
uniform float BrightMiddleGrey = 0.5;
uniform float BrightThreshold = 0.8;
uniform sampler2D ColorTexture;
uniform vec2 InvTargetSize;

void main()
{
	vec2 texCoord = gl_FragCoord.xy * InvTargetSize;

	vec3 color = textureLod(ColorTexture, texCoord, 0.0).rgb;

	color *= BrightMiddleGrey/BrightLuminance;
	color *= 1.0 + (color / (BrightThreshold*BrightThreshold));
	color -= 0.5;
	color /= (1.0 + color);

	RenderTarget0 = vec4(color, 1.0);
}
