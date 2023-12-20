// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PLATFORM_MESSAGEBOX_HPP
#define NAZARA_PLATFORM_MESSAGEBOX_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Platform/Config.hpp>
#include <Nazara/Platform/Enums.hpp>
#include <NazaraUtils/Result.hpp>
#include <string>
#include <vector>

namespace Nz
{
	class Window;

	class NAZARA_PLATFORM_API MessageBox
	{
		public:
			struct ColorScheme;

			inline MessageBox(MessageBoxType type, std::string title, std::string message);
			MessageBox(const MessageBox&) = delete;
			MessageBox(MessageBox&&) = delete;
			~MessageBox() = default;

			inline void AddButton(int id, std::string text, MessageBoxButtonRole role = MessageBoxButtonRole::None);
			void AddButton(int id, MessageBoxStandardButton standardButton);

			inline const ColorScheme* GetColorSchemeOverride() const;
			inline const std::string& GetMessage() const;
			inline const std::string& GetTitle() const;

			inline void OverrideColorScheme(const ColorScheme& colorScheme);

			inline void ResetColorScheme();

			Result<int, const char*> Show(Window* parent = nullptr) const;

			inline void UpdateMessage(std::string message);
			inline void UpdateTitle(std::string title);
			inline void UpdateType(MessageBoxType type);

			MessageBox& operator=(const MessageBox&) = delete;
			MessageBox& operator=(MessageBox&&) = delete;

			struct ColorScheme
			{
				Color backgroundColor = Color::White();
				Color textColor = Color::Black();
				Color buttonBackgroundColor = Color::Gray();
				Color buttonBorderColor = Color::Black();
				Color buttonSelectedColor = Color::Blue();
			};

		private:
			struct ButtonData
			{
				MessageBoxButtonRole role;
				std::string text;
				int buttonId;
			};

			std::optional<ColorScheme> m_colorSchemeOverride;
			std::string m_message;
			std::string m_title;
			std::vector<ButtonData> m_buttons;
			MessageBoxType m_type;
	};
}

#include <Nazara/Platform/MessageBox.inl>

#endif // NAZARA_PLATFORM_MESSAGEBOX_HPP
