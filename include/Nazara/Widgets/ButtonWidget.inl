// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Widgets/ButtonWidget.hpp>
#include <Nazara/Widgets/Debug.hpp>

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
		m_gradientSprite->SetCornerColor(RectCorner::LeftBottom, m_cornerColor);
		m_gradientSprite->SetCornerColor(RectCorner::RightBottom, m_cornerColor);
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

	inline void ButtonWidget::UpdateText(const AbstractTextDrawer& drawer)
	{
		m_textSprite->Update(drawer);

		Vector2f textSize = Vector2f(m_textSprite->GetAABB().GetLengths());
		SetMinimumSize(textSize);
		SetPreferredSize(textSize + Vector2f(20.f, 10.f));

		Layout();
	}
}

#include <Nazara/Widgets/DebugOff.hpp>
