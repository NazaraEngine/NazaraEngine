// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Platform/MessageBox.hpp>
#include <NazaraUtils/EnumArray.hpp>

#ifdef NAZARA_PLATFORM_ANDROID
#include <Nazara/Platform/Android/MessageBoxImpl.hpp>
#else
#include <Nazara/Platform/SDL3/MessageBoxImpl.hpp>
#endif

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
		return MessageBoxImpl::Show(*this, parent);
	}
}
