// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	inline void SimpleLabelWidget::AppendText(std::string_view str)
	{
		m_drawer.AppendText(str);
		UpdateTextAndSize();
	}

	inline void SimpleLabelWidget::Clear()
	{
		m_drawer.Clear();
		UpdateTextAndSize();
	}

	inline unsigned int SimpleLabelWidget::GetCharacterSize() const
	{
		return m_drawer.GetCharacterSize();
	}

	inline const SimpleTextDrawer& SimpleLabelWidget::GetDrawer() const
	{
		return m_drawer;
	}

	inline const std::string& SimpleLabelWidget::GetText() const
	{
		return m_drawer.GetText();
	}

	inline const Color& SimpleLabelWidget::GetTextColor() const
	{
		return m_drawer.GetTextColor();
	}

	inline void SimpleLabelWidget::SetCharacterSize(unsigned int characterSize)
	{
		m_drawer.SetCharacterSize(characterSize);
		UpdateTextAndSize();
	}

	inline void SimpleLabelWidget::SetText(std::string text)
	{
		m_drawer.SetText(std::move(text));
		UpdateTextAndSize();
	}

	inline void SimpleLabelWidget::SetTextColor(const Color& color)
	{
		m_drawer.SetTextColor(color);
		UpdateTextAndSize();
	}

	template<typename F>
	void SimpleLabelWidget::UpdateDrawer(F&& callback)
	{
		using Ret = decltype(callback(m_drawer));
		if constexpr (std::is_void_v<Ret>)
			callback(m_drawer);
		else
		{
			static_assert(std::is_same_v<Ret, bool>, "callback must either return a boolean or nothing");
			if (!callback(m_drawer))
				return;
		}

		UpdateTextAndSize();
	}
}

