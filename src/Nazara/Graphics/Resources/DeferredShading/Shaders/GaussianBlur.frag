// http://www.geeks3d.com/20100909/shader-library-gaussian-blur-post-processing-filter-in-glsl/
#version 140

out vec4 RenderTarget0;

uniform sampler2D ColorTexture;
uniform vec2 Filter;
uniform vec2 InvTargetSize;

float offset[3] = float[](0.0, 1.3846153846, 3.2307692308);
float weight[3] = float[](0.2270270270, 0.3162162162, 0.0702702703);

void main()
{
	vec2 texCoord = gl_FragCoord.xy * InvTargetSize;
	vec3 color = textureLod(ColorTexture, texCoord, 0.0).rgb * weight[0];

	for (int i = 1; i < 3; i++)
	{
		color += textureLod(ColorTexture, texCoord + Filter*vec2(offset[i])*InvTargetSize, 0.0).rgb * weight[i];
		color += textureLod(ColorTexture, texCoord - Filter*vec2(offset[i])*InvTargetSize, 0.0).rgb * weight[i];
	}

	RenderTarget0 = vec4(color, 1.0);
}
