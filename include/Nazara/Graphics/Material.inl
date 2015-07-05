// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <memory>
#include <Nazara/Graphics/Debug.hpp>

inline NzMaterial::NzMaterial()
{
	Reset();
}

inline NzMaterial::NzMaterial(const NzMaterial& material) :
	NzRefCounted(),
	NzResource(material)
{
	Copy(material);
}

inline NzMaterial::~NzMaterial()
{
	OnMaterialRelease(this);
}

inline void NzMaterial::Enable(nzRendererParameter renderParameter, bool enable)
{
	NazaraAssert(renderParameter <= nzRendererParameter_Max, "Renderer parameter out of enum");

	m_states.parameters[renderParameter] = enable;
}

inline void NzMaterial::EnableAlphaTest(bool alphaTest)
{
	m_alphaTestEnabled = alphaTest;

	InvalidateShaders();
}

inline void NzMaterial::EnableDepthSorting(bool depthSorting)
{
	// Has no influence on shaders
	m_depthSortingEnabled = depthSorting;
}

inline void NzMaterial::EnableLighting(bool lighting)
{
	m_lightingEnabled = lighting;

	InvalidateShaders();
}

inline void NzMaterial::EnableShadowCasting(bool castShadows)
{
	// Has no influence on shaders
	m_shadowCastingEnabled = castShadows;
}

inline void NzMaterial::EnableShadowReceive(bool receiveShadows)
{
	m_shadowReceiveEnabled = receiveShadows;

	InvalidateShaders();
}

inline void NzMaterial::EnableTransform(bool transform)
{
	m_transformEnabled = transform;

	InvalidateShaders();
}

inline const NzTextureRef& NzMaterial::GetAlphaMap() const
{
	return m_alphaMap;
}

inline float NzMaterial::GetAlphaThreshold() const
{
	return m_alphaThreshold;
}

inline NzColor NzMaterial::GetAmbientColor() const
{
	return m_ambientColor;
}

inline nzRendererComparison NzMaterial::GetDepthFunc() const
{
	return m_states.depthFunc;
}

inline const NzMaterialRef& NzMaterial::GetDepthMaterial() const
{
	return m_depthMaterial;
}

inline NzColor NzMaterial::GetDiffuseColor() const
{
	return m_diffuseColor;
}

inline NzTextureSampler& NzMaterial::GetDiffuseSampler()
{
	return m_diffuseSampler;
}

inline const NzTextureSampler& NzMaterial::GetDiffuseSampler() const
{
	return m_diffuseSampler;
}

const NzTextureRef& NzMaterial::GetDiffuseMap() const
{
	return m_diffuseMap;
}

inline nzBlendFunc NzMaterial::GetDstBlend() const
{
	return m_states.dstBlend;
}

inline const NzTextureRef& NzMaterial::GetEmissiveMap() const
{
	return m_emissiveMap;
}

inline nzFaceSide NzMaterial::GetFaceCulling() const
{
	return m_states.faceCulling;
}

inline nzFaceFilling NzMaterial::GetFaceFilling() const
{
	return m_states.faceFilling;
}

inline const NzTextureRef& NzMaterial::GetHeightMap() const
{
	return m_heightMap;
}

inline const NzTextureRef& NzMaterial::GetNormalMap() const
{
	return m_normalMap;
}

inline const NzRenderStates& NzMaterial::GetRenderStates() const
{
	return m_states;
}

inline const NzUberShader* NzMaterial::GetShader() const
{
	return m_uberShader;
}

inline const NzUberShaderInstance* NzMaterial::GetShaderInstance(nzUInt32 flags) const
{
	const ShaderInstance& instance = m_shaders[flags];
	if (!instance.uberInstance)
		GenerateShader(flags);

	return instance.uberInstance;
}

inline float NzMaterial::GetShininess() const
{
	return m_shininess;
}

inline NzColor NzMaterial::GetSpecularColor() const
{
	return m_specularColor;
}

inline const NzTextureRef& NzMaterial::GetSpecularMap() const
{
	return m_specularMap;
}

inline NzTextureSampler& NzMaterial::GetSpecularSampler()
{
	return m_specularSampler;
}

