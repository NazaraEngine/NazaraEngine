// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_SLICEDSPRITE_HPP
#define NAZARA_GRAPHICS_SLICEDSPRITE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Renderer/RenderPipeline.hpp>
#include <Nazara/Utility/VertexDeclaration.hpp>
#include <Nazara/Utility/VertexStruct.hpp>
#include <array>

namespace Nz
{
	class NAZARA_GRAPHICS_API SlicedSprite : public InstancedRenderable
	{
		public:
			struct Corner;

			SlicedSprite(std::shared_ptr<MaterialInstance> material);
			SlicedSprite(const SlicedSprite&) = delete;
			SlicedSprite(SlicedSprite&&) noexcept = default;
			~SlicedSprite() = default;

			void BuildElement(ElementRendererRegistry& registry, const ElementData& elementData, std::size_t passIndex, std::vector<RenderElementOwner>& elements) const override;

			inline const Color& GetColor() const;
			inline const Corner& GetBottomRightCorner() const;
			const std::shared_ptr<MaterialInstance>& GetMaterial(std::size_t materialIndex = 0) const override;
			std::size_t GetMaterialCount() const override;
			inline const Vector2f& GetOrigin() const;
			inline const Vector2f& GetSize() const;
			inline const Corner& GetTopLeftCorner() const;
			inline const Rectf& GetTextureCoords() const;
			Vector3ui GetTextureSize() const;

			inline void SetColor(const Color& color);
			inline void SetCorners(const Corner& topLeftCorner, const Corner& bottomRightCorner);
			inline void SetCornersSize(const Vector2f& topLeftSize, const Vector2f& bottomRightSize);
			inline void SetCornersTextureCoords(const Vector2f& topLeftTextureCoords, const Vector2f& bottomRightTextureCoords);
			inline void SetMaterial(std::shared_ptr<MaterialInstance> material);
			inline void SetOrigin(const Vector2f& origin);
			inline void SetSize(const Vector2f& size);
			inline void SetTextureCoords(const Rectf& textureCoords);
			inline void SetTextureRect(const Rectf& textureRect);

			SlicedSprite& operator=(const SlicedSprite&) = delete;
			SlicedSprite& operator=(SlicedSprite&&) noexcept = default;

			struct Corner
			{
				Vector2f textureCoords = Vector2f(10.f / 64.f, 10.f / 64.f);
				Vector2f size = Vector2f(10.f, 10.f);
			};

		private:
			void UpdateVertices();

			std::array<VertexStruct_XYZ_Color_UV, 4 * 9> m_vertices;
			std::shared_ptr<MaterialInstance> m_material;
			std::size_t m_spriteCount;
			Color m_color;
			Corner m_topLeftCorner;
			Corner m_bottomRightCorner;
			Rectf m_textureCoords;
			Vector2f m_origin;
			Vector2f m_size;
	};
}

#include <Nazara/Graphics/SlicedSprite.inl>

#endif // NAZARA_GRAPHICS_SLICEDSPRITE_HPP
