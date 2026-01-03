// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Widgets/ScrollbarButtonWidget.hpp>

namespace Nz
{
	ScrollbarButtonWidget::ScrollbarButtonWidget(BaseWidget* parent, const StyleFactory& styleFactory) :
	BaseWidget(parent),
	m_isGrabbed(false)
	{
		m_style = (styleFactory) ? styleFactory(this) : GetTheme()->CreateStyle(this);
		SetRenderLayerCount(m_style->GetRenderLayerCount());

		Layout();
	}

	void ScrollbarButtonWidget::Layout()
	{
		BaseWidget::Layout();
		m_style->Layout(GetSize());
	}

	bool ScrollbarButtonWidget::OnMouseButtonPress(int x, int y, Mouse::Button button)
	{
		if (button == Mouse::Left)
		{
			m_isGrabbed = true;
			OnButtonGrabbed(this, x, y);

			m_style->OnGrab();
			return true;
		}

		return false;
	}

	bool ScrollbarButtonWidget::OnMouseButtonRelease(int /*x*/, int /*y*/, Mouse::Button button)
	{
		if (button == Mouse::Left)
		{
			m_isGrabbed = false;
			OnButtonReleased(this);

			m_style->OnRelease();
			return true;
		}

		return false;
	}

	void ScrollbarButtonWidget::OnMouseEnter()
	{
		m_style->OnHoverBegin();
	}

	void ScrollbarButtonWidget::OnMouseExit()
	{
		m_style->OnHoverEnd();
	}

	bool ScrollbarButtonWidget::OnMouseMoved(int x, int y, int /*deltaX*/, int /*deltaY*/)
	{
		if (m_isGrabbed)
		{
			OnButtonMoved(this, x, y);
			return true;
		}

		return false;
	}

	void ScrollbarButtonWidget::OnRenderLayerUpdated(int baseRenderLayer)
	{
		m_style->UpdateRenderLayer(baseRenderLayer);
	}
}
