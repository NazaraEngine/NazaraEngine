// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <cassert>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline const Color& SlicedSprite::GetColor() const
	{
		return m_color;
	}

	inline auto SlicedSprite::GetBottomRightCorner() const -> const Corner&
	{
		return m_bottomRightCorner;
	}

	inline const Rectf& SlicedSprite::GetTextureCoords() const
	{
		return m_textureCoords;
	}

	inline const Vector2f& SlicedSprite::GetOrigin() const
	{
		return m_origin;
	}

	inline const Vector2f& SlicedSprite::GetSize() const
	{
		return m_size;
	}

	inline auto SlicedSprite::GetTopLeftCorner() const -> const Corner&
	{
		return m_topLeftCorner;
	}

	inline void SlicedSprite::SetColor(const Color& color)
	{
		m_color = color;

		UpdateVertices();
	}

	inline void SlicedSprite::SetCorners(const Corner& topLeftCorner, const Corner& bottomRightCorner)
	{
		m_topLeftCorner = topLeftCorner;
		m_bottomRightCorner = bottomRightCorner;

		UpdateVertices();
	}

	inline void SlicedSprite::SetCornersSize(const Vector2f& topLeftSize, const Vector2f& bottomRightSize)
	{
		m_topLeftCorner.size = topLeftSize;
		m_bottomRightCorner.size = bottomRightSize;

		UpdateVertices();
	}

	inline void SlicedSprite::SetCornersTextureCoords(const Vector2f& topLeftTextureCoords, const Vector2f& bottomRightTextureCoords)
	{
		m_topLeftCorner.textureCoords = topLeftTextureCoords;
		m_bottomRightCorner.textureCoords = bottomRightTextureCoords;

		UpdateVertices();
	}

	inline void SlicedSprite::SetMaterial(std::shared_ptr<MaterialInstance> material)
	{
		assert(material);

		if (m_material != material)
		{
			OnMaterialInvalidated(this, 0, material);
			m_material = std::move(material);

			OnElementInvalidated(this);
		}
	}

	inline void SlicedSprite::SetOrigin(const Vector2f& origin)
	{
		m_origin = origin;

		UpdateVertices();
	}

	inline void SlicedSprite::SetSize(const Vector2f& size)
	{
		m_size = size;

		UpdateVertices();
	}

	inline void SlicedSprite::SetTextureCoords(const Rectf& textureCoords)
	{
		m_textureCoords = textureCoords;

		UpdateVertices();
	}

	inline void SlicedSprite::SetTextureRect(const Rectf& textureRect)
	{
		Vector2f invTextureSize = 1.f / Vector2f(Vector2ui(GetTextureSize()));
		return SetTextureCoords(Rectf(textureRect.x * invTextureSize.x, textureRect.y * invTextureSize.y, textureRect.width * invTextureSize.x, textureRect.height * invTextureSize.y));
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
