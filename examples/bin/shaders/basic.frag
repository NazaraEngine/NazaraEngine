#version 110

varying vec2 TexCoord;

uniform sampler2D diffuseMap;

void main()
{
	gl_FragColor = texture2D(diffuseMap, TexCoord);
}
