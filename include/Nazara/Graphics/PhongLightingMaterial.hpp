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
		public:
			PhongLightingMaterial(Material* material);

			inline const Texture* GetAlphaMap() const;
			inline float GetAlphaThreshold() const;
			inline Color GetAmbientColor() const;
			inline Color GetDiffuseColor() const;
			inline const Texture* GetDiffuseMap() const;
			inline TextureSampler& GetDiffuseSampler();
			inline const TextureSampler& GetDiffuseSampler() const;
			inline const Texture* GetEmissiveMap() const;
			inline const Texture* GetHeightMap() const;
			inline const Texture* GetNormalMap() const;
			inline float GetShininess() const;
			inline Color GetSpecularColor() const;
			inline const Texture* GetSpecularMap() const;
			inline TextureSampler& GetSpecularSampler();
			inline const TextureSampler& GetSpecularSampler() const;

			inline bool HasAlphaMap() const;
			inline bool HasDiffuseMap() const;
			inline bool HasEmissiveMap() const;
			inline bool HasHeightMap() const;
			inline bool HasNormalMap() const;
			inline bool HasSpecularMap() const;

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
			TextureIndexes m_textureIndexes;
			UniformOffsets m_uniformOffsets;

			static std::shared_ptr<MaterialSettings> s_materialSettings;
			static RenderPipelineLayoutRef s_renderPipelineLayout;
			static TextureIndexes s_textureIndexes;
			static UniformOffsets s_uniformOffsets;
	};
}

#include <Nazara/Graphics/PhongLightingMaterial.inl>

#endif // NAZARA_PHONG_LIGHTING_MATERIAL_HPP