inline const NzTextureSampler& NzMaterial::GetSpecularSampler() const
{
	return m_specularSampler;
}

inline nzBlendFunc NzMaterial::GetSrcBlend() const
{
	return m_states.srcBlend;
}

inline bool NzMaterial::HasAlphaMap() const
{
	return m_alphaMap.IsValid();
}

inline bool NzMaterial::HasDepthMaterial() const
{
	return m_depthMaterial.IsValid();
}

inline bool NzMaterial::HasDiffuseMap() const
{
	return m_diffuseMap.IsValid();
}

inline bool NzMaterial::HasEmissiveMap() const
{
	return m_emissiveMap.IsValid();
}

inline bool NzMaterial::HasHeightMap() const
{
	return m_heightMap.IsValid();
}

inline bool NzMaterial::HasNormalMap() const
{
	return m_normalMap.IsValid();
}

inline bool NzMaterial::HasSpecularMap() const
{
	return m_specularMap.IsValid();
}

inline bool NzMaterial::IsAlphaTestEnabled() const
{
	return m_alphaTestEnabled;
}

inline bool NzMaterial::IsDepthSortingEnabled() const
{
	return m_depthSortingEnabled;
}

inline bool NzMaterial::IsEnabled(nzRendererParameter parameter) const
{
	NazaraAssert(parameter <= nzRendererParameter_Max, "Renderer parameter out of enum");

	return m_states.parameters[parameter];
}

inline bool NzMaterial::IsLightingEnabled() const
{
	return m_lightingEnabled;
}

inline bool NzMaterial::IsShadowCastingEnabled() const
{
	return m_shadowCastingEnabled;
}

inline bool NzMaterial::IsShadowReceiveEnabled() const
{
	return m_shadowReceiveEnabled;
}

inline bool NzMaterial::IsTransformEnabled() const
{
	return m_transformEnabled;
}

inline bool NzMaterial::LoadFromFile(const NzString& filePath, const NzMaterialParams& params)
{
	return NzMaterialLoader::LoadFromFile(this, filePath, params);
}

inline bool NzMaterial::LoadFromMemory(const void* data, std::size_t size, const NzMaterialParams& params)
{
	return NzMaterialLoader::LoadFromMemory(this, data, size, params);
}

inline bool NzMaterial::LoadFromStream(NzInputStream& stream, const NzMaterialParams& params)
{
	return NzMaterialLoader::LoadFromStream(this, stream, params);
}

inline bool NzMaterial::SetAlphaMap(const NzString& textureName)
{
	NzTextureRef texture = NzTextureLibrary::Query(textureName);
	if (!texture)
	{
		texture = NzTextureManager::Get(textureName);
		if (!texture)
			return false;
	}

	SetAlphaMap(std::move(texture));
	return true;
}

inline void NzMaterial::SetAlphaMap(NzTextureRef alphaMap)
{
	m_alphaMap = std::move(alphaMap);

	InvalidateShaders();
}

inline void NzMaterial::SetAlphaThreshold(float alphaThreshold)
{
	m_alphaThreshold = alphaThreshold;
}

inline void NzMaterial::SetAmbientColor(const NzColor& ambient)
{
	m_ambientColor = ambient;
}

inline void NzMaterial::SetDepthFunc(nzRendererComparison depthFunc)
{
	m_states.depthFunc = depthFunc;
}

inline void NzMaterial::SetDepthMaterial(NzMaterialRef depthMaterial)
{
	m_depthMaterial = std::move(depthMaterial);
}

inline void NzMaterial::SetDiffuseColor(const NzColor& diffuse)
{
	m_diffuseColor = diffuse;
}

inline bool NzMaterial::SetDiffuseMap(const NzString& textureName)
{
	NzTextureRef texture = NzTextureLibrary::Query(textureName);
	if (!texture)
	{
		texture = NzTextureManager::Get(textureName);
		if (!texture)
			return false;
	}

	SetDiffuseMap(std::move(texture));
	return true;
}

inline void NzMaterial::SetDiffuseMap(NzTextureRef diffuseMap)
{
	m_diffuseMap = std::move(diffuseMap);

	InvalidateShaders();
}

