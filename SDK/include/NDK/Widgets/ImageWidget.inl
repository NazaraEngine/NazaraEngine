// Copyright (C) 2017 Samy Bensaid
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NDK/Widgets/ImageWidget.hpp>

namespace Ndk
{
	inline const Nz::Color& ImageWidget::GetColor() const
	{
		return m_sprite->GetColor();
	}

	inline const Nz::TextureRef& ImageWidget::GetTexture() const
	{
		return m_sprite->GetMaterial()->GetDiffuseMap();
	}

	inline const Nz::Rectf& ImageWidget::GetTextureCoords() const
	{
		return m_sprite->GetTextureCoords();
	}

	inline void ImageWidget::SetColor(const Nz::Color& color)
	{
		m_sprite->SetColor(color);
	}

	inline void ImageWidget::SetTexture(const Nz::TextureRef& texture, bool resizeToContent)
	{
		m_sprite->SetTexture(texture, false);

		if (resizeToContent)
			ResizeToContent();
	}

	inline void ImageWidget::SetTextureCoords(const Nz::Rectf& coords)
	{
		m_sprite->SetTextureCoords(coords);
	}

	inline void ImageWidget::SetTextureRect(const Nz::Rectui& rect)
	{
		m_sprite->SetTextureRect(rect);
	}
}
