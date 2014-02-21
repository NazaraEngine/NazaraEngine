/********************Entrant********************/
varying mat4 InstanceData0;
varying vec3 VertexPosition;
varying vec3 VertexNormal;
varying vec3 VertexTangent;
varying vec2 VertexTexCoord;

/********************Sortant********************/
varying mat3 vLightToWorld;
varying vec3 vNormal;
varying vec2 vTexCoord;
varying vec3 vWorldPos;

/********************Uniformes********************/
uniform float VertexDepth;
uniform mat4 ViewProjMatrix;
uniform mat4 WorldMatrix;
uniform mat4 WorldViewProjMatrix;

/********************Fonctions********************/
void main()
{
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
/*	#if FLAG_FLIP_UVS
	vTexCoord = vec2(VertexTexCoord.x, 1.0 - VertexTexCoord.y);
	#else*/
	vTexCoord = VertexTexCoord;
	#endif
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
