// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Widgets/WidgetTheme.hpp>
#include <Nazara/Widgets/Canvas.hpp>

namespace Nz
{
	WidgetTheme::~WidgetTheme() = default;

	BaseWidgetStyle::~BaseWidgetStyle() = default;

	UInt32 BaseWidgetStyle::GetRenderMask() const
	{
		return m_widgetOwner->GetCanvas()->GetRenderMask();
	}


	void ButtonWidgetStyle::OnDisabled()
	{
	}

	void ButtonWidgetStyle::OnEnabled()
	{
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


	void CheckboxWidgetStyle::OnHoverBegin()
	{
	}

	void CheckboxWidgetStyle::OnHoverEnd()
	{
	}

	void CheckboxWidgetStyle::OnNewState(CheckboxState /*newState*/)
	{
	}

	void CheckboxWidgetStyle::OnPress()
	{
	}

	void CheckboxWidgetStyle::OnRelease()
	{
	}


	void ImageButtonWidgetStyle::OnHoverBegin()
	{
	}

	void ImageButtonWidgetStyle::OnHoverEnd()
	{
	}

	void ImageButtonWidgetStyle::OnPress()
	{
	}

	void ImageButtonWidgetStyle::OnRelease()
	{
	}


	void LabelWidgetStyle::OnHoverBegin()
	{
	}

	void LabelWidgetStyle::OnHoverEnd()
	{
	}


	void ScrollbarWidgetStyle::OnButtonGrab()
	{
	}

	void ScrollbarWidgetStyle::OnButtonRelease()
	{
	}

	void ScrollbarWidgetStyle::OnHoverBegin()
	{
	}

	void ScrollbarWidgetStyle::OnHoverEnd()
	{
	}


	void ScrollbarButtonWidgetStyle::OnHoverBegin()
	{
	}

	void ScrollbarButtonWidgetStyle::OnHoverEnd()
	{
	}

	void ScrollbarButtonWidgetStyle::OnGrab()
	{
	}

	void ScrollbarButtonWidgetStyle::OnRelease()
	{
	}


	void TextAreaWidgetStyle::OnDisabled()
	{
	}

	void TextAreaWidgetStyle::OnEnabled()
	{
	}

	void TextAreaWidgetStyle::OnFocusLost()
	{
	}

	void TextAreaWidgetStyle::OnFocusReceived()
	{
	}

	void TextAreaWidgetStyle::OnHoverBegin()
	{
	}

	void TextAreaWidgetStyle::OnHoverEnd()
	{
	}
}
