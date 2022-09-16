// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_PHONGLIGHTINGMATERIALPASS_HPP
#define NAZARA_GRAPHICS_PHONGLIGHTINGMATERIALPASS_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/BasicMaterialPass.hpp>
#include <Nazara/Graphics/MaterialPass.hpp>

namespace Nz
{
	class NAZARA_GRAPHICS_API PhongLightingMaterialPass : public BasicMaterialPass
	{
		friend class MaterialPipeline;

		public:
			PhongLightingMaterialPass(MaterialPass& material);

			Color GetAmbientColor() const;
			inline const std::shared_ptr<Texture>& GetEmissiveMap() const;
			inline const TextureSamplerInfo& GetEmissiveSampler() const;
			inline const std::shared_ptr<Texture>& GetHeightMap() const;
			inline const TextureSamplerInfo& GetHeightSampler() const;
			inline const std::shared_ptr<Texture>& GetNormalMap() const;
			inline const TextureSamplerInfo& GetNormalSampler() const;
			float GetShininess() const;
			Color GetSpecularColor() const;
			inline const std::shared_ptr<Texture>& GetSpecularMap() const;
			inline const TextureSamplerInfo& GetSpecularSampler() const;

			inline bool HasAmbientColor() const;
			inline bool HasEmissiveMap() const;
			inline bool HasHeightMap() const;
			inline bool HasNormalMap() const;
			inline bool HasShininess() const;
			inline bool HasSpecularColor() const;
			inline bool HasSpecularMap() const;

			void SetAmbientColor(const Color& ambient);
			inline void SetEmissiveMap(std::shared_ptr<Texture> emissiveMap);
			inline void SetEmissiveSampler(TextureSamplerInfo emissiveSampler);
			inline void SetHeightMap(std::shared_ptr<Texture> heightMap);
			inline void SetHeightSampler(TextureSamplerInfo heightSampler);
			inline void SetNormalMap(std::shared_ptr<Texture> normalMap);
			inline void SetNormalSampler(TextureSamplerInfo normalSampler);
			void SetShininess(float shininess);
			void SetSpecularColor(const Color& specular);
			inline void SetSpecularMap(std::shared_ptr<Texture> specularMap);
			inline void SetSpecularSampler(TextureSamplerInfo specularSampler);

			static const std::shared_ptr<MaterialSettings>& GetSettings();

		protected:
			struct PhongOptionIndexes
			{
				std::size_t hasEmissiveMap;
				std::size_t hasHeightMap;
				std::size_t hasNormalMap;
				std::size_t hasSpecularMap;
			};

			struct PhongUniformOffsets
			{
				std::size_t ambientColor;
				std::size_t shininess;
				std::size_t totalSize;
				std::size_t specularColor;
			};

			struct PhongTextureIndexes
			{
				std::size_t emissive;
				std::size_t height;
				std::size_t normal;
				std::size_t specular;
			};

			struct PhongBuildOptions : BasicBuildOptions
			{
				PhongUniformOffsets phongOffsets;
				PhongOptionIndexes* phongOptionIndexes = nullptr;
				PhongTextureIndexes* phongTextureIndexes = nullptr;
			};

			PhongOptionIndexes  m_phongOptionIndexes;
			PhongTextureIndexes m_phongTextureIndexes;
			PhongUniformOffsets m_phongUniformOffsets;

			static MaterialSettings::Builder Build(PhongBuildOptions& options);
			static std::vector<std::shared_ptr<UberShader>> BuildShaders();
			static std::pair<PhongUniformOffsets, nzsl::FieldOffsets> BuildUniformOffsets();

		private:
			static bool Initialize();
			static void Uninitialize();

			static std::shared_ptr<MaterialSettings> s_phongMaterialSettings;
			static std::size_t s_phongUniformBlockIndex;
			static PhongOptionIndexes s_phongOptionIndexes;
			static PhongTextureIndexes s_phongTextureIndexes;
			static PhongUniformOffsets s_phongUniformOffsets;
	};
}

#include <Nazara/Graphics/PhongLightingMaterialPass.inl>

#endif // NAZARA_GRAPHICS_PHONGLIGHTINGMATERIALPASS_HPP
