// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Sprite.hpp>
#include <cassert>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline void Sprite::SetColor(const Color& color)
	{
		m_color = color;

		UpdateVertices();
	}

	inline void Sprite::SetCornerColor(RectCorner corner, const Color& color)
	{
		m_cornerColor[UnderlyingCast(corner)] = color;

		UpdateVertices();
	}

	inline void Sprite::SetMaterial(std::shared_ptr<Material> material)
	{
		m_material = std::move(material);
	}

	inline void Sprite::SetSize(const Vector2f& size)
	{
		m_size = size;

		UpdateVertices();
	}

	inline void Sprite::UpdateVertices()
	{
		VertexStruct_XYZ_Color_UV* vertices = m_vertices.data();

		Vector3f origin = Vector3f::Zero();

		vertices->color = m_color * m_cornerColor[UnderlyingCast(RectCorner::LeftTop)];
		vertices->position = Vector3f(-origin);
		vertices->uv = m_textureCoords.GetCorner(RectCorner::LeftTop);
		vertices++;

		vertices->color = m_color * m_cornerColor[UnderlyingCast(RectCorner::RightTop)];
		vertices->position = m_size.x * Vector3f::Right() - origin;
		vertices->uv = m_textureCoords.GetCorner(RectCorner::RightTop);
		vertices++;

		vertices->color = m_color * m_cornerColor[UnderlyingCast(RectCorner::LeftBottom)];
		vertices->position = m_size.y * Vector3f::Down() - origin;
		vertices->uv = m_textureCoords.GetCorner(RectCorner::LeftBottom);
		vertices++;

		vertices->color = m_color * m_cornerColor[UnderlyingCast(RectCorner::RightBottom)];
		vertices->position = m_size.x * Vector3f::Right() + m_size.y * Vector3f::Down() - origin;
		vertices->uv = m_textureCoords.GetCorner(RectCorner::RightBottom);
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
