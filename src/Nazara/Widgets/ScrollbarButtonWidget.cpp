// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Widgets/ScrollbarButtonWidget.hpp>
#include <Nazara/Widgets/Debug.hpp>

namespace Nz
{
	ScrollbarButtonWidget::ScrollbarButtonWidget(BaseWidget* parent) :
	BaseWidget(parent),
	m_isGrabbed(false)
	{
		m_style = GetTheme()->CreateStyle(this);
		SetRenderLayerCount(m_style->GetRenderLayerCount());

		Layout();
	}

	void ScrollbarButtonWidget::Layout()
	{
		BaseWidget::Layout();
		m_style->Layout(GetSize());
	}

	void ScrollbarButtonWidget::OnMouseButtonPress(int x, int y, Mouse::Button button)
	{
		if (button == Mouse::Left)
		{
			m_isGrabbed = true;
			OnButtonGrabbed(this, x, y);

			m_style->OnGrab();
		}
	}

	void ScrollbarButtonWidget::OnMouseButtonRelease(int x, int y, Mouse::Button button)
	{
		if (button == Mouse::Left)
		{
			m_isGrabbed = false;
			OnButtonReleased(this);

			m_style->OnRelease();
		}
	}

	void ScrollbarButtonWidget::OnMouseEnter()
	{
		m_style->OnHoverBegin();
	}

	void ScrollbarButtonWidget::OnMouseExit()
	{
		m_style->OnHoverEnd();
	}

	void ScrollbarButtonWidget::OnMouseMoved(int x, int y, int /*deltaX*/, int /*deltaY*/)
	{
		if (m_isGrabbed)
			OnButtonMoved(this, x, y);
	}

	void ScrollbarButtonWidget::OnRenderLayerUpdated(int baseRenderLayer)
	{
		m_style->UpdateRenderLayer(baseRenderLayer);
	}
}
