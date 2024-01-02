// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Widgets/Debug.hpp>

namespace Nz
{
	inline unsigned int RichTextAreaWidget::GetCharacterSize() const
	{
		return m_drawer.GetCharacterSize();
	}

	inline float RichTextAreaWidget::GetCharacterSpacingOffset() const
	{
		return m_drawer.GetCharacterSpacingOffset();
	}

	inline float RichTextAreaWidget::GetLineSpacingOffset() const
	{
		return m_drawer.GetLineSpacingOffset();
	}

	inline const Color& RichTextAreaWidget::GetTextColor() const
	{
		return m_drawer.GetTextColor();
	}

	inline const std::shared_ptr<Font>& RichTextAreaWidget::GetTextFont() const
	{
		return m_drawer.GetTextFont();
	}

	inline const Color& RichTextAreaWidget::GetTextOutlineColor() const
	{
		return m_drawer.GetTextOutlineColor();
	}

	inline float RichTextAreaWidget::GetTextOutlineThickness() const
	{
		return m_drawer.GetTextOutlineThickness();
	}

	inline TextStyleFlags RichTextAreaWidget::GetTextStyle() const
	{
		return m_drawer.GetTextStyle();
	}

	inline void RichTextAreaWidget::SetCharacterSize(unsigned int characterSize)
	{
		m_drawer.SetCharacterSize(characterSize);
	}

	inline void RichTextAreaWidget::SetCharacterSpacingOffset(float offset)
	{
		m_drawer.SetCharacterSpacingOffset(offset);
	}

	inline void RichTextAreaWidget::SetLineSpacingOffset(float offset)
	{
		m_drawer.SetLineSpacingOffset(offset);
	}

	inline void RichTextAreaWidget::SetTextColor(const Color& color)
	{
		m_drawer.SetTextColor(color);
	}

	inline void RichTextAreaWidget::SetTextFont(std::shared_ptr<Font> font)
	{
		m_drawer.SetTextFont(std::move(font));
	}

	inline void RichTextAreaWidget::SetTextOutlineColor(const Color& color)
	{
		m_drawer.SetTextOutlineColor(color);
	}

	inline void RichTextAreaWidget::SetTextOutlineThickness(float thickness)
	{
		m_drawer.SetTextOutlineThickness(thickness);
	}

	inline void RichTextAreaWidget::SetTextStyle(TextStyleFlags style)
	{
		m_drawer.SetTextStyle(style);
	}
}

#include <Nazara/Widgets/DebugOff.hpp>
