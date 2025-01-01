// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_BILLBOARD_HPP
#define NAZARA_GRAPHICS_BILLBOARD_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/VertexDeclaration.hpp>
#include <Nazara/Core/VertexStruct.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Renderer/RenderPipeline.hpp>
#include <array>

namespace Nz
{
	class NAZARA_GRAPHICS_API Billboard : public InstancedRenderable
	{
		public:
			Billboard(std::shared_ptr<MaterialInstance> material);
			Billboard(std::shared_ptr<MaterialInstance> material, const Vector2f& size);
			Billboard(const Billboard&) = delete;
			Billboard(Billboard&&) noexcept = default;
			~Billboard() = default;

			void BuildElement(ElementRendererRegistry& registry, const ElementData& elementData, std::size_t passIndex, std::vector<RenderElementOwner>& elements) const override;

			inline const Color& GetColor() const;
			inline const Color& GetCornerColor(RectCorner corner) const;
			const std::shared_ptr<MaterialInstance>& GetMaterial(std::size_t i = 0) const override;
			std::size_t GetMaterialCount() const override;
			inline RadianAnglef GetRotation() const;
			inline const Vector2f& GetSize() const;
			inline const Rectf& GetTextureCoords() const;
			Vector3ui GetTextureSize() const;

			inline void SetColor(const Color& color);
			inline void SetCornerColor(RectCorner corner, const Color& color);
			inline void SetMaterial(std::shared_ptr<MaterialInstance> material);
			inline void SetRotation(RadianAnglef rotation);
			inline void SetSize(const Vector2f& size);
			inline void SetTextureCoords(const Rectf& textureCoords);
			inline void SetTextureRect(const Rectf& textureRect);

			Billboard& operator=(const Billboard&) = delete;
			Billboard& operator=(Billboard&&) noexcept = default;

		private:
			inline void UpdateVertices();

			std::array<VertexStruct_UV_SizeSinCos_Color, 4> m_vertices;
			std::shared_ptr<MaterialInstance> m_material;
			Color m_color;
			EnumArray<RectCorner, Color> m_cornerColor;
			RadianAnglef m_rotation;
			Rectf m_textureCoords;
			Vector2f m_size;
	};
}

#include <Nazara/Graphics/Billboard.inl>

#endif // NAZARA_GRAPHICS_BILLBOARD_HPP
