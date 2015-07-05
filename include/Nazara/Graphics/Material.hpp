// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MATERIAL_HPP
#define NAZARA_MATERIAL_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceLoader.hpp>
#include <Nazara/Core/ResourceManager.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Renderer/RenderStates.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Renderer/TextureSampler.hpp>
#include <Nazara/Renderer/UberShader.hpp>

struct NAZARA_GRAPHICS_API NzMaterialParams
{
	bool loadAlphaMap = true;
	bool loadDiffuseMap = true;
	bool loadEmissiveMap = true;
	bool loadHeightMap = true;
	bool loadNormalMap = true;
	bool loadSpecularMap = true;
	NzString shaderName = "Basic";

	bool IsValid() const;
};

class NzMaterial;

using NzMaterialConstRef = NzObjectRef<const NzMaterial>;
using NzMaterialLibrary = NzObjectLibrary<NzMaterial>;
using NzMaterialLoader = NzResourceLoader<NzMaterial, NzMaterialParams>;
using NzMaterialManager = NzResourceManager<NzMaterial, NzMaterialParams>;
using NzMaterialRef = NzObjectRef<NzMaterial>;

class NAZARA_GRAPHICS_API NzMaterial : public NzRefCounted, public NzResource
{
	friend NzMaterialLibrary;
	friend NzMaterialLoader;
	friend NzMaterialManager;
	friend class NzGraphics;

	public:
		inline NzMaterial();
		inline NzMaterial(const NzMaterial& material);
		inline ~NzMaterial();

		const NzShader* Apply(nzUInt32 shaderFlags = 0, nzUInt8 textureUnit = 0, nzUInt8* lastUsedUnit = nullptr) const;

		inline void Enable(nzRendererParameter renderParameter, bool enable);
		inline void EnableAlphaTest(bool alphaTest);
		inline void EnableDepthSorting(bool depthSorting);
		inline void EnableLighting(bool lighting);
		inline void EnableShadowCasting(bool castShadows);
		inline void EnableShadowReceive(bool receiveShadows);
		inline void EnableTransform(bool transform);

		inline const NzTextureRef& GetAlphaMap() const;
		inline float GetAlphaThreshold() const;
		inline NzColor GetAmbientColor() const;
		inline nzRendererComparison GetDepthFunc() const;
		inline const NzMaterialRef& GetDepthMaterial() const;
		inline NzColor GetDiffuseColor() const;
		inline const NzTextureRef& GetDiffuseMap() const;
		inline NzTextureSampler& GetDiffuseSampler();
		inline const NzTextureSampler& GetDiffuseSampler() const;
		inline nzBlendFunc GetDstBlend() const;
		inline const NzTextureRef& GetEmissiveMap() const;
		inline nzFaceSide GetFaceCulling() const;
		inline nzFaceFilling GetFaceFilling() const;
		inline const NzTextureRef& GetHeightMap() const;
		inline const NzTextureRef& GetNormalMap() const;
		inline const NzRenderStates& GetRenderStates() const;
		inline const NzUberShader* GetShader() const;
		inline const NzUberShaderInstance* GetShaderInstance(nzUInt32 flags = nzShaderFlags_None) const;
		inline float GetShininess() const;
		inline NzColor GetSpecularColor() const;
		inline const NzTextureRef& GetSpecularMap() const;
		inline NzTextureSampler& GetSpecularSampler();
		inline const NzTextureSampler& GetSpecularSampler() const;
		inline nzBlendFunc GetSrcBlend() const;

		inline bool HasAlphaMap() const;
		inline bool HasDepthMaterial() const;
		inline bool HasDiffuseMap() const;
		inline bool HasEmissiveMap() const;
		inline bool HasHeightMap() const;
		inline bool HasNormalMap() const;
		inline bool HasSpecularMap() const;

		inline bool IsAlphaTestEnabled() const;
		inline bool IsDepthSortingEnabled() const;
		inline bool IsEnabled(nzRendererParameter renderParameter) const;
		inline bool IsLightingEnabled() const;
		inline bool IsShadowCastingEnabled() const;
		inline bool IsShadowReceiveEnabled() const;
		inline bool IsTransformEnabled() const;

