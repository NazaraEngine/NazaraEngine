/********************Entrant********************/
#if FLAG_BILLBOARD
in vec3 InstanceData0; // center
in vec4 InstanceData1; // size | sin cos
in vec4 InstanceData2; // color
#else
in mat4 InstanceData0;
#endif

in vec4 VertexColor;
in vec3 VertexPosition;
in vec3 VertexNormal;
in vec3 VertexTangent;
in vec2 VertexTexCoord;
in vec4 VertexUserdata0;

/********************Sortant********************/
out vec4 vColor;
out vec4 vLightSpacePos[3];
out mat3 vLightToWorld;
out vec3 vNormal;
out vec2 vTexCoord;
out vec3 vViewDir;
out vec3 vWorldPos;

/********************Uniformes********************/
uniform vec3 EyePosition;
uniform mat4 InvViewMatrix;
uniform mat4 LightViewProjMatrix[3];
uniform float VertexDepth;
uniform mat4 ViewMatrix;
uniform mat4 ViewProjMatrix;
uniform mat4 WorldMatrix;
uniform mat4 WorldViewProjMatrix;

/********************Fonctions********************/
void main()
{
#if FLAG_VERTEXCOLOR
	vec4 color = VertexColor;
#else
	vec4 color = vec4(1.0);
#endif

	vec2 texCoords;

#if FLAG_BILLBOARD
	#if FLAG_INSTANCING
	vec3 billboardCenter = InstanceData0;
	vec2 billboardSize = InstanceData1.xy;
	vec2 billboardSinCos = InstanceData1.zw;
	vec4 billboardColor = InstanceData2;

	vec2 rotatedPosition;
	rotatedPosition.x = VertexPosition.x*billboardSinCos.y - VertexPosition.y*billboardSinCos.x;
	rotatedPosition.y = VertexPosition.y*billboardSinCos.y + VertexPosition.x*billboardSinCos.x;
	rotatedPosition *= billboardSize;

	vec3 cameraRight = vec3(ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
	vec3 cameraUp = vec3(ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
	vec3 vertexPos = billboardCenter + cameraRight*rotatedPosition.x + cameraUp*rotatedPosition.y;

	gl_Position = ViewProjMatrix * vec4(vertexPos, 1.0);
	color = billboardColor;
	texCoords = VertexPosition.xy + 0.5;
	#else
	vec2 billboardCorner = VertexTexCoord - 0.5;
	vec2 billboardSize = VertexUserdata0.xy;
	vec2 billboardSinCos = VertexUserdata0.zw;
	
	vec2 rotatedPosition;
	rotatedPosition.x = billboardCorner.x*billboardSinCos.y - billboardCorner.y*billboardSinCos.x;
	rotatedPosition.y = billboardCorner.y*billboardSinCos.y + billboardCorner.x*billboardSinCos.x;
	rotatedPosition *= billboardSize;

	vec3 cameraRight = vec3(ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
	vec3 cameraUp = vec3(ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
	vec3 vertexPos = VertexPosition + cameraRight*rotatedPosition.x + cameraUp*rotatedPosition.y;

	gl_Position = ViewProjMatrix * vec4(vertexPos, 1.0);
	texCoords = VertexTexCoord;
	#endif
	texCoords.y = 1.0 - texCoords.y;
#else
	#if FLAG_INSTANCING
		#if TRANSFORM
	gl_Position = ViewProjMatrix * InstanceData0 * vec4(VertexPosition, 1.0);
		#else
			#if UNIFORM_VERTEX_DEPTH
	gl_Position = InstanceData0 * vec4(VertexPosition.xy, VertexDepth, 1.0);
			#else
	gl_Position = InstanceData0 * vec4(VertexPosition, 1.0);
			#endif
		#endif
	#else
		#if TRANSFORM
	gl_Position = WorldViewProjMatrix * vec4(VertexPosition, 1.0);
		#else
			#if UNIFORM_VERTEX_DEPTH
	gl_Position = vec4(VertexPosition.xy, VertexDepth, 1.0);
			#else
	gl_Position = vec4(VertexPosition, 1.0);
			#endif
		#endif
	#endif

	texCoords = VertexTexCoord;
#endif

	vColor = color;

#if FLAG_INSTANCING
	mat3 rotationMatrix = mat3(InstanceData0);
#else
	mat3 rotationMatrix = mat3(WorldMatrix);
#endif
	
#if COMPUTE_TBNMATRIX
	vec3 binormal = cross(VertexNormal, VertexTangent);
	vLightToWorld[0] = normalize(rotationMatrix * VertexTangent);
	vLightToWorld[1] = normalize(rotationMatrix * binormal);
	vLightToWorld[2] = normalize(rotationMatrix * VertexNormal);
#else
	vNormal = normalize(rotationMatrix * VertexNormal);
#endif

#if SHADOW_MAPPING
	for (int i = 0; i < 3; ++i)
		vLightSpacePos[i] = LightViewProjMatrix[i] * WorldMatrix * vec4(VertexPosition, 1.0);
#endif

#if TEXTURE_MAPPING
	vTexCoord = VertexTexCoord;
#endif

#if PARALLAX_MAPPING
	vViewDir = EyePosition - VertexPosition; 
	vViewDir *= vLightToWorld;
#endif

#if !FLAG_DEFERRED
	#if FLAG_INSTANCING
	vWorldPos = vec3(InstanceData0 * vec4(VertexPosition, 1.0));
	#else
	vWorldPos = vec3(WorldMatrix * vec4(VertexPosition, 1.0));
	#endif
#endif
}
