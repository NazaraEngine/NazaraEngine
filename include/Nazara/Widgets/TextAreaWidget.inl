// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Widgets/TextAreaWidget.hpp>
#include <Nazara/Widgets/Debug.hpp>

namespace Nz
{
	inline unsigned int TextAreaWidget::GetCharacterSize() const
	{
		return m_drawer.GetCharacterSize();
	}

	inline const std::string& TextAreaWidget::GetDisplayText() const
	{
		return m_drawer.GetText();
	}

	inline float TextAreaWidget::GetCharacterSpacingOffset() const
	{
		return m_drawer.GetCharacterSpacingOffset();
	}

	inline float TextAreaWidget::GetLineSpacingOffset() const
	{
		return m_drawer.GetLineSpacingOffset();
	}

	inline const std::string& TextAreaWidget::GetText() const
	{
		return m_text;
	}

	inline const Color& TextAreaWidget::GetTextColor() const
	{
		return m_drawer.GetColor();
	}

	inline const std::shared_ptr<Font>& TextAreaWidget::GetTextFont() const
	{
		return m_drawer.GetFont();
	}

	inline const Color& TextAreaWidget::GetTextOulineColor() const
	{
		return m_drawer.GetOutlineColor();
	}

	inline float TextAreaWidget::GetTextOulineThickness() const
	{
		return m_drawer.GetOutlineThickness();
	}

	inline TextStyleFlags TextAreaWidget::GetTextStyle() const
	{
		return m_drawer.GetStyle();
	}

	inline void TextAreaWidget::SetCharacterSize(unsigned int characterSize)
	{
		m_drawer.SetCharacterSize(characterSize);

		UpdateMinimumSize();
		UpdateDisplayText();
	}

	inline void TextAreaWidget::SetCharacterSpacingOffset(float offset)
	{
		m_drawer.SetCharacterSpacingOffset(offset);

		UpdateMinimumSize();
		UpdateDisplayText();
	}

	inline void TextAreaWidget::SetLineSpacingOffset(float offset)
	{
		m_drawer.SetLineSpacingOffset(offset);

		UpdateDisplayText();
	}

	inline void TextAreaWidget::SetText(const std::string& text)
	{
		m_text = text;
		OnTextChanged(this, m_text);

		UpdateDisplayText();
	}

	inline void TextAreaWidget::SetTextColor(const Color& text)
	{
		m_drawer.SetColor(text);

		UpdateDisplayText();
	}

	inline void TextAreaWidget::SetTextFont(std::shared_ptr<Font> font)
	{
		m_drawer.SetFont(std::move(font));

		UpdateDisplayText();
	}

	inline void TextAreaWidget::SetTextOutlineColor(const Color& color)
	{
		m_drawer.SetOutlineColor(color);

		UpdateDisplayText();
	}

	inline void TextAreaWidget::SetTextOutlineThickness(float thickness)
	{
		m_drawer.SetOutlineThickness(thickness);

		UpdateDisplayText();
	}

	inline void TextAreaWidget::SetTextStyle(TextStyleFlags style)
	{
		m_drawer.SetStyle(style);

		UpdateDisplayText();
	}
}

#include <Nazara/Widgets/DebugOff.hpp>