inline void NzMaterial::SetDiffuseSampler(const NzTextureSampler& sampler)
{
	m_diffuseSampler = sampler;
}

inline void NzMaterial::SetDstBlend(nzBlendFunc func)
{
	m_states.dstBlend = func;
}

inline bool NzMaterial::SetEmissiveMap(const NzString& textureName)
{
	NzTextureRef texture = NzTextureLibrary::Query(textureName);
	if (!texture)
	{
		texture = NzTextureManager::Get(textureName);
		if (!texture)
			return false;
	}

	SetEmissiveMap(std::move(texture));
	return true;
}

inline void NzMaterial::SetEmissiveMap(NzTextureRef emissiveMap)
{
	m_emissiveMap = std::move(emissiveMap);

	InvalidateShaders();
}

inline void NzMaterial::SetFaceCulling(nzFaceSide faceSide)
{
	m_states.faceCulling = faceSide;
}

inline void NzMaterial::SetFaceFilling(nzFaceFilling filling)
{
	m_states.faceFilling = filling;
}

inline bool NzMaterial::SetHeightMap(const NzString& textureName)
{
	NzTextureRef texture = NzTextureLibrary::Query(textureName);
	if (!texture)
	{
		texture = NzTextureManager::Get(textureName);
		if (!texture)
			return false;
	}

	SetHeightMap(std::move(texture));
	return true;
}

inline void NzMaterial::SetHeightMap(NzTextureRef heightMap)
{
	m_heightMap = std::move(heightMap);

	InvalidateShaders();
}

inline bool NzMaterial::SetNormalMap(const NzString& textureName)
{
	NzTextureRef texture = NzTextureLibrary::Query(textureName);
	if (!texture)
	{
		texture = NzTextureManager::Get(textureName);
		if (!texture)
			return false;
	}

	SetNormalMap(std::move(texture));
	return true;
}

inline void NzMaterial::SetNormalMap(NzTextureRef normalMap)
{
	m_normalMap = std::move(normalMap);

	InvalidateShaders();
}

inline void NzMaterial::SetRenderStates(const NzRenderStates& states)
{
	m_states = states;
}

inline void NzMaterial::SetShader(NzUberShaderConstRef uberShader)
{
	m_uberShader = std::move(uberShader);

	InvalidateShaders();
}

inline bool NzMaterial::SetShader(const NzString& uberShaderName)
{
	NzUberShaderConstRef uberShader = NzUberShaderLibrary::Get(uberShaderName);
	if (!uberShader)
		return false;

	SetShader(std::move(uberShader));
	return true;
}

inline void NzMaterial::SetShininess(float shininess)
{
	m_shininess = shininess;
}

inline void NzMaterial::SetSpecularColor(const NzColor& specular)
{
	m_specularColor = specular;
}

inline bool NzMaterial::SetSpecularMap(const NzString& textureName)
{
	NzTextureRef texture = NzTextureLibrary::Query(textureName);
	if (!texture)
	{
		texture = NzTextureManager::Get(textureName);
		if (!texture)
			return false;
	}

	SetSpecularMap(std::move(texture));
	return true;
}

inline void NzMaterial::SetSpecularMap(NzTextureRef specularMap)
{
	m_specularMap = std::move(specularMap);

	InvalidateShaders();
}

inline void NzMaterial::SetSpecularSampler(const NzTextureSampler& sampler)
{
	m_specularSampler = sampler;
}

inline void NzMaterial::SetSrcBlend(nzBlendFunc func)
{
	m_states.srcBlend = func;
}

inline NzMaterial& NzMaterial::operator=(const NzMaterial& material)
{
	NzResource::operator=(material);

	Copy(material);
	return *this;
}

inline NzMaterialRef NzMaterial::GetDefault()
{
	return s_defaultMaterial;
}

inline void NzMaterial::InvalidateShaders()
{
	for (ShaderInstance& instance : m_shaders)
		instance.uberInstance = nullptr;
}

template<typename... Args>
NzMaterialRef NzMaterial::New(Args&&... args)
{
	std::unique_ptr<NzMaterial> object(new NzMaterial(std::forward<Args>(args)...));
	object->SetPersistent(false);

	return object.release();
}

#include <Nazara/Graphics/DebugOff.hpp>
