// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <cassert>

namespace Nz
{
	inline const Color& Billboard::GetColor() const
	{
		return m_color;
	}

	inline const Color& Billboard::GetCornerColor(RectCorner corner) const
	{
		return m_cornerColor[corner];
	}

	inline RadianAnglef Billboard::GetRotation() const
	{
		return m_rotation;
	}

	inline const Rectf& Billboard::GetTextureCoords() const
	{
		return m_textureCoords;
	}

	inline const Vector2f& Billboard::GetSize() const
	{
		return m_size;
	}

	inline void Billboard::SetColor(const Color& color)
	{
		m_color = color;

		UpdateVertices();
	}

	inline void Billboard::SetCornerColor(RectCorner corner, const Color& color)
	{
		m_cornerColor[corner] = color;

		UpdateVertices();
	}

	inline void Billboard::SetMaterial(std::shared_ptr<MaterialInstance> material)
	{
		assert(material);

		if (m_material != material)
		{
			OnMaterialInvalidated(this, 0, material);
			m_material = std::move(material);

			OnElementInvalidated(this);
		}
	}

	inline void Billboard::SetRotation(RadianAnglef rotation)
	{
		m_rotation = rotation;

		UpdateVertices();
	}

	inline void Billboard::SetSize(const Vector2f& size)
	{
		m_size = size;

		UpdateVertices();
	}

	inline void Billboard::SetTextureCoords(const Rectf& textureCoords)
	{
		m_textureCoords = textureCoords;

		UpdateVertices();
	}

	inline void Billboard::SetTextureRect(const Rectf& textureRect)
	{
		Vector2f invTextureSize = 1.f / Vector2f(Vector2ui(GetTextureSize()));
		return SetTextureCoords(Rectf(textureRect.x * invTextureSize.x, textureRect.y * invTextureSize.y, textureRect.width * invTextureSize.x, textureRect.height * invTextureSize.y));
	}

	inline void Billboard::UpdateVertices()
	{
		VertexStruct_UV_SizeSinCos_Color* vertices = m_vertices.data();

		EnumArray<RectCorner, Vector2f> cornerExtent;
		cornerExtent[RectCorner::LeftBottom]  = Vector2f(0.f, 0.f);
		cornerExtent[RectCorner::RightBottom] = Vector2f(1.f, 0.f);
		cornerExtent[RectCorner::LeftTop]     = Vector2f(0.f, 1.f);
		cornerExtent[RectCorner::RightTop]    = Vector2f(1.f, 1.f);

		auto [sin, cos] = m_rotation.GetSinCos();

		for (RectCorner corner : { RectCorner::LeftBottom, RectCorner::RightBottom, RectCorner::LeftTop, RectCorner::RightTop })
		{
			vertices->color = m_color * m_cornerColor[corner];
			vertices->sizeSinCos = Vector4f(m_size.x, m_size.y, sin, cos);
			vertices->uv = m_textureCoords.GetCorner<CoordinateSystem::UV>(corner);

			vertices++;
		}

		Vector2f halfSize = m_size * 0.5f;
		float maxExtent = std::max(halfSize.x, halfSize.y);
		UpdateAABB(Boxf(-maxExtent, -maxExtent, -maxExtent, maxExtent * 2.0f, maxExtent * 2.0f, maxExtent * 2.0f));
		OnElementInvalidated(this);
	}
}
