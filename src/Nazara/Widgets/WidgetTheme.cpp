// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Widgets/WidgetTheme.hpp>
#include <Nazara/Widgets/Canvas.hpp>
#include <Nazara/Widgets/Debug.hpp>

namespace Nz
{
	WidgetTheme::~WidgetTheme() = default;

	BaseWidgetStyle::~BaseWidgetStyle() = default;

	UInt32 BaseWidgetStyle::GetRenderMask() const
	{
		return m_widgetOwner->GetCanvas()->GetRenderMask();
	}

	void ButtonWidgetStyle::OnHoverBegin()
	{
	}

	void ButtonWidgetStyle::OnHoverEnd()
	{
	}

	void ButtonWidgetStyle::OnPress()
	{
	}

	void ButtonWidgetStyle::OnRelease()
	{
	}
}
