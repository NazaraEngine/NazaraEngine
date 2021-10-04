// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SPRITE_HPP
#define NAZARA_SPRITE_HPP

#include <Nazara/Prerequisites.hpp>
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
			Sprite(std::shared_ptr<Material> material);
			Sprite(const Sprite&) = delete;
			Sprite(Sprite&&) noexcept = default;
			~Sprite() = default;

			void BuildElement(std::size_t passIndex, const WorldInstance& worldInstance, std::vector<std::unique_ptr<RenderElement>>& elements) const override;

			inline const Color& GetColor() const;
			inline const Color& GetCornerColor(RectCorner corner) const;
			const std::shared_ptr<Material>& GetMaterial(std::size_t i) const;
			std::size_t GetMaterialCount() const;

			inline void SetColor(const Color& color);
			inline void SetCornerColor(RectCorner corner, const Color& color);
			inline void SetMaterial(std::shared_ptr<Material> material);
			inline void SetSize(const Vector2f& size);

			Sprite& operator=(const Sprite&) = delete;
			Sprite& operator=(Sprite&&) noexcept = default;

		private:
			inline void UpdateVertices();

			std::array<Color, RectCornerCount> m_cornerColor;
			std::array<VertexStruct_XYZ_Color_UV, 4> m_vertices;
			std::shared_ptr<Material> m_material;
			Color m_color;
			Rectf m_textureCoords;
			Vector2f m_size;
	};
}

#include <Nazara/Graphics/Sprite.inl>

#endif
