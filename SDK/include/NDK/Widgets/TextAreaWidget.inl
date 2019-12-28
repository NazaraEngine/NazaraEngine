// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NDK/Widgets/TextAreaWidget.hpp>

namespace Ndk
{
	inline unsigned int TextAreaWidget::GetCharacterSize() const
	{
		return m_drawer.GetCharacterSize();
	}

	inline const Nz::String& TextAreaWidget::GetDisplayText() const
	{
		return m_drawer.GetText();
	}

	inline const Nz::String& TextAreaWidget::GetText() const
	{
		return m_text;
	}

	inline const Nz::Color& TextAreaWidget::GetTextColor() const
	{
		return m_drawer.GetColor();
	}

	inline Nz::Font* TextAreaWidget::GetTextFont() const
	{
		return m_drawer.GetFont();
	}

	inline const Nz::Color& TextAreaWidget::GetTextOulineColor() const
	{
		return m_drawer.GetOutlineColor();
	}

	inline float TextAreaWidget::GetTextOulineThickness() const
	{
		return m_drawer.GetOutlineThickness();
	}

	inline Nz::TextStyleFlags TextAreaWidget::GetTextStyle() const
	{
		return m_drawer.GetStyle();
	}

	inline void TextAreaWidget::SetText(const Nz::String& text)
	{
		m_text = text;
		OnTextChanged(this, m_text);

		UpdateDisplayText();
	}

	inline void TextAreaWidget::SetTextColor(const Nz::Color& text)
	{
		m_drawer.SetColor(text);

		UpdateDisplayText();
	}

	inline void TextAreaWidget::SetTextFont(Nz::FontRef font)
	{
		m_drawer.SetFont(font);

		UpdateDisplayText();
	}

	inline void TextAreaWidget::SetTextOutlineColor(const Nz::Color& color)
	{
		m_drawer.SetOutlineColor(color);

		UpdateDisplayText();
	}

	inline void TextAreaWidget::SetTextOutlineThickness(float thickness)
	{
		m_drawer.SetOutlineThickness(thickness);

		UpdateDisplayText();
	}

	inline void TextAreaWidget::SetTextStyle(Nz::TextStyleFlags style)
	{
		m_drawer.SetStyle(style);

		UpdateDisplayText();
	}
}
