// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NDK/Widgets/TabBarWidget.hpp>

namespace Ndk
{
	inline void TabBarWidget::DisableAutoHide()
	{
		return EnableAutoHide(false);
	}

	inline bool TabBarWidget::IsAutoHideEnabled() const
	{
		return m_shouldAutoHide;
	}

	inline bool Ndk::TabBarWidget::ShouldHide() const
	{
		return m_shouldAutoHide && m_tabs.size() <= 1;
	}
}
