// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <Nazara/Widgets/ButtonWidget.hpp>

namespace Nz
{
	inline const Color& ButtonWidget::GetColor() const
	{
		return m_color;
	}

	inline const Color& ButtonWidget::GetCornerColor() const
	{
		return m_cornerColor;
	}

	inline const Color& ButtonWidget::GetHoverColor() const
	{
		return m_hoverColor;
	}

	inline const Color& ButtonWidget::GetHoverCornerColor() const
	{
		return m_hoverCornerColor;
	}

	inline const Color& ButtonWidget::GetPressColor() const
	{
		return m_pressColor;
	}

	inline const Color& ButtonWidget::GetPressCornerColor() const
	{
		return m_pressCornerColor;
	}

	inline void ButtonWidget::SetColor(const Color& color, const Color& cornerColor)
	{
		m_color = color;
		m_cornerColor = cornerColor;

		m_gradientSprite->SetColor(m_color);
		m_gradientSprite->SetCornerColor(Nz::RectCorner::LeftBottom, m_cornerColor);
		m_gradientSprite->SetCornerColor(Nz::RectCorner::RightBottom, m_cornerColor);
	}

	inline void ButtonWidget::SetHoverColor(const Color& color, const Color& cornerColor)
	{
		m_hoverColor = color;
		m_hoverCornerColor = cornerColor;
	}

	inline void ButtonWidget::SetPressColor(const Color& color, const Color& cornerColor)
	{
		m_pressColor = color;
		m_pressCornerColor = cornerColor;
	}

	inline void ButtonWidget::UpdateText(const Nz::AbstractTextDrawer& drawer)
	{
		m_textSprite->Update(drawer);

		Nz::Vector2f textSize = Nz::Vector2f(m_textSprite->GetAABB().GetLengths());
		SetMinimumSize(textSize);
		SetPreferredSize(textSize + Nz::Vector2f(20.f, 10.f));

		Layout();
	}
}
