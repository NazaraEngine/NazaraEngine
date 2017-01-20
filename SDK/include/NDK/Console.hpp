// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_SERVER
#ifndef NDK_CONSOLE_HPP
#define NDK_CONSOLE_HPP

#include <Nazara/Core/HandledObject.hpp>
#include <Nazara/Core/ObjectHandle.hpp>
#include <Nazara/Graphics/Sprite.hpp>
#include <Nazara/Graphics/TextSprite.hpp>
#include <Nazara/Utility/Event.hpp>
#include <Nazara/Utility/Node.hpp>
#include <Nazara/Utility/SimpleTextDrawer.hpp>
#include <NDK/BaseWidget.hpp>
#include <NDK/EntityOwner.hpp>

namespace Nz
{
	class LuaInstance;
}

namespace Ndk
{
	class Console;
	class Entity;
	class TextAreaWidget;

	using ConsoleHandle = Nz::ObjectHandle<Console>;

	class NDK_API Console : public BaseWidget, public Nz::HandledObject<Console>
	{
		public:
			Console(BaseWidget* parent, Nz::LuaInstance& instance);
			Console(const Console& console) = delete;
			Console(Console&& console) = default;
			~Console() = default;

			void AddLine(const Nz::String& text, const Nz::Color& color = Nz::Color::White);

			void Clear();

			inline unsigned int GetCharacterSize() const;
			inline const TextAreaWidget* GetHistory() const;
			inline const TextAreaWidget* GetInput() const;
			inline const Nz::FontRef& GetTextFont() const;

			void ResizeToContent() override;

			void SetCharacterSize(unsigned int size);
			void SetTextFont(Nz::FontRef font);

			Console& operator=(const Console& console) = delete;
			Console& operator=(Console&& console) = default;

		private:
			void ExecuteInput(const TextAreaWidget* textArea, bool* ignoreDefaultAction);
			void Layout() override;

			struct Line
			{
				Nz::Color color;
				Nz::String text;
			};

			std::size_t m_historyPosition;
			std::vector<Nz::String> m_commandHistory;
			std::vector<Line> m_historyLines;
			TextAreaWidget* m_history;
			TextAreaWidget* m_input;
			Nz::FontRef m_defaultFont;
			Nz::LuaInstance& m_instance;
			unsigned int m_characterSize;
			unsigned int m_maxHistoryLines;
	};
}

#include <NDK/Console.inl>

#endif // NDK_CONSOLE_HPP
#endif // NDK_SERVER
