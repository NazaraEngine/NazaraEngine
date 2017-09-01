// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Widgets/ButtonWidget.hpp>

namespace Ndk
{
	inline const Nz::Color& ButtonWidget::GetColor() const
	{
		return m_color;
	}

	inline const Nz::Color& ButtonWidget::GetCornerColor() const
	{
		return m_cornerColor;
	}

	inline const Nz::Color& ButtonWidget::GetHoverColor() const
	{
		return m_hoverColor;
	}

	inline const Nz::Color& ButtonWidget::GetHoverCornerColor() const
	{
		return m_hoverCornerColor;
	}

	inline const Nz::Color& ButtonWidget::GetPressColor() const
	{
		return m_pressColor;
	}

	inline const Nz::Color& ButtonWidget::GetPressCornerColor() const
	{
		return m_pressCornerColor;
	}

	inline const Nz::TextureRef& ButtonWidget::GetTexture() const
	{
		return m_texture;
	}

	inline const Nz::TextureRef& ButtonWidget::GetHoverTexture() const
	{
		return m_hoverTexture;
	}

	inline const Nz::TextureRef& ButtonWidget::GetPressTexture() const
	{
		return m_pressTexture;
	}

	inline void ButtonWidget::SetColor(const Nz::Color& color, const Nz::Color& cornerColor)
	{
		m_color = color;
		m_cornerColor = cornerColor;

		m_gradientSprite->SetColor(m_color);
		m_gradientSprite->SetCornerColor(Nz::RectCorner_LeftBottom, m_cornerColor);
		m_gradientSprite->SetCornerColor(Nz::RectCorner_RightBottom, m_cornerColor);
	}

	inline void ButtonWidget::SetHoverColor(const Nz::Color& color, const Nz::Color& cornerColor)
	{
		m_hoverColor = color;
		m_hoverCornerColor = cornerColor;
	}

	inline void ButtonWidget::SetPressColor(const Nz::Color& color, const Nz::Color& cornerColor)
	{
		m_pressColor = color;
		m_pressCornerColor = cornerColor;
	}

	inline void ButtonWidget::SetTexture(const Nz::TextureRef& texture, bool resetColors, bool changeHoverTexture)
	{
		m_texture = texture;
		m_gradientSprite->SetTexture(m_texture);

		if (resetColors)
			SetColor(Nz::Color::White, Nz::Color::White);

		if (changeHoverTexture)
			SetHoverTexture(texture, resetColors);
	}

	inline void ButtonWidget::SetHoverTexture(const Nz::TextureRef& texture, bool resetColors)
	{
		m_hoverTexture = texture;

		if (resetColors)
			SetHoverColor(Nz::Color::White, Nz::Color::White);
	}

	inline void ButtonWidget::SetPressTexture(const Nz::TextureRef& texture, bool resetColors)
	{
		m_pressTexture = texture;

		if (resetColors)
			SetPressColor(Nz::Color::White, Nz::Color::White);
	}

	inline void ButtonWidget::UpdateText(const Nz::AbstractTextDrawer& drawer)
	{
		m_textSprite->Update(drawer);

		Layout();
	}
}
