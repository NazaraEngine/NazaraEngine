// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_LINEARSLICEDSPRITE_HPP
#define NAZARA_GRAPHICS_LINEARSLICEDSPRITE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/VertexDeclaration.hpp>
#include <Nazara/Core/VertexStruct.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Renderer/RenderPipeline.hpp>
#include <array>

namespace Nz
{
	class NAZARA_GRAPHICS_API LinearSlicedSprite : public InstancedRenderable
	{
		public:
			enum class Orientation;
			struct Section;

			LinearSlicedSprite(std::shared_ptr<MaterialInstance> material, Orientation orientation);
			LinearSlicedSprite(const LinearSlicedSprite&) = delete;
			LinearSlicedSprite(LinearSlicedSprite&&) noexcept = default;
			~LinearSlicedSprite() = default;

			inline void AddSection(float size, float textureCoord);

			void BuildElement(ElementRendererRegistry& registry, const ElementData& elementData, std::size_t passIndex, std::vector<RenderElementOwner>& elements) const override;

			inline void Clear();

			inline const Color& GetColor() const;
			const std::shared_ptr<MaterialInstance>& GetMaterial(std::size_t materialIndex = 0) const override;
			std::size_t GetMaterialCount() const override;
			inline Orientation GetOrientation() const;
			inline const Vector2f& GetOrigin() const;
			inline const Section& GetSection(std::size_t sectionIndex) const;
			std::size_t GetSectionCount() const;
			inline float GetSize() const;
			inline const Rectf& GetTextureCoords() const;
			Vector3ui GetTextureSize() const;

			inline void RemoveSection(std::size_t sectionIndex);

			inline void SetColor(const Color& color);
			inline void SetMaterial(std::shared_ptr<MaterialInstance> material);
			inline void SetOrigin(const Vector2f& origin);
			inline void SetSection(std::size_t sectionIndex, float size, float textureCoord);
			inline void SetSectionSize(std::size_t sectionIndex, float size);
			inline void SetSectionTextureCoord(std::size_t sectionIndex, float textureCoord);
			inline void SetSize(float size);
			inline void SetTextureCoords(const Rectf& textureCoords);
			inline void SetTextureRect(const Rectf& textureRect);

			LinearSlicedSprite& operator=(const LinearSlicedSprite&) = delete;
			LinearSlicedSprite& operator=(LinearSlicedSprite&&) noexcept = default;

			enum class Orientation
			{
				Horizontal,
				Vertical
			};

			struct Section
			{
				float size;
				float textureCoord;
			};

			static constexpr std::size_t MaxSection = 5;

		private:
			void UpdateVertices();

			std::array<Section, MaxSection> m_sections;
			std::array<VertexStruct_XYZ_Color_UV, 4 * MaxSection> m_vertices;
			std::shared_ptr<MaterialInstance> m_material;
			std::size_t m_sectionCount;
			std::size_t m_spriteCount;
			Color m_color;
			Orientation m_orientation;
			Rectf m_textureCoords;
			Vector2f m_origin;
			float m_size;
	};
}

#include <Nazara/Graphics/LinearSlicedSprite.inl>

#endif // NAZARA_GRAPHICS_LINEARSLICEDSPRITE_HPP
