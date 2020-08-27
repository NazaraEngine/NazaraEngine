// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NazaraSDK/Widgets/ScrollAreaWidget.hpp>

namespace Ndk
{
	inline float ScrollAreaWidget::GetScrollHeight() const
	{
		return m_scrollRatio * m_content->GetHeight();
	}

	inline float ScrollAreaWidget::GetScrollRatio() const
	{
		return m_scrollRatio;
	}

	inline bool ScrollAreaWidget::HasScrollbar() const
	{
		return m_hasScrollbar;
	}

	inline bool ScrollAreaWidget::IsScrollbarEnabled() const
	{
		return m_isScrollbarEnabled;
	}

	inline bool ScrollAreaWidget::IsScrollbarVisible() const
	{
		return HasScrollbar() && IsScrollbarEnabled();
	}

	inline void ScrollAreaWidget::ScrollToHeight(float height)
	{
		float contentHeight = m_content->GetHeight();
		ScrollToRatio(height / contentHeight);
	}
}
