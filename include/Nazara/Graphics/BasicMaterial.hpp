// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_BASIC_MATERIAL_HPP
#define NAZARA_BASIC_MATERIAL_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/Material.hpp>

namespace Nz
{
	class NAZARA_GRAPHICS_API BasicMaterial
	{
		friend class MaterialPipeline;

		public:
			BasicMaterial(Material* material);

			inline const TextureRef& GetAlphaMap() const;
			float GetAlphaThreshold() const;
			Color GetDiffuseColor() const;
			inline const TextureRef& GetDiffuseMap() const;

			inline bool HasAlphaMap() const;
			inline bool HasAlphaThreshold() const;
			inline bool HasDiffuseColor() const;
			inline bool HasDiffuseMap() const;

			inline bool SetAlphaMap(const String& textureName);
			inline void SetAlphaMap(TextureRef alphaMap);
			void SetAlphaThreshold(float alphaThreshold);
			void SetDiffuseColor(const Color& diffuse);
			inline bool SetDiffuseMap(const String& textureName);
			inline void SetDiffuseMap(TextureRef diffuseMap);

			static const std::shared_ptr<MaterialSettings>& GetSettings();

		private:
			struct UniformOffsets
			{
				std::size_t alphaThreshold;
				std::size_t diffuseColor;
			};

			struct TextureIndexes
			{
				std::size_t alpha;
				std::size_t diffuse;
			};

			static bool Initialize();
			static void Uninitialize();

			MaterialRef m_material;
			std::size_t m_uniformBlockIndex;
			TextureIndexes m_textureIndexes;
			UniformOffsets m_uniformOffsets;

			static std::shared_ptr<MaterialSettings> s_materialSettings;
			static std::size_t s_uniformBlockIndex;
			static RenderPipelineLayoutRef s_renderPipelineLayout;
			static TextureIndexes s_textureIndexes;
			static UniformOffsets s_uniformOffsets;
	};
}

#include <Nazara/Graphics/BasicMaterial.inl>

#endif // NAZARA_BASIC_MATERIAL_HPP
