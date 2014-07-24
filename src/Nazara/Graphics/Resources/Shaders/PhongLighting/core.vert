/********************Entrant********************/
in mat4 InstanceData0;
in vec3 VertexPosition;
in vec3 VertexNormal;
in vec3 VertexTangent;
in vec2 VertexTexCoord;

/********************Sortant********************/
out mat3 vLightToWorld;
out vec3 vNormal;
out vec2 vTexCoord;
out vec3 vViewDir;
out vec3 vWorldPos;

/********************Uniformes********************/
uniform vec3 EyePosition;
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
#endif

#if TEXTURE_MAPPING
	#if FLAG_FLIP_UVS
	vTexCoord = vec2(VertexTexCoord.x, 1.0 - VertexTexCoord.y);
	#else
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
