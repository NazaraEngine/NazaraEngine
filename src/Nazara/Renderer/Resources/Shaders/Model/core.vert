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
uniform mat4 ViewProjMatrix;
uniform mat4 WorldMatrix;
uniform mat4 WorldViewProjMatrix;

/********************Fonctions********************/
void main()
{
#if FLAG_INSTANCING
	gl_Position = ViewProjMatrix * InstanceData0 * vec4(VertexPosition, 1.0);
#else
	gl_Position = WorldViewProjMatrix * vec4(VertexPosition, 1.0);
#endif

#if LIGHTING
	#if FLAG_INSTANCING
	mat3 rotationMatrix = mat3(InstanceData0);
	#else
	mat3 rotationMatrix = mat3(WorldMatrix);
	#endif
	
	#if NORMAL_MAPPING || PARALLAX_MAPPING
	vec3 binormal = cross(VertexNormal, VertexTangent);
	vLightToWorld[0] = normalize(rotationMatrix * VertexTangent);
	vLightToWorld[1] = normalize(rotationMatrix * binormal);
	vLightToWorld[2] = normalize(rotationMatrix * VertexNormal);
	#else
	vNormal = normalize(rotationMatrix * VertexNormal);
	#endif
#endif

#if ALPHA_MAPPING || DIFFUSE_MAPPING || EMISSIVE_MAPPING || NORMAL_MAPPING || PARALLAX_MAPPING || SPECULAR_MAPPING
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
