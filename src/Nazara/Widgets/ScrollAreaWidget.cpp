// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Widgets/ScrollAreaWidget.hpp>
#include <Nazara/Widgets/ScrollbarWidget.hpp>

namespace Nz
{
	ScrollAreaWidget::ScrollAreaWidget(BaseWidget* parent, BaseWidget* content, const StyleFactory& styleFactory) :
	BaseWidget(parent),
	m_content(content),
	m_isPerformingLayout(false),
	m_isScrollbarEnabled(true),
	m_hasScrollbar(false)
	{
		AddChild(m_content->ReleaseFromParent());
		m_content->SetPosition(Nz::Vector3f::Zero());

		//m_style = (styleFactory) ? styleFactory(this) : GetTheme()->CreateStyle(this);
		//SetRenderLayerCount(m_style->GetRenderLayerCount());

		m_verticalScrollbar = Add<ScrollbarWidget>(ScrollbarOrientation::Vertical);
		m_verticalScrollbar->OnScrollbarValueUpdate.Connect([this](ScrollbarWidget*, float newValue)
		{
			float contentPosition = (GetHeight() - m_content->GetHeight()) * (1.f - newValue);

			m_content->SetPosition({ 0.f, contentPosition });
			m_content->SetRenderingRect(Nz::Rectf(-std::numeric_limits<float>::infinity(), -contentPosition, std::numeric_limits<float>::infinity(), GetHeight()));
		});

		Resize(m_content->GetSize()); //< will automatically layout
	}

	void ScrollAreaWidget::EnableScrollbar(bool enable)
	{
		if (m_isScrollbarEnabled != enable)
		{
			m_isScrollbarEnabled = enable;

			Layout();
		}
	}

	float ScrollAreaWidget::GetScrollHeight() const
	{
		return m_verticalScrollbar->GetValue() * m_content->GetHeight();
	}

	float ScrollAreaWidget::GetScrollRatio() const
	{
		return m_verticalScrollbar->GetValue();
	}

	void ScrollAreaWidget::ScrollToRatio(float ratio)
	{
		m_verticalScrollbar->SetValue(ratio);
	}

	void ScrollAreaWidget::Layout()
	{
		// Prevent infinite recursion
		m_isPerformingLayout = true;

		float scrollBarWidth = m_verticalScrollbar->GetPreferredWidth();

		float areaWidth = GetWidth();
		float areaHeight = GetHeight();

		m_content->Resize({ areaWidth, areaHeight }); //< setting width with line wrap adjust preferred height
		float contentHeight = m_content->GetPreferredHeight();

		float scrollRatio = m_verticalScrollbar->GetValue();
		if (contentHeight > areaHeight)
		{
			m_hasScrollbar = true;

			if (m_isScrollbarEnabled)
			{
				m_verticalScrollbar->Show();
				areaWidth -= scrollBarWidth;

				m_verticalScrollbar->SetPosition({ areaWidth, 0.f, 0.f });
				m_verticalScrollbar->Resize({ scrollBarWidth, GetHeight() });
			}
		}
		else
		{
			m_hasScrollbar = false;

			m_verticalScrollbar->Hide();

			scrollRatio = 0.f;
		}

		m_content->Resize({ areaWidth, std::max(areaHeight, contentHeight) });
		ScrollToRatio(scrollRatio);

		BaseWidget::Layout();

		m_isPerformingLayout = false;
	}

	void ScrollAreaWidget::OnChildPreferredSizeUpdated(const BaseWidget* child)
	{
		if (child == m_content && !m_isPerformingLayout)
			Layout();
	}

	bool ScrollAreaWidget::OnMouseWheelMoved(int /*x*/, int /*y*/, float delta)
	{
		constexpr float scrollStep = 100.f;

		ScrollToHeight(GetScrollHeight() - scrollStep * delta);
		return true;
	}

	void ScrollAreaWidget::OnVisibilityUpdated(bool isVisible)
	{
		// Fix scrollbar getting visible when showing the scroll area widget
		// FIXME: Handle this in a better and transparent way
		if (isVisible && !IsScrollbarVisible())
			m_verticalScrollbar->Hide();
	}
}
