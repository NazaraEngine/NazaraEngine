#version 140

out vec4 RenderTarget0;

uniform float FXAAReduceMul = 0.0; // 1.0/8.0
uniform float FXAASpanMax = 8.0;
uniform sampler2D ColorTexture;
uniform vec2 InvTargetSize;

void main()
{
    #define FXAA_REDUCE_MIN   (1.0/128.0)

	vec2 texCoord = gl_FragCoord.xy * InvTargetSize;

    vec3 rgbNW = textureLodOffset(ColorTexture, texCoord, 0.0, ivec2(-1,-1)).rgb;
    vec3 rgbNE = textureLodOffset(ColorTexture, texCoord, 0.0, ivec2(1,-1)).rgb;
    vec3 rgbSW = textureLodOffset(ColorTexture, texCoord, 0.0, ivec2(-1,1)).rgb;
    vec3 rgbSE = textureLodOffset(ColorTexture, texCoord, 0.0, ivec2(1,1)).rgb;
    vec3 rgbM  = textureLod(ColorTexture, texCoord, 0.0).rgb;

    vec3 luma = vec3(0.299, 0.587, 0.114);
    float lumaNW = dot(rgbNW, luma);
    float lumaNE = dot(rgbNE, luma);
    float lumaSW = dot(rgbSW, luma);
    float lumaSE = dot(rgbSE, luma);
    float lumaM  = dot(rgbM,  luma);

    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

    vec2 dir; 
    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));

    float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAAReduceMul), FXAA_REDUCE_MIN);
    float rcpDirMin = 1.0/(min(abs(dir.x), abs(dir.y)) + dirReduce);
    dir = min(vec2(FXAASpanMax, FXAASpanMax), max(vec2(-FXAASpanMax, -FXAASpanMax), dir * rcpDirMin)) * InvTargetSize;

    vec3 rgbA = (1.0/2.0) * (textureLod(ColorTexture, texCoord + dir * (1.0/3.0 - 0.5), 0.0).rgb + textureLod(ColorTexture, texCoord + dir * (2.0/3.0 - 0.5), 0.0).rgb);
    vec3 rgbB = rgbA * 1.0/2.0 + 1.0/4.0 * (textureLod(ColorTexture, texCoord + dir * (0.0/3.0 - 0.5), 0.0).rgb + textureLod(ColorTexture, texCoord + dir * (3.0/3.0 - 0.5), 0.0).rgb);
    float lumaB = dot(rgbB, luma);

	vec3 fragmentColor = (lumaB < lumaMin || lumaB > lumaMax) ? rgbA : rgbB;

	RenderTarget0 = vec4(fragmentColor, 1.0);
}