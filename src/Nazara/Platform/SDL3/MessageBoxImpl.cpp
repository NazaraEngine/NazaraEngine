// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Platform/SDL3/MessageBoxImpl.hpp>
#include <Nazara/Platform/MessageBox.hpp>
#include <Nazara/Platform/SDL3/WindowImpl.hpp>
#include <NazaraUtils/EnumArray.hpp>
#include <NazaraUtils/StackArray.hpp>
#include <SDL3/SDL_messagebox.h>

namespace Nz
{
	Result<int, const char*> MessageBoxImpl::Show(const MessageBox& messageBox, const Window* parent)
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

		StackArray<SDL_MessageBoxButtonData> buttons = NazaraStackArrayNoInit(SDL_MessageBoxButtonData, messageBox.m_buttons.size());

		SDL_MessageBoxButtonData* buttonPtr = buttons.data();
		for (const MessageBox::ButtonData& buttonData : messageBox.m_buttons)
		{
			buttonPtr->buttonID = buttonData.buttonId;
			buttonPtr->flags = s_buttonFlags[buttonData.role];
			buttonPtr->text = buttonData.text.c_str();

			buttonPtr++;
		}

		SDL_MessageBoxColorScheme colorScheme;
		if (messageBox.m_colorSchemeOverride)
		{
			Color::ToRGB8(messageBox.m_colorSchemeOverride->backgroundColor, &colorScheme.colors[SDL_MESSAGEBOX_COLOR_BACKGROUND].r, &colorScheme.colors[SDL_MESSAGEBOX_COLOR_BACKGROUND].g, &colorScheme.colors[SDL_MESSAGEBOX_COLOR_BACKGROUND].b);
			Color::ToRGB8(messageBox.m_colorSchemeOverride->buttonBackgroundColor, &colorScheme.colors[SDL_MESSAGEBOX_COLOR_BUTTON_BACKGROUND].r, &colorScheme.colors[SDL_MESSAGEBOX_COLOR_BUTTON_BACKGROUND].g, &colorScheme.colors[SDL_MESSAGEBOX_COLOR_BUTTON_BACKGROUND].b);
			Color::ToRGB8(messageBox.m_colorSchemeOverride->buttonBorderColor, &colorScheme.colors[SDL_MESSAGEBOX_COLOR_BUTTON_BORDER].r, &colorScheme.colors[SDL_MESSAGEBOX_COLOR_BUTTON_BORDER].g, &colorScheme.colors[SDL_MESSAGEBOX_COLOR_BUTTON_BORDER].b);
			Color::ToRGB8(messageBox.m_colorSchemeOverride->buttonSelectedColor, &colorScheme.colors[SDL_MESSAGEBOX_COLOR_BUTTON_SELECTED].r, &colorScheme.colors[SDL_MESSAGEBOX_COLOR_BUTTON_SELECTED].g, &colorScheme.colors[SDL_MESSAGEBOX_COLOR_BUTTON_SELECTED].b);
			Color::ToRGB8(messageBox.m_colorSchemeOverride->textColor, &colorScheme.colors[SDL_MESSAGEBOX_COLOR_TEXT].r, &colorScheme.colors[SDL_MESSAGEBOX_COLOR_TEXT].g, &colorScheme.colors[SDL_MESSAGEBOX_COLOR_TEXT].b);
		}

		SDL_Window* window = (parent) ? static_cast<const WindowImpl*>(parent->GetImpl())->GetHandle() : nullptr;

		SDL_MessageBoxData messageBoxData = {
			.flags = s_flags[messageBox.m_type],
			.window = window,
			.title = messageBox.m_title.c_str(),
			.message = messageBox.m_message.c_str(),
			.numbuttons = SafeCast<int>(messageBox.m_buttons.size()),
			.buttons = buttons.data(),
			.colorScheme = (messageBox.m_colorSchemeOverride) ? &colorScheme : nullptr
		};

		int buttonId;
		if (SDL_ShowMessageBox(&messageBoxData, &buttonId) != 0)
			return Err(SDL_GetError());

		return Ok(buttonId);
	}
}
