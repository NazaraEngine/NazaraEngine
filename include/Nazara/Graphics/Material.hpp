// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MATERIAL_HPP
#define NAZARA_MATERIAL_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/ObjectListenerWrapper.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceLoader.hpp>
#include <Nazara/Core/ResourceManager.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Renderer/RenderStates.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Renderer/TextureSampler.hpp>
#include <Nazara/Renderer/UberShader.hpp>

struct NAZARA_API NzMaterialParams
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

using NzMaterialConstListener = NzObjectListenerWrapper<const NzMaterial>;
using NzMaterialConstRef = NzObjectRef<const NzMaterial>;
using NzMaterialLibrary = NzObjectLibrary<NzMaterial>;
using NzMaterialListener = NzObjectListenerWrapper<NzMaterial>;
using NzMaterialLoader = NzResourceLoader<NzMaterial, NzMaterialParams>;
using NzMaterialManager = NzResourceManager<NzMaterial, NzMaterialParams>;
using NzMaterialRef = NzObjectRef<NzMaterial>;

class NAZARA_API NzMaterial : public NzRefCounted, public NzResource
{
	friend NzMaterialLibrary;
	friend NzMaterialLoader;
	friend NzMaterialManager;
	friend class NzGraphics;

	public:
		NzMaterial();
		NzMaterial(const NzMaterial& material);
		~NzMaterial();

		const NzShader* Apply(nzUInt32 shaderFlags = 0, nzUInt8 textureUnit = 0, nzUInt8* lastUsedUnit = nullptr) const;

		void Enable(nzRendererParameter renderParameter, bool enable);
		void EnableAlphaTest(bool alphaTest);
		void EnableDepthSorting(bool depthSorting);
		void EnableLighting(bool lighting);
		void EnableTransform(bool transform);

		NzTexture* GetAlphaMap() const;
		float GetAlphaThreshold() const;
		NzColor GetAmbientColor() const;
		nzRendererComparison GetDepthFunc() const;
		NzColor GetDiffuseColor() const;
		NzTexture* GetDiffuseMap() const;
		NzTextureSampler& GetDiffuseSampler();
		const NzTextureSampler& GetDiffuseSampler() const;
		nzBlendFunc GetDstBlend() const;
		NzTexture* GetEmissiveMap() const;
		nzFaceSide GetFaceCulling() const;
		nzFaceFilling GetFaceFilling() const;
		NzTexture* GetHeightMap() const;
		NzTexture* GetNormalMap() const;
		const NzRenderStates& GetRenderStates() const;
		const NzUberShader* GetShader() const;
		const NzUberShaderInstance* GetShaderInstance(nzUInt32 flags = nzShaderFlags_None) const;
		float GetShininess() const;
		NzColor GetSpecularColor() const;
		NzTexture* GetSpecularMap() const;
		NzTextureSampler& GetSpecularSampler();
		const NzTextureSampler& GetSpecularSampler() const;
		nzBlendFunc GetSrcBlend() const;

		bool HasAlphaMap() const;
		bool HasDiffuseMap() const;
		bool HasEmissiveMap() const;
		bool HasHeightMap() const;
		bool HasNormalMap() const;
		bool HasSpecularMap() const;

		bool IsAlphaTestEnabled() const;
		bool IsDepthSortingEnabled() const;
		bool IsEnabled(nzRendererParameter renderParameter) const;
		bool IsLightingEnabled() const;
		bool IsTransformEnabled() const;

		bool LoadFromFile(const NzString& filePath, const NzMaterialParams& params = NzMaterialParams());
		bool LoadFromMemory(const void* data, std::size_t size, const NzMaterialParams& params = NzMaterialParams());
		bool LoadFromStream(NzInputStream& stream, const NzMaterialParams& params = NzMaterialParams());

		void Reset();

		bool SetAlphaMap(const NzString& textureName);
		void SetAlphaMap(NzTextureRef alphaMap);
		void SetAlphaThreshold(float alphaThreshold);
		void SetAmbientColor(const NzColor& ambient);
		void SetDepthFunc(nzRendererComparison depthFunc);
		void SetDiffuseColor(const NzColor& diffuse);
		bool SetDiffuseMap(const NzString& textureName);
		void SetDiffuseMap(NzTextureRef diffuseMap);
		void SetDiffuseSampler(const NzTextureSampler& sampler);
		void SetDstBlend(nzBlendFunc func);
		bool SetEmissiveMap(const NzString& textureName);
		void SetEmissiveMap(NzTextureRef textureName);
		void SetFaceCulling(nzFaceSide faceSide);
		void SetFaceFilling(nzFaceFilling filling);
		bool SetHeightMap(const NzString& textureName);
		void SetHeightMap(NzTextureRef textureName);
		bool SetNormalMap(const NzString& textureName);
		void SetNormalMap(NzTextureRef textureName);
		void SetRenderStates(const NzRenderStates& states);
		void SetShader(NzUberShaderConstRef uberShader);
		bool SetShader(const NzString& uberShaderName);
		void SetShininess(float shininess);
		void SetSpecularColor(const NzColor& specular);
		bool SetSpecularMap(const NzString& textureName);
		void SetSpecularMap(NzTextureRef specularMap);
		void SetSpecularSampler(const NzTextureSampler& sampler);
		void SetSrcBlend(nzBlendFunc func);

		NzMaterial& operator=(const NzMaterial& material);

		static NzMaterialRef GetDefault();
		template<typename... Args> static NzMaterialRef New(Args&&... args);

	private:
		struct ShaderInstance
		{
			const NzShader* shader;
			NzUberShaderInstance* uberInstance = nullptr;
			int uniforms[nzMaterialUniform_Max+1];
		};

		void Copy(const NzMaterial& material);
		void GenerateShader(nzUInt32 flags) const;
		void InvalidateShaders();

		static bool Initialize();
		static void Uninitialize();

		NzColor m_ambientColor;
		NzColor m_diffuseColor;
		NzColor m_specularColor;
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
