/********************Entrant********************/
#if FLAG_BILLBOARD
varying vec3 InstanceData0; // center
varying vec4 InstanceData1; // size | sin cos
varying vec4 InstanceData2; // color
#else
varying mat4 InstanceData0;
#endif

varying vec3 VertexPosition;
varying vec3 VertexNormal;
varying vec3 VertexTangent;
varying vec2 VertexTexCoord;

/********************Sortant********************/
varying mat3 vLightToWorld;
varying vec3 vNormal;
varying vec2 vTexCoord;
varying vec3 vWorldPos;
varying vec4 vColor;

/********************Uniformes********************/
uniform float VertexDepth;
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
	texCoords = VertexPosition.xy + vec2(0.5, 0.5);
	#else
	vec2 billboardCorner = VertexTexCoord - vec2(0.5, 0.5);
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

#if LIGHTING
	#if FLAG_INSTANCING
	mat3 rotationMatrix = mat3(InstanceData0[0].xyz, InstanceData0[1].xyz, InstanceData0[2].xyz);
	#else
	mat3 rotationMatrix = mat3(WorldMatrix[0].xyz, WorldMatrix[1].xyz, WorldMatrix[2].xyz);
	#endif
	
	#if NORMAL_MAPPING
	vec3 binormal = cross(VertexNormal, VertexTangent);
	vLightToWorld[0] = normalize(rotationMatrix * VertexTangent);
	vLightToWorld[1] = normalize(rotationMatrix * binormal);
	vLightToWorld[2] = normalize(rotationMatrix * VertexNormal);
	#else
	vNormal = normalize(rotationMatrix * VertexNormal);
	#endif
#endif

#if TEXTURE_MAPPING
	vTexCoord = vec2(texCoords);
#endif

#if LIGHTING && PARALLAX_MAPPING
	vViewDir = EyePosition - VertexPosition; 
	vViewDir *= vLightToWorld;
#endif

#if LIGHTING && !FLAG_DEFERRED
	#if FLAG_INSTANCING
	vWorldPos = vec3(InstanceData0 * vec4(VertexPosition, 1.0));
	#else
	vWorldPos = vec3(WorldMatrix * vec4(VertexPosition, 1.0));
	#endif
#endif
}