		inline bool LoadFromFile(const NzString& filePath, const NzMaterialParams& params = NzMaterialParams());
		inline bool LoadFromMemory(const void* data, std::size_t size, const NzMaterialParams& params = NzMaterialParams());
		inline bool LoadFromStream(NzInputStream& stream, const NzMaterialParams& params = NzMaterialParams());

		void Reset();

		inline bool SetAlphaMap(const NzString& textureName);
		inline void SetAlphaMap(NzTextureRef alphaMap);
		inline void SetAlphaThreshold(float alphaThreshold);
		inline void SetAmbientColor(const NzColor& ambient);
		inline void SetDepthFunc(nzRendererComparison depthFunc);
		inline void SetDepthMaterial(NzMaterialRef depthMaterial);
		inline void SetDiffuseColor(const NzColor& diffuse);
		inline bool SetDiffuseMap(const NzString& textureName);
		inline void SetDiffuseMap(NzTextureRef diffuseMap);
		inline void SetDiffuseSampler(const NzTextureSampler& sampler);
		inline void SetDstBlend(nzBlendFunc func);
		inline bool SetEmissiveMap(const NzString& textureName);
		inline void SetEmissiveMap(NzTextureRef textureName);
		inline void SetFaceCulling(nzFaceSide faceSide);
		inline void SetFaceFilling(nzFaceFilling filling);
		inline bool SetHeightMap(const NzString& textureName);
		inline void SetHeightMap(NzTextureRef textureName);
		inline bool SetNormalMap(const NzString& textureName);
		inline void SetNormalMap(NzTextureRef textureName);
		inline void SetRenderStates(const NzRenderStates& states);
		inline void SetShader(NzUberShaderConstRef uberShader);
		inline bool SetShader(const NzString& uberShaderName);
		inline void SetShininess(float shininess);
		inline void SetSpecularColor(const NzColor& specular);
		inline bool SetSpecularMap(const NzString& textureName);
		inline void SetSpecularMap(NzTextureRef specularMap);
		inline void SetSpecularSampler(const NzTextureSampler& sampler);
		inline void SetSrcBlend(nzBlendFunc func);

		inline NzMaterial& operator=(const NzMaterial& material);

		static NzMaterialRef GetDefault();
		template<typename... Args> static NzMaterialRef New(Args&&... args);

		// Signals
		NazaraSignal(OnMaterialRelease, const NzMaterial*); //< Args: me
		NazaraSignal(OnMaterialReset, const NzMaterial*); //< Args: me

	private:
		struct ShaderInstance
		{
			const NzShader* shader;
			NzUberShaderInstance* uberInstance = nullptr;
			int uniforms[nzMaterialUniform_Max+1];
		};

		void Copy(const NzMaterial& material);
		void GenerateShader(nzUInt32 flags) const;
		inline void InvalidateShaders();

		static bool Initialize();
		static void Uninitialize();

		NzColor m_ambientColor;
		NzColor m_diffuseColor;
		NzColor m_specularColor;
		NzMaterialRef m_depthMaterial; //< Materialception
		NzRenderStates m_states;
		NzTextureSampler m_diffuseSampler;
		NzTextureSampler m_specularSampler;
		NzTextureRef m_alphaMap;
		NzTextureRef m_diffuseMap;
		NzTextureRef m_emissiveMap;
		NzTextureRef m_heightMap;
		NzTextureRef m_normalMap;
		NzTextureRef m_specularMap;
		NzUberShaderConstRef m_uberShader;
		mutable ShaderInstance m_shaders[nzShaderFlags_Max+1];
		bool m_alphaTestEnabled;
		bool m_depthSortingEnabled;
		bool m_lightingEnabled;
		bool m_shadowCastingEnabled;
		bool m_shadowReceiveEnabled;
		bool m_transformEnabled;
		float m_alphaThreshold;
		float m_shininess;

		static NzMaterialLibrary::LibraryMap s_library;
		static NzMaterialLoader::LoaderList s_loaders;
		static NzMaterialManager::ManagerMap s_managerMap;
		static NzMaterialManager::ManagerParams s_managerParameters;
		static NzMaterialRef s_defaultMaterial;
};

#include <Nazara/Graphics/Material.inl>

#endif // NAZARA_MATERIAL_HPP
