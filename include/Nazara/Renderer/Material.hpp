// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MATERIAL_HPP
#define NAZARA_MATERIAL_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceLoader.hpp>
#include <Nazara/Core/ResourceRef.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Renderer/RenderStates.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Renderer/TextureSampler.hpp>

struct NAZARA_API NzMaterialParams
{
	bool loadDiffuseMap = true;
	bool loadSpecularMap = true;

	bool IsValid() const;
};

class NzMaterial;

using NzMaterialConstRef = NzResourceRef<const NzMaterial>;
using NzMaterialLoader = NzResourceLoader<NzMaterial, NzMaterialParams>;
using NzMaterialRef = NzResourceRef<NzMaterial>;

class NAZARA_API NzMaterial : public NzResource
{
	friend NzMaterialLoader;

	public:
		NzMaterial();
		NzMaterial(const NzMaterial& material);
		NzMaterial(NzMaterial&& material);
		~NzMaterial() = default;

		void Apply(const NzShader* shader) const;

		void EnableAlphaBlending(bool blending);
		void EnableFaceCulling(bool faceCulling);
		void EnableLighting(bool lighting);
		void EnableZBuffer(bool zBuffer);
		void EnableZWrite(bool zWrite);

		NzColor GetAmbientColor() const;
		NzTexture* GetAlphaMap() const;
		const NzShader* GetCustomShader() const;
		NzColor GetDiffuseColor() const;
		NzTexture* GetDiffuseMap() const;
		NzTextureSampler& GetDiffuseSampler();
		const NzTextureSampler& GetDiffuseSampler() const;
		nzBlendFunc GetDstBlend() const;
		NzTexture* GetEmissiveMap() const;
		nzFaceCulling GetFaceCulling() const;
		nzFaceFilling GetFaceFilling() const;
		NzTexture* GetHeightMap() const;
		NzTexture* GetNormalMap() const;
		nzUInt32 GetShaderFlags() const;
		float GetShininess() const;
		NzColor GetSpecularColor() const;
		NzTexture* GetSpecularMap() const;
		NzTextureSampler& GetSpecularSampler();
		const NzTextureSampler& GetSpecularSampler() const;
		nzBlendFunc GetSrcBlend() const;
		nzRendererComparison GetZTestCompare() const;

		bool HasCustomShader() const;

		bool IsAlphaBlendingEnabled() const;
		bool IsFaceCullingEnabled() const;
		bool IsLightingEnabled() const;
		bool IsZBufferEnabled() const;
		bool IsZWriteEnabled() const;

		bool LoadFromFile(const NzString& filePath, const NzMaterialParams& params = NzMaterialParams());
		bool LoadFromMemory(const void* data, std::size_t size, const NzMaterialParams& params = NzMaterialParams());
		bool LoadFromStream(NzInputStream& stream, const NzMaterialParams& params = NzMaterialParams());

		void Reset();

		bool SetAlphaMap(const NzString& texturePath);
		void SetAlphaMap(NzTexture* map);
		void SetAmbientColor(const NzColor& ambient);
		void SetDiffuseColor(const NzColor& diffuse);
		bool SetDiffuseMap(const NzString& texturePath);
		void SetDiffuseMap(NzTexture* map);
		void SetDiffuseSampler(const NzTextureSampler& sampler);
		void SetDstBlend(nzBlendFunc func);
		bool SetEmissiveMap(const NzString& texturePath);
		void SetEmissiveMap(NzTexture* map);
		void SetFaceCulling(nzFaceCulling culling);
		void SetFaceFilling(nzFaceFilling filling);
		bool SetHeightMap(const NzString& texturePath);
		void SetHeightMap(NzTexture* map);
		bool SetNormalMap(const NzString& texturePath);
		void SetNormalMap(NzTexture* map);
		void SetCustomShader(const NzShader* shader);
		void SetShininess(float shininess);
		void SetSpecularColor(const NzColor& specular);
		bool SetSpecularMap(const NzString& texturePath);
		void SetSpecularMap(NzTexture* map);
		void SetSpecularSampler(const NzTextureSampler& sampler);
		void SetSrcBlend(nzBlendFunc func);
		void SetZTestCompare(nzRendererComparison compareFunc);

		NzMaterial& operator=(const NzMaterial& material);
		NzMaterial& operator=(NzMaterial&& material);

		static NzMaterial* GetDefault();

	private:
		void Copy(const NzMaterial& material);

		nzUInt32 m_shaderFlags;
		NzColor m_ambientColor;
		NzColor m_diffuseColor;
		NzColor m_specularColor;
		NzRenderStates m_states;
		NzTextureSampler m_diffuseSampler;
		NzTextureSampler m_specularSampler;
		mutable NzShaderConstRef m_customShader;
		NzTextureRef m_alphaMap;
		NzTextureRef m_diffuseMap;
		NzTextureRef m_emissiveMap;
		NzTextureRef m_heightMap;
		NzTextureRef m_normalMap;
		NzTextureRef m_specularMap;
		bool m_lightingEnabled;
		float m_shininess;

		static NzMaterialLoader::LoaderList s_loaders;
};

#endif // NAZARA_MATERIAL_HPP
