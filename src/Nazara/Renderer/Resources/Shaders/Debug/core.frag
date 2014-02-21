#version 140

/********************Sortant********************/
out vec4 RenderTarget0;

/********************Uniformes********************/
layout(location = 0) uniform vec4 Color;

/********************Fonctions********************/
void main()
{
	RenderTarget0 = Color;
}