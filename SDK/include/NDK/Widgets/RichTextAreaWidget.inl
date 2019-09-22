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
}
