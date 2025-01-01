// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Widgets/ButtonWidget.hpp>
#include <Nazara/TextRenderer/AbstractTextDrawer.hpp>

namespace Nz
{
	ButtonWidget::ButtonWidget(BaseWidget* parent, const StyleFactory& styleFactory) :
	BaseWidget(parent),
	m_isEnabled(true)
	{
		m_style = (styleFactory) ? styleFactory(this) : GetTheme()->CreateStyle(this);
		SetRenderLayerCount(m_style->GetRenderLayerCount());

		Layout();
	}

	void ButtonWidget::Enable(bool enable)
	{
		if (m_isEnabled != enable)
		{
			if (enable)
			{
				m_style->OnEnabled();
				EnableMouseInput(true);
			}
			else
			{
				m_style->OnDisabled();
				EnableMouseInput(false);
			}

			m_isEnabled = enable;
		}
	}

	void ButtonWidget::UpdateText(const AbstractTextDrawer& drawer)
	{
		m_style->UpdateText(drawer);

		Vector2f size(drawer.GetBounds().GetLengths());
		SetMinimumSize(size);
		SetPreferredSize(size + Vector2f(20.f, 10.f)); //< corner size (TODO: Retrieve them from theme)

		Layout();
	}

	void ButtonWidget::Layout()
	{
		BaseWidget::Layout();
		m_style->Layout(GetSize());
	}

	bool ButtonWidget::OnMouseButtonPress(int /*x*/, int /*y*/, Mouse::Button button)
	{
		if (button == Mouse::Left)
		{
			m_style->OnPress();
			return true;
		}

		return false;
	}

	bool ButtonWidget::OnMouseButtonRelease(int x, int y, Mouse::Button button)
	{
		if (button == Mouse::Left)
		{
			m_style->OnRelease();

			// If user clicks inside button and holds it outside, a release mouse button event will be triggered outside of the widget
			// we don't want this to trigger the button, so double-check
			if (m_isEnabled && IsInside(float(x), float(y)))
				OnButtonTrigger(this);

			return true;
		}

		return false;
	}

	void ButtonWidget::OnMouseEnter()
	{
		m_style->OnHoverBegin();
	}

	void ButtonWidget::OnMouseExit()
	{
		m_style->OnHoverEnd();
	}

	void ButtonWidget::OnRenderLayerUpdated(int baseRenderLayer)
	{
		m_style->UpdateRenderLayer(baseRenderLayer);
	}
}
