// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_PHYSICALLYBASEDMATERIAL_HPP
#define NAZARA_GRAPHICS_PHYSICALLYBASEDMATERIAL_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/BasicMaterial.hpp>
#include <Nazara/Graphics/MaterialPass.hpp>

namespace Nz
{
	class NAZARA_GRAPHICS_API PhysicallyBasedMaterial : public BasicMaterial
	{
		friend class MaterialPipeline;

		public:
			PhysicallyBasedMaterial(MaterialPass& material);

			Color GetAmbientColor() const;
			inline const std::shared_ptr<Texture>& GetEmissiveMap() const;
			inline const TextureSamplerInfo& GetEmissiveSampler() const;
			inline const std::shared_ptr<Texture>& GetHeightMap() const;
			inline const TextureSamplerInfo& GetHeightSampler() const;
			inline const std::shared_ptr<Texture>& GetMetallicMap() const;
			inline const TextureSamplerInfo& GetMetallicSampler() const;
			inline const std::shared_ptr<Texture>& GetNormalMap() const;
			inline const TextureSamplerInfo& GetNormalSampler() const;
			inline const std::shared_ptr<Texture>& GetRoughnessMap() const;
			inline const TextureSamplerInfo& GetRoughnessSampler() const;
			float GetShininess() const;
			Color GetSpecularColor() const;
			inline const std::shared_ptr<Texture>& GetSpecularMap() const;
			inline const TextureSamplerInfo& GetSpecularSampler() const;

			inline bool HasAmbientColor() const;
			inline bool HasEmissiveMap() const;
			inline bool HasHeightMap() const;
			inline bool HasMetallicMap() const;
			inline bool HasNormalMap() const;
			inline bool HasRoughnessMap() const;
			inline bool HasShininess() const;
			inline bool HasSpecularColor() const;
			inline bool HasSpecularMap() const;

			void SetAmbientColor(const Color& ambient);
			inline void SetEmissiveMap(std::shared_ptr<Texture> emissiveMap);
			inline void SetEmissiveSampler(TextureSamplerInfo emissiveSampler);
			inline void SetHeightMap(std::shared_ptr<Texture> heightMap);
			inline void SetHeightSampler(TextureSamplerInfo heightSampler);
			inline void SetMetallicMap(std::shared_ptr<Texture> metallicMap);
			inline void SetMetallicSampler(TextureSamplerInfo metallicSampler);
			inline void SetNormalMap(std::shared_ptr<Texture> normalMap);
			inline void SetNormalSampler(TextureSamplerInfo normalSampler);
			inline void SetRoughnessMap(std::shared_ptr<Texture> roughnessMap);
			inline void SetRoughnessSampler(TextureSamplerInfo roughnessSampler);
			void SetShininess(float shininess);
			void SetSpecularColor(const Color& specular);
			inline void SetSpecularMap(std::shared_ptr<Texture> specularMap);
			inline void SetSpecularSampler(TextureSamplerInfo specularSampler);

			static const std::shared_ptr<MaterialSettings>& GetSettings();

		protected:
			struct PbrOptionIndexes
			{
				std::size_t hasEmissiveMap;
				std::size_t hasHeightMap;
				std::size_t hasMetallicMap;
				std::size_t hasNormalMap;
				std::size_t hasRoughnessMap;
				std::size_t hasSpecularMap;
			};

			struct PbrUniformOffsets
			{
				std::size_t ambientColor;
				std::size_t shininess;
				std::size_t totalSize;
				std::size_t specularColor;
			};

			struct PbrTextureIndexes
			{
				std::size_t emissive;
				std::size_t height;
				std::size_t metallic;
				std::size_t roughness;
				std::size_t normal;
				std::size_t specular;
			};

			struct PbrBuildOptions : BasicBuildOptions
			{
				PbrUniformOffsets pbrOffsets;
				PbrOptionIndexes* pbrOptionIndexes = nullptr;
				PbrTextureIndexes* pbrTextureIndexes = nullptr;
			};

			PbrOptionIndexes  m_pbrOptionIndexes;
			PbrTextureIndexes m_pbrTextureIndexes;
			PbrUniformOffsets m_pbrUniformOffsets;

			static MaterialSettings::Builder Build(PbrBuildOptions& options);
			static std::vector<std::shared_ptr<UberShader>> BuildShaders();
			static std::pair<PbrUniformOffsets, FieldOffsets> BuildUniformOffsets();

		private:
			static bool Initialize();
			static void Uninitialize();

			static std::shared_ptr<MaterialSettings> s_pbrMaterialSettings;
			static std::size_t s_pbrUniformBlockIndex;
			static PbrOptionIndexes s_pbrOptionIndexes;
			static PbrTextureIndexes s_pbrTextureIndexes;
			static PbrUniformOffsets s_pbrUniformOffsets;
	};
}

#include <Nazara/Graphics/PhysicallyBasedMaterial.inl>

#endif // NAZARA_GRAPHICS_PHYSICALLYBASEDMATERIAL_HPP
