// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PHONG_LIGHTING_MATERIAL_HPP
#define NAZARA_PHONG_LIGHTING_MATERIAL_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/MaterialPass.hpp>

namespace Nz
{
	class NAZARA_GRAPHICS_API PhongLightingMaterial
	{
		friend class MaterialPipeline;

		public:
			PhongLightingMaterial(MaterialPass& material);

			inline const std::shared_ptr<Texture>& GetAlphaMap() const;
			float GetAlphaThreshold() const;
			Color GetAmbientColor() const;
			Color GetDiffuseColor() const;
			inline const std::shared_ptr<Texture>& GetDiffuseMap() const;
			inline TextureSampler& GetDiffuseSampler();
			inline const TextureSampler& GetDiffuseSampler() const;
			inline const std::shared_ptr<Texture>& GetEmissiveMap() const;
			inline const std::shared_ptr<Texture>& GetHeightMap() const;
			inline const std::shared_ptr<Texture>& GetNormalMap() const;
			float GetShininess() const;
			Color GetSpecularColor() const;
			inline const std::shared_ptr<Texture>& GetSpecularMap() const;
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

			inline void SetAlphaMap(std::shared_ptr<Texture> alphaMap);
			void SetAlphaThreshold(float alphaThreshold);
			void SetAmbientColor(const Color& ambient);
			void SetDiffuseColor(const Color& diffuse);
			inline void SetDiffuseMap(std::shared_ptr<Texture> diffuseMap);
			inline void SetDiffuseSampler(const TextureSampler& sampler);
			inline void SetEmissiveMap(std::shared_ptr<Texture> textureName);
			inline void SetHeightMap(std::shared_ptr<Texture> textureName);
			inline void SetNormalMap(std::shared_ptr<Texture> textureName);
			void SetShininess(float shininess);
			void SetSpecularColor(const Color& specular);
			inline void SetSpecularMap(std::shared_ptr<Texture> specularMap);
			inline void SetSpecularSampler(const TextureSampler& sampler);

			static const std::shared_ptr<MaterialSettings>& GetSettings();

		private:
			struct PhongUniformOffsets
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

			MaterialPass& m_material;
			std::size_t m_phongUniformIndex;
			TextureIndexes m_textureIndexes;
			PhongUniformOffsets m_phongUniformOffsets;

			static std::shared_ptr<MaterialSettings> s_materialSettings;
			static std::size_t s_phongUniformBlockIndex;
			static TextureIndexes s_textureIndexes;
			static PhongUniformOffsets s_phongUniformOffsets;
	};
}

#include <Nazara/Graphics/PhongLightingMaterial.inl>

#endif // NAZARA_PHONG_LIGHTING_MATERIAL_HPP
