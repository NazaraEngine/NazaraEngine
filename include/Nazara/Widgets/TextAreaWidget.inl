// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

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
		return m_drawer.GetTextColor();
	}

	inline const std::shared_ptr<Font>& TextAreaWidget::GetTextFont() const
	{
		return m_drawer.GetTextFont();
	}

	inline const Color& TextAreaWidget::GetTextOulineColor() const
	{
		return m_drawer.GetTextOutlineColor();
	}

	inline float TextAreaWidget::GetTextOulineThickness() const
	{
		return m_drawer.GetTextOutlineThickness();
	}

	inline TextStyleFlags TextAreaWidget::GetTextStyle() const
	{
		return m_drawer.GetTextStyle();
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

	inline void TextAreaWidget::SetText(std::string text)
	{
		m_text = std::move(text);
		OnTextChanged(this, m_text);

		UpdateDisplayText();
	}

	inline void TextAreaWidget::SetTextColor(const Color& text)
	{
		m_drawer.SetTextColor(text);

		UpdateDisplayText();
	}

	inline void TextAreaWidget::SetTextFont(std::shared_ptr<Font> font)
	{
		m_drawer.SetTextFont(std::move(font));

		UpdateDisplayText();
	}

	inline void TextAreaWidget::SetTextOutlineColor(const Color& color)
	{
		m_drawer.SetTextOutlineColor(color);

		UpdateDisplayText();
	}

	inline void TextAreaWidget::SetTextOutlineThickness(float thickness)
	{
		m_drawer.SetTextOutlineThickness(thickness);

		UpdateDisplayText();
	}

	inline void TextAreaWidget::SetTextStyle(TextStyleFlags style)
	{
		m_drawer.SetTextStyle(style);

		UpdateDisplayText();
	}
}

#include <Nazara/Widgets/DebugOff.hpp>
