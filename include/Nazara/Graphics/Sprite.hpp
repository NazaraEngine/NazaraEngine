// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_SPRITE_HPP
#define NAZARA_GRAPHICS_SPRITE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Renderer/RenderPipeline.hpp>
#include <Nazara/Utility/VertexDeclaration.hpp>
#include <Nazara/Utility/VertexStruct.hpp>
#include <array>

namespace Nz
{
	class NAZARA_GRAPHICS_API Sprite : public InstancedRenderable
	{
		public:
			Sprite(std::shared_ptr<MaterialInstance> material);
			Sprite(const Sprite&) = delete;
			Sprite(Sprite&&) noexcept = default;
			~Sprite() = default;

			void BuildElement(ElementRendererRegistry& registry, const ElementData& elementData, std::size_t passIndex, std::vector<RenderElementOwner>& elements) const override;

			inline const Color& GetColor() const;
			inline const Color& GetCornerColor(RectCorner corner) const;
			const std::shared_ptr<MaterialInstance>& GetMaterial(std::size_t i = 0) const override;
			std::size_t GetMaterialCount() const override;
			inline const Vector2f& GetOrigin() const;
			inline const Vector2f& GetSize() const;
			inline const Rectf& GetTextureCoords() const;
			Vector3ui GetTextureSize() const;

			inline void SetColor(const Color& color);
			inline void SetCornerColor(RectCorner corner, const Color& color);
			inline void SetMaterial(std::shared_ptr<MaterialInstance> material);
			inline void SetOrigin(const Vector2f& origin);
			inline void SetSize(const Vector2f& size);
			inline void SetTextureCoords(const Rectf& textureCoords);
			inline void SetTextureRect(const Rectf& textureRect);

			Sprite& operator=(const Sprite&) = delete;
			Sprite& operator=(Sprite&&) noexcept = default;

		private:
			inline void UpdateVertices();

			EnumArray<RectCorner, Color> m_cornerColor;
			std::array<VertexStruct_XYZ_Color_UV, 4> m_vertices;
			std::shared_ptr<MaterialInstance> m_material;
			Color m_color;
			Rectf m_textureCoords;
			Vector2f m_origin;
			Vector2f m_size;
	};
}

#include <Nazara/Graphics/Sprite.inl>

#endif // NAZARA_GRAPHICS_SPRITE_HPP
