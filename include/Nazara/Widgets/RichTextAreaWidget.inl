// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Widgets/Debug.hpp>

namespace Nz
{
	inline unsigned int RichTextAreaWidget::GetCharacterSize() const
	{
		return m_drawer.GetDefaultCharacterSize();
	}

	inline float RichTextAreaWidget::GetCharacterSpacingOffset() const
	{
		return m_drawer.GetDefaultCharacterSpacingOffset();
	}

	inline float RichTextAreaWidget::GetLineSpacingOffset() const
	{
		return m_drawer.GetDefaultLineSpacingOffset();
	}

	inline const Color& RichTextAreaWidget::GetTextColor() const
	{
		return m_drawer.GetDefaultColor();
	}

	inline const std::shared_ptr<Font>& RichTextAreaWidget::GetTextFont() const
	{
		return m_drawer.GetDefaultFont();
	}

	inline const Color& RichTextAreaWidget::GetTextOutlineColor() const
	{
		return m_drawer.GetDefaultOutlineColor();
	}

	inline float RichTextAreaWidget::GetTextOutlineThickness() const
	{
		return m_drawer.GetDefaultOutlineThickness();
	}

	inline TextStyleFlags RichTextAreaWidget::GetTextStyle() const
	{
		return m_drawer.GetDefaultStyle();
	}

	inline void RichTextAreaWidget::SetCharacterSize(unsigned int characterSize)
	{
		m_drawer.SetDefaultCharacterSize(characterSize);
	}

	inline void RichTextAreaWidget::SetCharacterSpacingOffset(float offset)
	{
		m_drawer.SetDefaultCharacterSpacingOffset(offset);
	}

	inline void RichTextAreaWidget::SetLineSpacingOffset(float offset)
	{
		m_drawer.SetDefaultLineSpacingOffset(offset);
	}

	inline void RichTextAreaWidget::SetTextColor(const Color& color)
	{
		m_drawer.SetDefaultColor(color);
	}

	inline void RichTextAreaWidget::SetTextFont(std::shared_ptr<Font> font)
	{
		m_drawer.SetDefaultFont(std::move(font));
	}

	inline void RichTextAreaWidget::SetTextOutlineColor(const Color& color)
	{
		m_drawer.SetDefaultOutlineColor(color);
	}

	inline void RichTextAreaWidget::SetTextOutlineThickness(float thickness)
	{
		m_drawer.SetDefaultOutlineThickness(thickness);
	}

	inline void RichTextAreaWidget::SetTextStyle(TextStyleFlags style)
	{
		m_drawer.SetDefaultStyle(style);
	}
}

#include <Nazara/Widgets/DebugOff.hpp>
