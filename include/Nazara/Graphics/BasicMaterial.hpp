// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_BASICMATERIAL_HPP
#define NAZARA_GRAPHICS_BASICMATERIAL_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/MaterialPass.hpp>

namespace nzsl
{
	class FieldOffsets;
}

namespace Nz
{
	class NAZARA_GRAPHICS_API BasicMaterial
	{
		friend class MaterialPipeline;

		public:
			struct BasicUniformOffsets;

			BasicMaterial(MaterialPass& material);
			~BasicMaterial() = default;

			inline void EnableAlphaTest(bool alphaTest);

			inline const std::shared_ptr<Texture>& GetAlphaMap() const;
			inline const TextureSamplerInfo& GetAlphaSampler() const;
			float GetAlphaTestThreshold() const;
			Color GetBaseColor() const;
			inline const std::shared_ptr<Texture>& GetBaseColorMap() const;
			inline const TextureSamplerInfo& GetBaseColorSampler() const;

			inline bool IsAlphaTestEnabled() const;

			inline bool HasAlphaMap() const;
			inline bool HasAlphaTest() const;
			inline bool HasAlphaTestThreshold() const;
			inline bool HasBaseColor() const;
			inline bool HasBaseColorMap() const;

			inline void SetAlphaMap(std::shared_ptr<Texture> alphaMap);
			inline void SetAlphaSampler(TextureSamplerInfo alphaSampler);
			void SetAlphaTestThreshold(float alphaThreshold);
			void SetBaseColor(const Color& baseColor);
			inline void SetBaseColorMap(std::shared_ptr<Texture> baseColorMap);
			inline void SetBaseColorSampler(TextureSamplerInfo baseColorSampler);

			static inline const BasicUniformOffsets& GetOffsets();
			static inline const std::shared_ptr<MaterialSettings>& GetSettings();

			struct BasicUniformOffsets
			{
				std::size_t alphaThreshold;
				std::size_t baseColor;
				std::size_t totalSize;
			};

		protected:
			struct NoInit {};

			inline BasicMaterial(MaterialPass& material, NoInit);

			struct BasicOptionIndexes
			{
				std::size_t alphaTest;
				std::size_t hasAlphaMap;
				std::size_t hasBaseColorMap;
			};

			struct BasicTextureIndexes
			{
				std::size_t alpha;
				std::size_t baseColor;
			};

			struct BasicBuildOptions
			{
				// Common
				std::vector<UInt8> defaultValues;
				std::size_t* uniformBlockIndex = nullptr;
				std::vector<std::shared_ptr<UberShader>> shaders;

				// Basic
				BasicUniformOffsets basicOffsets;
				BasicOptionIndexes* basicOptionIndexes = nullptr;
				BasicTextureIndexes* basicTextureIndexes = nullptr;
			};

			inline MaterialPass& GetMaterial();
			inline const MaterialPass& GetMaterial() const;

			static MaterialSettings::Builder Build(BasicBuildOptions& options);
			static std::vector<std::shared_ptr<UberShader>> BuildShaders();
			static std::pair<BasicUniformOffsets, nzsl::FieldOffsets> BuildUniformOffsets();

			std::size_t m_uniformBlockIndex;
			BasicOptionIndexes m_basicOptionIndexes;
			BasicTextureIndexes m_basicTextureIndexes;
			BasicUniformOffsets m_basicUniformOffsets;

			static std::shared_ptr<MaterialSettings> s_basicMaterialSettings;
			static std::size_t s_uniformBlockIndex;
			static BasicOptionIndexes s_basicOptionIndexes;
			static BasicTextureIndexes s_basicTextureIndexes;
			static BasicUniformOffsets s_basicUniformOffsets;

		private:
			static bool Initialize();
			static void Uninitialize();

			MaterialPass& m_material;
	};
}

#include <Nazara/Graphics/BasicMaterial.inl>

#endif // NAZARA_GRAPHICS_BASICMATERIAL_HPP
