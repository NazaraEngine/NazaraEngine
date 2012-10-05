#version 110

varying vec2 TexCoord;

uniform sampler2D texture;

void main()
{
	gl_FragColor = texture2D(texture, TexCoord);
}
