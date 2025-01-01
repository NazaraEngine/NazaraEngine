// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <NazaraUtils/Algorithm.hpp>

namespace Nz
{
	inline MessageBox::MessageBox(MessageBoxType type, std::string title, std::string message) :
	m_message(std::move(message)),
	m_title(std::move(title)),
	m_type(type)
	{
	}

	inline void MessageBox::AddButton(int id, std::string text, MessageBoxButtonRole role)
	{
		m_buttons.push_back({
			.role = role,
			.text = std::move(text),
			.buttonId = id
		});
	}

	inline auto MessageBox::GetColorSchemeOverride() const -> const ColorScheme*
	{
		if (!m_colorSchemeOverride)
			return nullptr;

		return &m_colorSchemeOverride.value();
	}

	inline const std::string& MessageBox::GetMessage() const
	{
		return m_message;
	}

	inline const std::string& Nz::MessageBox::GetTitle() const
	{
		return m_title;
	}

	inline void MessageBox::OverrideColorScheme(const ColorScheme& colorScheme)
	{
		m_colorSchemeOverride = colorScheme;
	}

	inline void MessageBox::ResetColorScheme()
	{
		m_colorSchemeOverride = std::nullopt;
	}

	inline void MessageBox::UpdateMessage(std::string message)
	{
		m_message = std::move(message);
	}

	inline void MessageBox::UpdateTitle(std::string title)
	{
		m_title = std::move(title);
	}

	inline void MessageBox::UpdateType(MessageBoxType type)
	{
		m_type = type;
	}
}
