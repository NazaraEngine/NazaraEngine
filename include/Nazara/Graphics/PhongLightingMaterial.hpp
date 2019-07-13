// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PHONG_LIGHTING_MATERIAL_HPP
#define NAZARA_PHONG_LIGHTING_MATERIAL_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/Material.hpp>

namespace Nz
{
	class NAZARA_GRAPHICS_API PhongLightingMaterial
	{
		friend class Material;

		public:
			PhongLightingMaterial(Material* material);

			inline const TextureRef& GetAlphaMap() const;
			float GetAlphaThreshold() const;
			Color GetAmbientColor() const;
			Color GetDiffuseColor() const;
			inline const TextureRef& GetDiffuseMap() const;
			inline TextureSampler& GetDiffuseSampler();
			inline const TextureSampler& GetDiffuseSampler() const;
			inline const TextureRef& GetEmissiveMap() const;
			inline const TextureRef& GetHeightMap() const;
			inline const TextureRef& GetNormalMap() const;
			float GetShininess() const;
			Color GetSpecularColor() const;
			inline const TextureRef& GetSpecularMap() const;
			inline TextureSampler& GetSpecularSampler();
			inline const TextureSampler& GetSpecularSampler() const;

			inline bool HasAlphaMap() const;
			inline bool HasAlphaThreshold() const;
			inline bool HasAmbientColor() const;
			inline bool HasDiffuseColor() const;
			inline bool HasDiffuseMap() const;
			inline bool HasEmissiveMap() const;
			inline bool HasHeightMap() const;
			inline bool HasNormalMap() const;
			inline bool HasShininess() const;
			inline bool HasSpecularColor() const;
			inline bool HasSpecularMap() const;

			inline bool SetAlphaMap(const String& textureName);
			inline void SetAlphaMap(TextureRef alphaMap);
			void SetAlphaThreshold(float alphaThreshold);
			void SetAmbientColor(const Color& ambient);
			void SetDiffuseColor(const Color& diffuse);
			inline bool SetDiffuseMap(const String& textureName);
			inline void SetDiffuseMap(TextureRef diffuseMap);
			inline void SetDiffuseSampler(const TextureSampler& sampler);
			inline bool SetEmissiveMap(const String& textureName);
			inline void SetEmissiveMap(TextureRef textureName);
			inline bool SetHeightMap(const String& textureName);
			inline void SetHeightMap(TextureRef textureName);
			inline bool SetNormalMap(const String& textureName);
			inline void SetNormalMap(TextureRef textureName);
			void SetShininess(float shininess);
			void SetSpecularColor(const Color& specular);
			inline bool SetSpecularMap(const String& textureName);
			inline void SetSpecularMap(TextureRef specularMap);
			inline void SetSpecularSampler(const TextureSampler& sampler);

			static const std::shared_ptr<MaterialSettings>& GetSettings();

		private:
			struct UniformOffsets
			{
				std::size_t alphaThreshold;
				std::size_t shininess;
				std::size_t ambientColor;
				std::size_t diffuseColor;
				std::size_t specularColor;
			};

			struct TextureIndexes
			{
				std::size_t alpha;
				std::size_t diffuse;
				std::size_t emissive;
				std::size_t height;
				std::size_t normal;
				std::size_t specular;
			};

			static bool Initialize();
			static void Uninitialize();

			MaterialRef m_material;
			std::size_t m_phongUniformIndex;
			TextureIndexes m_textureIndexes;
			UniformOffsets m_phongUniformOffsets;

			static std::shared_ptr<MaterialSettings> s_materialSettings;
			static std::size_t s_phongUniformBlockIndex;
			static RenderPipelineLayoutRef s_renderPipelineLayout;
			static TextureIndexes s_textureIndexes;
			static UniformOffsets s_phongUniformOffsets;
	};
}

#include <Nazara/Graphics/PhongLightingMaterial.inl>

#endif // NAZARA_PHONG_LIGHTING_MATERIAL_HPP
