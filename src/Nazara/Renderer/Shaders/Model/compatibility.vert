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

#if ALPHA_MAPPING || DIFFUSE_MAPPING || EMISSIVE_MAPPING || NORMAL_MAPPING || PARALLAX_MAPPING || SPECULAR_MAPPING
	#if FLAG_FLIP_UVS
	vTexCoord = vec2(VertexTexCoord.x, 1.0 - VertexTexCoord.y);
	#else
	vTexCoord = VertexTexCoord;
	#endif
#endif

#if LIGHTING
	#if FLAG_INSTANCING
	vWorldPos = vec3(InstanceData0 * vec4(VertexPosition, 1.0));
	#else
	vWorldPos = vec3(WorldMatrix * vec4(VertexPosition, 1.0));
	#endif
#endif
}
