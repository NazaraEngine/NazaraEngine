// Copyright (C) 2014 Jérôme Leclercq
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
#include <Nazara/Renderer/ShaderProgram.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Renderer/TextureSampler.hpp>

struct NAZARA_API NzMaterialParams
{
	bool loadAlphaMap = true;
	bool loadDiffuseMap = true;
	bool loadEmissiveMap = true;
	bool loadHeightMap = true;
	bool loadNormalMap = true;
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
	friend class NzRenderer;

	public:
		NzMaterial();
		NzMaterial(const NzMaterial& material);
		NzMaterial(NzMaterial&& material);
		~NzMaterial();

		void Apply(const NzShaderProgram* program) const;

		void Enable(nzRendererParameter renderParameter, bool enable);
		void EnableAlphaTest(bool alphaTest);
		void EnableLighting(bool lighting);

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
		const NzShaderProgram* GetShaderProgram(nzShaderTarget target, nzUInt32 flags) const;
		float GetShininess() const;
		NzColor GetSpecularColor() const;
		NzTexture* GetSpecularMap() const;
		NzTextureSampler& GetSpecularSampler();
		const NzTextureSampler& GetSpecularSampler() const;
		nzBlendFunc GetSrcBlend() const;

		bool HasAlphaMap() const;
		bool HasCustomShaderProgram(nzShaderTarget target, nzUInt32 flags) const;
		bool HasDiffuseMap() const;
		bool HasEmissiveMap() const;
		bool HasHeightMap() const;
		bool HasNormalMap() const;
		bool HasSpecularMap() const;

		bool IsAlphaTestEnabled() const;
		bool IsEnabled(nzRendererParameter renderParameter) const;
		bool IsLightingEnabled() const;

		bool LoadFromFile(const NzString& filePath, const NzMaterialParams& params = NzMaterialParams());
		bool LoadFromMemory(const void* data, std::size_t size, const NzMaterialParams& params = NzMaterialParams());
		bool LoadFromStream(NzInputStream& stream, const NzMaterialParams& params = NzMaterialParams());

		void Reset();

		bool SetAlphaMap(const NzString& texturePath);
		void SetAlphaMap(NzTexture* map);
		void SetAlphaThreshold(float alphaThreshold);
		void SetAmbientColor(const NzColor& ambient);
		void SetDepthFunc(nzRendererComparison depthFunc);
		void SetDiffuseColor(const NzColor& diffuse);
		bool SetDiffuseMap(const NzString& texturePath);
		void SetDiffuseMap(NzTexture* map);
		void SetDiffuseSampler(const NzTextureSampler& sampler);
		void SetDstBlend(nzBlendFunc func);
		bool SetEmissiveMap(const NzString& texturePath);
		void SetEmissiveMap(NzTexture* map);
		void SetFaceCulling(nzFaceSide faceSide);
		void SetFaceFilling(nzFaceFilling filling);
		bool SetHeightMap(const NzString& texturePath);
		void SetHeightMap(NzTexture* map);
		bool SetNormalMap(const NzString& texturePath);
		void SetNormalMap(NzTexture* map);
		void SetRenderStates(const NzRenderStates& states);
		void SetShaderProgram(nzShaderTarget target, nzUInt32 flags, const NzShaderProgram* program);
		void SetShininess(float shininess);
		void SetSpecularColor(const NzColor& specular);
		bool SetSpecularMap(const NzString& texturePath);
		void SetSpecularMap(NzTexture* map);
		void SetSpecularSampler(const NzTextureSampler& sampler);
		void SetSrcBlend(nzBlendFunc func);

		NzMaterial& operator=(const NzMaterial& material);
		NzMaterial& operator=(NzMaterial&& material);

		static NzMaterial* GetDefault();

	private:
		struct ProgramUnit
		{
			NzShaderProgramConstRef program;
			bool custom = false;
		};

		void Copy(const NzMaterial& material);
		void GenerateProgram(nzShaderTarget target, nzUInt32 flags) const;
		void InvalidatePrograms(nzShaderTarget target);

		static bool Initialize();
		static void Uninitialize();

		NzColor m_ambientColor;
		NzColor m_diffuseColor;
		NzColor m_specularColor;
		NzRenderStates m_states;
		mutable ProgramUnit m_programs[nzShaderTarget_Max+1][nzShaderFlags_Max+1];
		NzTextureSampler m_diffuseSampler;
		NzTextureSampler m_specularSampler;
		NzTextureRef m_alphaMap;
		NzTextureRef m_diffuseMap;
		NzTextureRef m_emissiveMap;
		NzTextureRef m_heightMap;
		NzTextureRef m_normalMap;
		NzTextureRef m_specularMap;
		bool m_alphaTestEnabled;
		bool m_lightingEnabled;
		float m_alphaThreshold;
		float m_shininess;

		static NzMaterial* s_defaultMaterial;
		static NzMaterialLoader::LoaderList s_loaders;
};

#endif // NAZARA_MATERIAL_HPP
