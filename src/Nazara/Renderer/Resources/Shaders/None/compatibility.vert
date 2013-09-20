/********************Entrant********************/
varying mat4 InstanceData0;
varying vec3 VertexPosition;

/********************Uniformes********************/
uniform mat4 ViewProjMatrix;
uniform mat4 WorldViewProjMatrix;

/********************Fonctions********************/
void main()
{
#if FLAG_INSTANCING
	gl_Position = ViewProjMatrix * InstanceData0 * vec4(VertexPosition, 1.0);
#else
	gl_Position = WorldViewProjMatrix * vec4(VertexPosition, 1.0);
#endif
}
