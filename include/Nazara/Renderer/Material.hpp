// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MATERIAL_HPP
#define NAZARA_MATERIAL_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/ResourceLoader.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Renderer/TextureSampler.hpp>

struct NAZARA_API NzMaterialParams
{
	bool loadDiffuseMap = true;
	bool loadSpecularMap = true;

	bool IsValid() const;
};

class NzMaterial;
class NzShader;

using NzMaterialLoader = NzResourceLoader<NzMaterial, NzMaterialParams>;

class NAZARA_API NzMaterial : public NzResource
{
	friend NzMaterialLoader;

	public:
		NzMaterial();
		NzMaterial(const NzMaterial& material);
		NzMaterial(NzMaterial&& material);
		~NzMaterial();

		void Apply() const;

		void EnableAlphaBlending(bool alphaBlending);
		void EnableFaceCulling(bool faceCulling);
		void EnableLighting(bool lighting);
		void EnableZTest(bool zTest);
		void EnableZWrite(bool zWrite);

		NzColor GetAmbientColor() const;
		NzColor GetDiffuseColor() const;
		NzTexture* GetDiffuseMap() const;
		NzTextureSampler& GetDiffuseSampler();
		const NzTextureSampler& GetDiffuseSampler() const;
		nzBlendFunc GetDstBlend() const;
		nzFaceCulling GetFaceCulling() const;
		nzFaceFilling GetFaceFilling() const;
		NzTexture* GetHeightMap() const;
		NzTexture* GetNormalMap() const;
		const NzShader* GetShader() const;
		float GetShininess() const;
		NzColor GetSpecularColor() const;
		NzTexture* GetSpecularMap() const;
		NzTextureSampler& GetSpecularSampler();
		const NzTextureSampler& GetSpecularSampler() const;
		nzBlendFunc GetSrcBlend() const;
		nzRendererComparison GetZTestCompare() const;

		bool IsAlphaBlendingEnabled() const;
		bool IsFaceCullingEnabled() const;
		bool IsLightingEnabled() const;
		bool IsZTestEnabled() const;
		bool IsZWriteEnabled() const;

		bool LoadFromFile(const NzString& filePath, const NzMaterialParams& params = NzMaterialParams());
		bool LoadFromMemory(const void* data, std::size_t size, const NzMaterialParams& params = NzMaterialParams());
		bool LoadFromStream(NzInputStream& stream, const NzMaterialParams& params = NzMaterialParams());

		void Reset();

		void SetAmbientColor(const NzColor& ambient);
		void SetDiffuseColor(const NzColor& diffuse);
		void SetDiffuseMap(NzTexture* map);
		void SetDiffuseSampler(const NzTextureSampler& sampler);
		void SetDstBlend(nzBlendFunc func);
		void SetFaceCulling(nzFaceCulling culling);
		void SetFaceFilling(nzFaceFilling filling);
		void SetHeightMap(NzTexture* map);
		void SetNormalMap(NzTexture* map);
		void SetShader(const NzShader* shader);
		void SetShininess(float shininess);
		void SetSpecularColor(const NzColor& specular);
		void SetSpecularMap(NzTexture* map);
		void SetSpecularSampler(const NzTextureSampler& sampler);
		void SetSrcBlend(nzBlendFunc func);
		void SetZTestCompare(nzRendererComparison compareFunc);

		NzMaterial& operator=(const NzMaterial& material);
		NzMaterial& operator=(NzMaterial&& material);

		static NzMaterial* GetDefault();

	private:
		void UpdateShader() const;

		nzBlendFunc m_dstBlend;
		nzBlendFunc m_srcBlend;
		nzFaceCulling m_faceCulling;
		nzFaceFilling m_faceFilling;
		nzRendererComparison m_zTestCompareFunc;
		NzColor m_ambientColor;
		NzColor m_diffuseColor;
		NzColor m_specularColor;
		NzTextureSampler m_diffuseSampler;
		NzTextureSampler m_specularSampler;
		mutable const NzShader* m_shader;
		NzTexture* m_diffuseMap;
		NzTexture* m_heightMap;
		NzTexture* m_normalMap;
		NzTexture* m_specularMap;
		bool m_alphaBlendingEnabled;
		bool m_autoShader;
		bool m_faceCullingEnabled;
		bool m_lightingEnabled;
		bool m_zTestEnabled;
		bool m_zWriteEnabled;
		float m_shininess;

		static NzMaterialLoader::LoaderList s_loaders;
};

#endif // NAZARA_MATERIAL_HPP
