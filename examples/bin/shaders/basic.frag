#version 110

varying vec2 vTexCoord;

uniform sampler2D DiffuseMap;

void main()
{
	gl_FragColor = texture2D(DiffuseMap, vTexCoord);
}
