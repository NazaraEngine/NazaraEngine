// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PHONG_LIGHTING_MATERIAL_HPP
#define NAZARA_PHONG_LIGHTING_MATERIAL_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/BaseMaterial.hpp>

namespace Nz
{
	struct NAZARA_GRAPHICS_API PhongMaterialParams : ResourceParameters
	{
		bool loadAlphaMap = true;
		bool loadDiffuseMap = true;
		bool loadEmissiveMap = true;
		bool loadHeightMap = true;
		bool loadNormalMap = true;
		bool loadSpecularMap = true;
		String shaderName = "Basic";

		bool IsValid() const;
	};

	class NAZARA_GRAPHICS_API PhongLightingMaterial : public BaseMaterial
	{
		public:
			inline PhongLightingMaterial();
			inline PhongLightingMaterial(const MaterialPipeline* pipeline);
			inline PhongLightingMaterial(const MaterialPipelineInfo& pipelineInfo);
			inline PhongLightingMaterial(const String& pipelineName);
			inline PhongLightingMaterial(const PhongLightingMaterial& material);
			inline ~PhongLightingMaterial();

			void Apply(const MaterialPipeline::Instance& instance) const override;

			inline const TextureRef& GetAlphaMap() const;
			inline float GetAlphaThreshold() const;
			inline Color GetAmbientColor() const;
			inline Color GetDiffuseColor() const;
			inline const TextureRef& GetDiffuseMap() const;
			inline TextureSampler& GetDiffuseSampler();
			inline const TextureSampler& GetDiffuseSampler() const;
			inline const TextureRef& GetEmissiveMap() const;
			inline const TextureRef& GetHeightMap() const;
			inline const TextureRef& GetNormalMap() const;
			inline float GetShininess() const;
			inline Color GetSpecularColor() const;
			inline const TextureRef& GetSpecularMap() const;
			inline TextureSampler& GetSpecularSampler();
			inline const TextureSampler& GetSpecularSampler() const;

			inline bool HasAlphaMap() const;
			inline bool HasDepthMaterial() const;
			inline bool HasDiffuseMap() const;
			inline bool HasEmissiveMap() const;
			inline bool HasHeightMap() const;
			inline bool HasNormalMap() const;
			inline bool HasSpecularMap() const;

			inline bool LoadFromFile(const String& filePath, const PhongMaterialParams& params = PhongMaterialParams());
			inline bool LoadFromMemory(const void* data, std::size_t size, const PhongMaterialParams& params = PhongMaterialParams());
			inline bool LoadFromStream(Stream& stream, const PhongMaterialParams& params = PhongMaterialParams());

			void SaveToParameters(ParameterList* matData) override;

			inline bool SetAlphaMap(const String& textureName);
			inline void SetAlphaMap(TextureRef alphaMap);
			inline void SetAlphaThreshold(float alphaThreshold);
			inline void SetAmbientColor(const Color& ambient);
			inline void SetDiffuseColor(const Color& diffuse);
			inline bool SetDiffuseMap(const String& textureName);
			inline void SetDiffuseMap(TextureRef diffuseMap);
			inline void SetDiffuseSampler(const TextureSampler& sampler);
			inline bool SetEmissiveMap(const String& textureName);
			inline void SetEmissiveMap(TextureRef textureName);
			inline bool SetHeightMap(const String& textureName);
			inline void SetHeightMap(TextureRef textureName);
			inline bool SetNormalMap(const String& textureName);
			inline void SetNormalMap(TextureRef textureName);
			inline void SetShininess(float shininess);
			inline void SetSpecularColor(const Color& specular);
			inline bool SetSpecularMap(const String& textureName);
			inline void SetSpecularMap(TextureRef specularMap);
			inline void SetSpecularSampler(const TextureSampler& sampler);

			inline BaseMaterial& operator=(const BaseMaterial& material);

			inline static MaterialRef GetDefault();
			inline static int GetTextureUnit(TextureMap textureMap);
			template<typename... Args> static MaterialRef New(Args&&... args);

		private:
			struct ShaderCache;

			void Copy(const BaseMaterial& material);
			const ShaderCache& GetShaderCache(const Shader* shader) const;
			void InvalidateShader(Shader* shader);

			static bool Initialize();
			static void Uninitialize();

			struct ShaderCache
			{
				Shader::LayoutBindings bindings;

				NazaraSlot(Shader, OnShaderDestroy, onDestroy);
				NazaraSlot(Shader, OnShaderUniformInvalidated, onUniformInvalidated);
			};

			mutable std::unordered_map<const Shader*, ShaderCache> m_shaderCache;
			Color m_ambientColor;
			Color m_diffuseColor;
			Color m_specularColor;
			TextureSampler m_diffuseSampler;
			TextureSampler m_specularSampler;
			TextureRef m_alphaMap;
			TextureRef m_diffuseMap;
			TextureRef m_emissiveMap;
			TextureRef m_heightMap;
			TextureRef m_normalMap;
			TextureRef m_specularMap;
			float m_alphaThreshold;
			float m_shininess;

			static RenderPipelineLayoutRef s_renderPipelineLayout;
	};
}

#include <Nazara/Graphics/PhongLightingMaterial.inl>

#endif // NAZARA_PHONG_LIGHTING_MATERIAL_HPP
