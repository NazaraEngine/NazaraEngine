#version 110

/********************Uniformes********************/
layout(location = 0) uniform vec4 Color;

/********************Fonctions********************/
void main()
{
	gl_FragColor = Color;
}