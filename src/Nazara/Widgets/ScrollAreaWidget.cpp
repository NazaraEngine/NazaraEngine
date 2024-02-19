// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Widgets/ScrollAreaWidget.hpp>
#include <Nazara/Widgets/ScrollbarWidget.hpp>

namespace Nz
{
	ScrollAreaWidget::ScrollAreaWidget(BaseWidget* parent, BaseWidget* content) :
	BaseWidget(parent),
	m_content(content),
	m_isScrollbarEnabled(true),
	m_hasScrollbar(false)
	{
		AddChild(m_content->ReleaseFromParent());
		m_content->SetPosition(Nz::Vector3f::Zero());

		//m_style = GetTheme()->CreateStyle(this);
		//SetRenderLayerCount(m_style->GetRenderLayerCount());

		m_horizontalScrollbar = Add<ScrollbarWidget>(ScrollbarOrientation::Vertical);
		m_horizontalScrollbar->OnScrollbarValueUpdate.Connect([this](ScrollbarWidget*, float newValue)
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

			bool isVisible = IsScrollbarVisible();
			m_horizontalScrollbar->Show(isVisible);
		}
	}

	float ScrollAreaWidget::GetScrollHeight() const
	{
		return m_horizontalScrollbar->GetValue() * m_content->GetHeight();
	}

	float ScrollAreaWidget::GetScrollRatio() const
	{
		return m_horizontalScrollbar->GetValue();
	}

	void ScrollAreaWidget::ScrollToRatio(float ratio)
	{
		m_horizontalScrollbar->SetValue(ratio);
	}

	void ScrollAreaWidget::Layout()
	{
		float scrollBarWidth = m_horizontalScrollbar->GetPreferredWidth();

		float areaWidth = GetWidth();
		float areaHeight = GetHeight();

		m_content->Resize({ areaWidth, areaHeight }); //< setting width with line wrap adjust preferred height
		float contentHeight = m_content->GetPreferredHeight();

		if (contentHeight > areaHeight)
		{
			m_hasScrollbar = true;

			Nz::Vector2f contentSize(areaWidth - scrollBarWidth, contentHeight);
			m_content->Resize(contentSize);

			if (m_isScrollbarEnabled)
				m_horizontalScrollbar->Show();

			m_horizontalScrollbar->SetPosition({ contentSize.x, 0.f, 0.f });
			m_horizontalScrollbar->Resize({ scrollBarWidth, GetHeight() });

			ScrollToRatio(m_horizontalScrollbar->GetValue());
		}
		else
		{
			m_hasScrollbar = false;

			m_content->Resize(GetSize());

			m_horizontalScrollbar->Hide();

			ScrollToRatio(0.f);
		}

		BaseWidget::Layout();
	}

	bool ScrollAreaWidget::OnMouseWheelMoved(int /*x*/, int /*y*/, float delta)
	{
		constexpr float scrollStep = 100.f;

		ScrollToHeight(GetScrollHeight() - scrollStep * delta);
		return true;
	}
}
