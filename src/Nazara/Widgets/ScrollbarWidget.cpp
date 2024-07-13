// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Widgets/ScrollbarWidget.hpp>
#include <Nazara/Widgets/ImageButtonWidget.hpp>
#include <Nazara/Widgets/ScrollbarButtonWidget.hpp>

namespace Nz
{
	ScrollbarWidget::ScrollbarWidget(BaseWidget* parent, ScrollbarOrientation orientation, const StyleFactory& styleFactory) :
	BaseWidget(parent),
	m_orientation(orientation),
	m_maximumValue(1.f),
	m_minimumValue(0.f),
	m_step(0.5f),
	m_value(0.f)
	{
		m_style = (styleFactory) ? styleFactory(this) : GetTheme()->CreateStyle(this);
		SetRenderLayerCount(m_style->GetRenderLayerCount());

		m_scrollCenterButton = Add<ScrollbarButtonWidget>();

		m_scrollCenterButton->OnButtonReleased.Connect([this](const ScrollbarButtonWidget*)
		{
			m_style->OnButtonRelease();
		});

		if (m_orientation == ScrollbarOrientation::Horizontal)
		{
			m_scrollCenterButton->OnButtonGrabbed.Connect([this](const ScrollbarButtonWidget*, int x, int /*y*/)
			{
				m_grabbedPosition = x;
				m_grabbedValue = GetValue();
				m_style->OnButtonGrab();
			});

			m_scrollCenterButton->OnButtonMoved.Connect([this](const ScrollbarButtonWidget*, int x, int /*y*/)
			{
				int deltaX = x - m_grabbedPosition;
				if (deltaX == 0)
					return;

				float scrollbarWidth = m_step * GetWidth();
				float remainingWidth = GetWidth() - m_scrollBackButton->GetWidth() - scrollbarWidth;
				float valueRange = m_maximumValue - m_minimumValue;

				SetValue(m_grabbedValue + deltaX * valueRange / remainingWidth);
			});
		}
		else
		{
			m_scrollCenterButton->OnButtonGrabbed.Connect([this](const ScrollbarButtonWidget* button, int /*x*/, int y)
			{
				m_grabbedPosition = SafeCast<int>(button->GetPosition().y + y);
				m_grabbedValue = GetValue();
				m_style->OnButtonGrab();
			});

			m_scrollCenterButton->OnButtonMoved.Connect([this](const ScrollbarButtonWidget* button, int /*x*/, int y)
			{
				int deltaY = SafeCast<int>(m_grabbedPosition - (button->GetPosition().y + y));
				if (deltaY == 0)
					return;

				float scrollbarHeight = m_step * GetHeight();
				float remainingHeight = GetHeight() - m_scrollBackButton->GetHeight() - scrollbarHeight;
				float valueRange = m_maximumValue - m_minimumValue;

				SetValue(m_grabbedValue + deltaY * valueRange / remainingHeight);
			});
		}

		m_scrollBackButton = AddChild(m_style->CreateBackButton(this, m_orientation));
		m_scrollBackButton->OnButtonTrigger.Connect([this](const ImageButtonWidget*)
		{
			SetValue(GetValue() - 0.1f * (GetMaximumValue() - GetMinimumValue()));
		});

		m_scrollForwardButton = AddChild(m_style->CreateForwardButton(this, m_orientation));
		m_scrollForwardButton->OnButtonTrigger.Connect([this](const ImageButtonWidget*)
		{
			SetValue(GetValue() + 0.1f * (GetMaximumValue() - GetMinimumValue()));
		});

		SetPreferredSize({ 32.f, 32.f });
	}

	void ScrollbarWidget::Layout()
	{
		BaseWidget::Layout();

		float stepPct = m_step / (m_maximumValue - m_minimumValue);
		float valuePct = m_value / (m_maximumValue - m_minimumValue);
		float invValuePct = 1.f - valuePct; //< Remember we're Y up

		Vector2f size = GetSize();
		if (m_orientation == ScrollbarOrientation::Horizontal)
		{
			m_scrollBackButton->Resize({ size.y, size.y });
			m_scrollForwardButton->Resize({ size.y, size.y });
			m_scrollForwardButton->SetPosition({ GetWidth() - m_scrollForwardButton->GetWidth(), 0.f });

			float start = m_scrollBackButton->GetWidth();
			float remaining = size.x - start - m_scrollForwardButton->GetWidth();
			float centerPosition = start + invValuePct * (remaining - remaining * stepPct);

			m_scrollCenterButton->Resize({ remaining * stepPct, size.y });
			m_scrollCenterButton->SetPosition({ start + centerPosition, 0.f });
		}
		else
		{
			m_scrollBackButton->Resize({ size.x, size.x });
			m_scrollBackButton->SetPosition({ 0.f, GetHeight() - m_scrollBackButton->GetHeight() });
			m_scrollForwardButton->Resize({ size.x, size.x });

			float start = m_scrollBackButton->GetHeight();
			float remaining = size.y - start - m_scrollForwardButton->GetHeight();
			float centerPosition = start + invValuePct * (remaining - remaining * stepPct);

			m_scrollCenterButton->Resize({ size.x, remaining * stepPct });
			m_scrollCenterButton->SetPosition({ 0.f, centerPosition });
		}

		m_style->Layout(size);
	}

	void ScrollbarWidget::OnMouseEnter()
	{
		m_style->OnHoverBegin();
	}

	void ScrollbarWidget::OnMouseExit()
	{
		m_style->OnHoverEnd();
	}

	void ScrollbarWidget::OnRenderLayerUpdated(int baseRenderLayer)
	{
		m_style->UpdateRenderLayer(baseRenderLayer);
	}
}
