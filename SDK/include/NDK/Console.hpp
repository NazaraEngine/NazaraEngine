// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NDK_CONSOLE_HPP
#define NDK_CONSOLE_HPP

#include <NDK/ClientPrerequisites.hpp>
#include <Nazara/Core/HandledObject.hpp>
#include <Nazara/Core/ObjectHandle.hpp>
#include <Nazara/Graphics/Sprite.hpp>
#include <Nazara/Graphics/TextSprite.hpp>
#include <Nazara/Utility/Node.hpp>
#include <Nazara/Utility/SimpleTextDrawer.hpp>
#include <NDK/BaseWidget.hpp>
#include <NDK/EntityOwner.hpp>

namespace Nz
{
	struct WindowEvent;
}

namespace Ndk
{
	class AbstractTextAreaWidget;
	class Console;
	class Entity;
	class RichTextAreaWidget;
	class ScrollAreaWidget;
	class TextAreaWidget;

	using ConsoleHandle = Nz::ObjectHandle<Console>;

	class NDK_CLIENT_API Console : public BaseWidget, public Nz::HandledObject<Console>
	{
		public:
			Console(BaseWidget* parent);
			Console(const Console& console) = delete;
			Console(Console&& console) = default;
			~Console() = default;

			void AddLine(const Nz::String& text, const Nz::Color& color = Nz::Color::White);

			void Clear();
			void ClearFocus();

			inline unsigned int GetCharacterSize() const;
			inline const RichTextAreaWidget* GetHistory() const;
			inline const TextAreaWidget* GetInput() const;
			inline const Nz::FontRef& GetTextFont() const;

			void SetCharacterSize(unsigned int size);
			void SetFocus();
			void SetTextFont(Nz::FontRef font);

			Console& operator=(const Console& console) = delete;
			Console& operator=(Console&& console) = default;

			NazaraSignal(OnCommand, Console* /*console*/, const Nz::String& /*command*/);

		private:
			void ExecuteInput(const AbstractTextAreaWidget* textArea, bool* ignoreDefaultAction);
			void Layout() override;

			struct Line
			{
				Nz::Color color;
				Nz::String text;
			};

			std::size_t m_historyPosition;
			std::vector<Nz::String> m_commandHistory;
			std::vector<Line> m_historyLines;
			ScrollAreaWidget* m_historyArea;
			RichTextAreaWidget* m_history;
			TextAreaWidget* m_input;
			Nz::FontRef m_defaultFont;
			unsigned int m_characterSize;
			unsigned int m_maxHistoryLines;
	};
}

#include <NDK/Console.inl>

#endif // NDK_CONSOLE_HPP
