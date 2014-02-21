#version 140

out vec4 RenderTarget0;
out vec4 RenderTarget1;
out vec4 RenderTarget2;

void main()
{
	RenderTarget0 = vec4(0.0, 0.0, 0.0, 0.0);
	RenderTarget1 = vec4(0.0, 0.0, 0.0, 0.0);
	RenderTarget2 = vec4(1.0, 0.0, 0.0, 0.0);
	gl_FragDepth = 1.0;
}
