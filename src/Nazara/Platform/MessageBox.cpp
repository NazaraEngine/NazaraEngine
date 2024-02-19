// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Platform/MessageBox.hpp>
#include <Nazara/Platform/SDL2/WindowImpl.hpp>
#include <NazaraUtils/EnumArray.hpp>
#include <NazaraUtils/StackArray.hpp>
#include <SDL_messagebox.h>

namespace Nz
{
	void MessageBox::AddButton(int id, MessageBoxStandardButton standardButton)
	{
		struct StandardButtonData
		{
			std::string_view text;
			MessageBoxButtonRole role;
		};

		constexpr EnumArray<MessageBoxStandardButton, StandardButtonData> s_buttonTexts = {
			StandardButtonData { "Abort",      MessageBoxButtonRole::Reject }, // Abort
			StandardButtonData { "Apply",      MessageBoxButtonRole::Accept }, // Apply
			StandardButtonData { "Cancel",     MessageBoxButtonRole::Reject }, // Cancel
			StandardButtonData { "Close",      MessageBoxButtonRole::Reject }, // Close
			StandardButtonData { "Discard",    MessageBoxButtonRole::Reject }, // Discard
			StandardButtonData { "Ignore",     MessageBoxButtonRole::Reject }, // Ignore
			StandardButtonData { "No",         MessageBoxButtonRole::Reject }, // No
			StandardButtonData { "No to all",  MessageBoxButtonRole::Reject }, // NoToAll
			StandardButtonData { "Ok",         MessageBoxButtonRole::Accept }, // Ok
			StandardButtonData { "Reset",      MessageBoxButtonRole::Reject }, // Reset
			StandardButtonData { "Retry",      MessageBoxButtonRole::Accept }, // Retry
			StandardButtonData { "Save",       MessageBoxButtonRole::Accept }, // Save
			StandardButtonData { "Save all",   MessageBoxButtonRole::Accept }, // SaveAll
			StandardButtonData { "Yes",        MessageBoxButtonRole::Accept }, // Yes
			StandardButtonData { "Yes to all", MessageBoxButtonRole::Accept }, // YesToAll
		};

		return AddButton(id, std::string(s_buttonTexts[standardButton].text), s_buttonTexts[standardButton].role);
	}

	Result<int, const char*> MessageBox::Show(Window* parent) const
	{
		constexpr EnumArray<MessageBoxType, Uint32> s_flags = {
			SDL_MESSAGEBOX_INFORMATION, // Info
			SDL_MESSAGEBOX_ERROR,       // Error
			SDL_MESSAGEBOX_WARNING      // Warning
		};

		constexpr EnumArray<MessageBoxButtonRole, Uint32> s_buttonFlags = {
			SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, // Accept
			0, // None
			SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, // Reject
		};

		StackArray<SDL_MessageBoxButtonData> buttons = NazaraStackArrayNoInit(SDL_MessageBoxButtonData, m_buttons.size());

		SDL_MessageBoxButtonData* buttonPtr = buttons.data();
		for (const ButtonData& buttonData : m_buttons)
		{
			buttonPtr->buttonid = buttonData.buttonId;
			buttonPtr->flags = s_buttonFlags[buttonData.role];
			buttonPtr->text = buttonData.text.c_str();

			buttonPtr++;
		}

		SDL_MessageBoxColorScheme colorScheme;
		if (m_colorSchemeOverride)
		{
			Color::ToRGB8(m_colorSchemeOverride->backgroundColor, &colorScheme.colors[SDL_MESSAGEBOX_COLOR_BACKGROUND].r, &colorScheme.colors[SDL_MESSAGEBOX_COLOR_BACKGROUND].g, &colorScheme.colors[SDL_MESSAGEBOX_COLOR_BACKGROUND].b);
			Color::ToRGB8(m_colorSchemeOverride->buttonBackgroundColor, &colorScheme.colors[SDL_MESSAGEBOX_COLOR_BUTTON_BACKGROUND].r, &colorScheme.colors[SDL_MESSAGEBOX_COLOR_BUTTON_BACKGROUND].g, &colorScheme.colors[SDL_MESSAGEBOX_COLOR_BUTTON_BACKGROUND].b);
			Color::ToRGB8(m_colorSchemeOverride->buttonBorderColor, &colorScheme.colors[SDL_MESSAGEBOX_COLOR_BUTTON_BORDER].r, &colorScheme.colors[SDL_MESSAGEBOX_COLOR_BUTTON_BORDER].g, &colorScheme.colors[SDL_MESSAGEBOX_COLOR_BUTTON_BORDER].b);
			Color::ToRGB8(m_colorSchemeOverride->buttonSelectedColor, &colorScheme.colors[SDL_MESSAGEBOX_COLOR_BUTTON_SELECTED].r, &colorScheme.colors[SDL_MESSAGEBOX_COLOR_BUTTON_SELECTED].g, &colorScheme.colors[SDL_MESSAGEBOX_COLOR_BUTTON_SELECTED].b);
			Color::ToRGB8(m_colorSchemeOverride->textColor, &colorScheme.colors[SDL_MESSAGEBOX_COLOR_TEXT].r, &colorScheme.colors[SDL_MESSAGEBOX_COLOR_TEXT].g, &colorScheme.colors[SDL_MESSAGEBOX_COLOR_TEXT].b);
		}

		SDL_Window* window = (parent) ? static_cast<const WindowImpl*>(parent->GetImpl())->GetHandle() : nullptr;

		SDL_MessageBoxData messageBoxData = {
			.flags = s_flags[m_type],
			.window = window,
			.title = m_title.c_str(),
			.message = m_message.c_str(),
			.numbuttons = SafeCast<int>(m_buttons.size()),
			.buttons = buttons.data(),
			.colorScheme = (m_colorSchemeOverride) ? &colorScheme : nullptr
		};

		int buttonId;
		if (SDL_ShowMessageBox(&messageBoxData, &buttonId) != 0)
			return Err(SDL_GetError());

		return Ok(buttonId);
	}
}
