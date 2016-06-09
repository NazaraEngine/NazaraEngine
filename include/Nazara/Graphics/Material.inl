// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline Material::Material()
	{
		Reset();
	}

	inline Material::Material(const Material& material) :
	RefCounted(),
	Resource(material)
	{
		Copy(material);
	}

	inline Material::~Material()
	{
		OnMaterialRelease(this);
	}

	inline void Material::Enable(RendererParameter renderParameter, bool enable)
	{
		NazaraAssert(renderParameter <= RendererParameter_Max, "Renderer parameter out of enum");

		m_states.parameters[renderParameter] = enable;
	}

	inline void Material::EnableAlphaTest(bool alphaTest)
	{
		m_alphaTestEnabled = alphaTest;

		InvalidateShaders();
	}

	inline void Material::EnableDepthSorting(bool depthSorting)
	{
		// Has no influence on shaders
		m_depthSortingEnabled = depthSorting;
	}

	inline void Material::EnableLighting(bool lighting)
	{
		m_lightingEnabled = lighting;

		InvalidateShaders();
	}

	inline void Material::EnableShadowCasting(bool castShadows)
	{
		// Has no influence on shaders
		m_shadowCastingEnabled = castShadows;
	}

	inline void Material::EnableShadowReceive(bool receiveShadows)
	{
		m_shadowReceiveEnabled = receiveShadows;

		InvalidateShaders();
	}

	inline void Material::EnableTransform(bool transform)
	{
		m_transformEnabled = transform;

		InvalidateShaders();
	}

	inline const TextureRef& Material::GetAlphaMap() const
	{
		return m_alphaMap;
	}

	inline float Material::GetAlphaThreshold() const
	{
		return m_alphaThreshold;
	}

	inline Color Material::GetAmbientColor() const
	{
		return m_ambientColor;
	}

	inline RendererComparison Material::GetDepthFunc() const
	{
		return m_states.depthFunc;
	}

	inline const MaterialRef& Material::GetDepthMaterial() const
	{
		return m_depthMaterial;
	}

	inline Color Material::GetDiffuseColor() const
	{
		return m_diffuseColor;
	}

	inline TextureSampler& Material::GetDiffuseSampler()
	{
		return m_diffuseSampler;
	}

	inline const TextureSampler& Material::GetDiffuseSampler() const
	{
		return m_diffuseSampler;
	}

	const TextureRef& Material::GetDiffuseMap() const
	{
		return m_diffuseMap;
	}

	inline BlendFunc Material::GetDstBlend() const
	{
		return m_states.dstBlend;
	}

	inline const TextureRef& Material::GetEmissiveMap() const
	{
		return m_emissiveMap;
	}

	inline FaceSide Material::GetFaceCulling() const
	{
		return m_states.faceCulling;
	}

	inline FaceFilling Material::GetFaceFilling() const
	{
		return m_states.faceFilling;
	}

	inline const TextureRef& Material::GetHeightMap() const
	{
		return m_heightMap;
	}

	inline const TextureRef& Material::GetNormalMap() const
	{
		return m_normalMap;
	}

	inline const RenderStates& Material::GetRenderStates() const
	{
		return m_states;
	}

	inline const UberShader* Material::GetShader() const
	{
		return m_uberShader;
	}

	inline const UberShaderInstance* Material::GetShaderInstance(UInt32 flags) const
	{
		const ShaderInstance& instance = m_shaders[flags];
		if (!instance.uberInstance)
			GenerateShader(flags);

		return instance.uberInstance;
	}

	inline float Material::GetShininess() const
	{
		return m_shininess;
	}

	inline Color Material::GetSpecularColor() const
	{
		return m_specularColor;
	}

	inline const TextureRef& Material::GetSpecularMap() const
	{
		return m_specularMap;
	}

	inline TextureSampler& Material::GetSpecularSampler()
	{
		return m_specularSampler;
	}

	inline const TextureSampler& Material::GetSpecularSampler() const
	{
		return m_specularSampler;
	}

	inline BlendFunc Material::GetSrcBlend() const
	{
		return m_states.srcBlend;
	}

	inline bool Material::HasAlphaMap() const
	{
		return m_alphaMap.IsValid();
	}

	inline bool Material::HasDepthMaterial() const
	{
		return m_depthMaterial.IsValid();
	}

	inline bool Material::HasDiffuseMap() const
	{
		return m_diffuseMap.IsValid();
	}

	inline bool Material::HasEmissiveMap() const
	{
		return m_emissiveMap.IsValid();
	}

	inline bool Material::HasHeightMap() const
	{
		return m_heightMap.IsValid();
	}

	inline bool Material::HasNormalMap() const
	{
		return m_normalMap.IsValid();
	}

	inline bool Material::HasSpecularMap() const
	{
		return m_specularMap.IsValid();
	}

	inline bool Material::IsAlphaTestEnabled() const
	{
		return m_alphaTestEnabled;
	}

	inline bool Material::IsDepthSortingEnabled() const
	{
		return m_depthSortingEnabled;
	}

	inline bool Material::IsEnabled(RendererParameter parameter) const
	{
		NazaraAssert(parameter <= RendererParameter_Max, "Renderer parameter out of enum");

		return m_states.parameters[parameter];
	}

	inline bool Material::IsLightingEnabled() const
	{
		return m_lightingEnabled;
	}

	inline bool Material::IsShadowCastingEnabled() const
	{
		return m_shadowCastingEnabled;
	}

	inline bool Material::IsShadowReceiveEnabled() const
	{
		return m_shadowReceiveEnabled;
	}

	inline bool Material::IsTransformEnabled() const
	{
		return m_transformEnabled;
	}

	inline bool Material::LoadFromFile(const String& filePath, const MaterialParams& params)
	{
		return MaterialLoader::LoadFromFile(this, filePath, params);
	}

	inline bool Material::LoadFromMemory(const void* data, std::size_t size, const MaterialParams& params)
	{
		return MaterialLoader::LoadFromMemory(this, data, size, params);
	}

	inline bool Material::LoadFromStream(Stream& stream, const MaterialParams& params)
	{
		return MaterialLoader::LoadFromStream(this, stream, params);
	}

	inline bool Material::SetAlphaMap(const String& textureName)
	{
		TextureRef texture = TextureLibrary::Query(textureName);
		if (!texture)
		{
			texture = TextureManager::Get(textureName);
			if (!texture)
			{
				NazaraError("Failed to get alpha map \"" + textureName + "\"");
				return false;
			}
		}

		SetAlphaMap(std::move(texture));
		return true;
	}

	inline void Material::SetAlphaMap(TextureRef alphaMap)
	{
		m_alphaMap = std::move(alphaMap);

		InvalidateShaders();
	}

	inline void Material::SetAlphaThreshold(float alphaThreshold)
	{
		m_alphaThreshold = alphaThreshold;
	}

	inline void Material::SetAmbientColor(const Color& ambient)
	{
		m_ambientColor = ambient;
	}

	inline void Material::SetDepthFunc(RendererComparison depthFunc)
	{
		m_states.depthFunc = depthFunc;
	}

	inline void Material::SetDepthMaterial(MaterialRef depthMaterial)
	{
		m_depthMaterial = std::move(depthMaterial);
	}

	inline void Material::SetDiffuseColor(const Color& diffuse)
	{
		m_diffuseColor = diffuse;
	}

	inline bool Material::SetDiffuseMap(const String& textureName)
	{
		TextureRef texture = TextureLibrary::Query(textureName);
		if (!texture)
		{
			texture = TextureManager::Get(textureName);
			if (!texture)
			{
				NazaraError("Failed to get diffuse map \"" + textureName + "\"");
				return false;
			}
		}

		SetDiffuseMap(std::move(texture));
		return true;
	}

	inline void Material::SetDiffuseMap(TextureRef diffuseMap)
	{
		m_diffuseMap = std::move(diffuseMap);

		InvalidateShaders();
	}

	inline void Material::SetDiffuseSampler(const TextureSampler& sampler)
	{
		m_diffuseSampler = sampler;
	}

	inline void Material::SetDstBlend(BlendFunc func)
	{
		m_states.dstBlend = func;
	}

	inline bool Material::SetEmissiveMap(const String& textureName)
	{
		TextureRef texture = TextureLibrary::Query(textureName);
		if (!texture)
		{
			texture = TextureManager::Get(textureName);
			if (!texture)
			{
				NazaraError("Failed to get emissive map \"" + textureName + "\"");
				return false;
			}
		}

		SetEmissiveMap(std::move(texture));
		return true;
	}

	inline void Material::SetEmissiveMap(TextureRef emissiveMap)
	{
		m_emissiveMap = std::move(emissiveMap);

		InvalidateShaders();
	}

	inline void Material::SetFaceCulling(FaceSide faceSide)
	{
		m_states.faceCulling = faceSide;
	}

	inline void Material::SetFaceFilling(FaceFilling filling)
	{
		m_states.faceFilling = filling;
	}

	inline bool Material::SetHeightMap(const String& textureName)
	{
		TextureRef texture = TextureLibrary::Query(textureName);
		if (!texture)
		{
			texture = TextureManager::Get(textureName);
			if (!texture)
			{
				NazaraError("Failed to get height map \"" + textureName + "\"");
				return false;
			}
		}

		SetHeightMap(std::move(texture));
		return true;
	}

	inline void Material::SetHeightMap(TextureRef heightMap)
	{
		m_heightMap = std::move(heightMap);

		InvalidateShaders();
	}

	inline bool Material::SetNormalMap(const String& textureName)
	{
		TextureRef texture = TextureLibrary::Query(textureName);
		if (!texture)
		{
			texture = TextureManager::Get(textureName);
			if (!texture)
			{
				NazaraError("Failed to get normal map \"" + textureName + "\"");
				return false;
			}
		}

		SetNormalMap(std::move(texture));
		return true;
	}

	inline void Material::SetNormalMap(TextureRef normalMap)
	{
		m_normalMap = std::move(normalMap);

		InvalidateShaders();
	}

	inline void Material::SetRenderStates(const RenderStates& states)
	{
		m_states = states;
	}

	inline void Material::SetShader(UberShaderConstRef uberShader)
	{
		m_uberShader = std::move(uberShader);

		InvalidateShaders();
	}

	inline bool Material::SetShader(const String& uberShaderName)
	{
		UberShaderConstRef uberShader = UberShaderLibrary::Get(uberShaderName);
		if (!uberShader)
			return false;

		SetShader(std::move(uberShader));
		return true;
	}

	inline void Material::SetShininess(float shininess)
	{
		m_shininess = shininess;
	}

	inline void Material::SetSpecularColor(const Color& specular)
	{
		m_specularColor = specular;
	}

	inline bool Material::SetSpecularMap(const String& textureName)
	{
		TextureRef texture = TextureLibrary::Query(textureName);
		if (!texture)
		{
			texture = TextureManager::Get(textureName);
			if (!texture)
			{
				NazaraError("Failed to get specular map \"" + textureName + "\"");
				return false;
			}
		}

		SetSpecularMap(std::move(texture));
		return true;
	}

	inline void Material::SetSpecularMap(TextureRef specularMap)
	{
		m_specularMap = std::move(specularMap);

		InvalidateShaders();
	}

	inline void Material::SetSpecularSampler(const TextureSampler& sampler)
	{
		m_specularSampler = sampler;
	}

	inline void Material::SetSrcBlend(BlendFunc func)
	{
		m_states.srcBlend = func;
	}

	inline Material& Material::operator=(const Material& material)
	{
		Resource::operator=(material);

		Copy(material);
		return *this;
	}

	inline MaterialRef Material::GetDefault()
	{
		return s_defaultMaterial;
	}

	inline void Material::InvalidateShaders()
	{
		for (ShaderInstance& instance : m_shaders)
			instance.uberInstance = nullptr;
	}

	template<typename... Args>
	MaterialRef Material::New(Args&&... args)
	{
		std::unique_ptr<Material> object(new Material(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
