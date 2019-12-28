// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NDK/Widgets/RichTextAreaWidget.hpp>

namespace Ndk
{
	inline unsigned int RichTextAreaWidget::GetCharacterSize() const
	{
		return m_drawer.GetDefaultCharacterSize();
	}

	inline const Nz::Color& RichTextAreaWidget::GetTextColor() const
	{
		return m_drawer.GetDefaultColor();
	}

	inline Nz::Font* RichTextAreaWidget::GetTextFont() const
	{
		return m_drawer.GetDefaultFont();
	}

	inline const Nz::Color& RichTextAreaWidget::GetTextOutlineColor() const
	{
		return m_drawer.GetDefaultOutlineColor();
	}

	inline float RichTextAreaWidget::GetTextOutlineThickness() const
	{
		return m_drawer.GetDefaultOutlineThickness();
	}

	inline Nz::TextStyleFlags RichTextAreaWidget::GetTextStyle() const
	{
		return m_drawer.GetDefaultStyle();
	}

	inline void RichTextAreaWidget::SetCharacterSize(unsigned int characterSize)
	{
		m_drawer.SetDefaultCharacterSize(characterSize);
	}

	inline void RichTextAreaWidget::SetTextColor(const Nz::Color& color)
	{
		m_drawer.SetDefaultColor(color);
	}

	inline void RichTextAreaWidget::SetTextFont(Nz::FontRef font)
	{
		m_drawer.SetDefaultFont(std::move(font));
	}

	inline void RichTextAreaWidget::SetTextOutlineColor(const Nz::Color& color)
	{
		m_drawer.SetDefaultOutlineColor(color);
	}

	inline void RichTextAreaWidget::SetTextOutlineThickness(float thickness)
	{
		m_drawer.SetDefaultOutlineThickness(thickness);
	}

	inline void RichTextAreaWidget::SetTextStyle(Nz::TextStyleFlags style)
	{
		m_drawer.SetDefaultStyle(style);
	}
}
