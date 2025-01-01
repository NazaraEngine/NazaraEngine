// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <cassert>

namespace Nz
{
	inline const Color& Sprite::GetColor() const
	{
		return m_color;
	}

	inline const Color& Sprite::GetCornerColor(RectCorner corner) const
	{
		return m_cornerColor[corner];
	}

	inline const Vector2f& Sprite::GetOrigin() const
	{
		return m_origin;
	}

	inline const Vector2f& Sprite::GetSize() const
	{
		return m_size;
	}

	inline const Rectf& Sprite::GetTextureCoords() const
	{
		return m_textureCoords;
	}

	inline void Sprite::SetColor(const Color& color)
	{
		m_color = color;

		UpdateVertices();
	}

	inline void Sprite::SetCornerColor(RectCorner corner, const Color& color)
	{
		m_cornerColor[corner] = color;

		UpdateVertices();
	}

	inline void Sprite::SetMaterial(std::shared_ptr<MaterialInstance> material)
	{
		assert(material);

		if (m_material != material)
		{
			OnMaterialInvalidated(this, 0, material);
			m_material = std::move(material);

			OnElementInvalidated(this);
		}
	}

	inline void Sprite::SetOrigin(const Vector2f& origin)
	{
		m_origin = origin;

		UpdateVertices();
	}

	inline void Sprite::SetSize(const Vector2f& size)
	{
		m_size = size;

		UpdateVertices();
	}

	inline void Sprite::SetTextureCoords(const Rectf& textureCoords)
	{
		m_textureCoords = textureCoords;

		UpdateVertices();
	}

	inline void Sprite::SetTextureRect(const Rectf& textureRect)
	{
		Vector2f invTextureSize = 1.f / Vector2f(Vector2ui(GetTextureSize()));
		return SetTextureCoords(Rectf(textureRect.x * invTextureSize.x, textureRect.y * invTextureSize.y, textureRect.width * invTextureSize.x, textureRect.height * invTextureSize.y));
	}

	inline void Sprite::UpdateVertices()
	{
		VertexStruct_XYZ_Color_UV* vertices = m_vertices.data();

		EnumArray<RectCorner, Vector2f> cornerExtent;
		cornerExtent[RectCorner::LeftBottom]  = Vector2f(0.f, 0.f);
		cornerExtent[RectCorner::RightBottom] = Vector2f(1.f, 0.f);
		cornerExtent[RectCorner::LeftTop]     = Vector2f(0.f, 1.f);
		cornerExtent[RectCorner::RightTop]    = Vector2f(1.f, 1.f);

		Vector3f originShift = m_origin * m_size;

		for (RectCorner corner : { RectCorner::LeftBottom, RectCorner::RightBottom, RectCorner::LeftTop, RectCorner::RightTop })
		{
			vertices->color = m_color * m_cornerColor[corner];
			vertices->position = Vector3f(m_size * cornerExtent[corner], 0.f) - originShift;
			vertices->uv = m_textureCoords.GetCorner<CoordinateSystem::UV>(corner);

			vertices++;
		}

		UpdateAABB(Rectf(-originShift.x, -originShift.y, m_size.x, m_size.y));
		OnElementInvalidated(this);
	}
}
