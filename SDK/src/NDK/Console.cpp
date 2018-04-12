// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NDK/Console.hpp>
#include <Nazara/Core/Unicode.hpp>
#include <Nazara/Lua/LuaState.hpp>
#include <Nazara/Platform/Event.hpp>
#include <NDK/Components/GraphicsComponent.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/Widgets.hpp>
#include <NDK/World.hpp>

///TODO: For now is unable to display different color in the history, it needs a RichTextDrawer to do so

namespace Ndk
{
	namespace
	{
		const char s_inputPrefix[] = "> ";
		constexpr std::size_t s_inputPrefixSize = Nz::CountOf(s_inputPrefix) - 1;
	}

	/*!
	* \ingroup NDK
	* \class Ndk::Console
	* \brief NDK class that represents a console to help development with Lua scripting
	*/

	/*!
	* \brief Constructs a Console object with a world to interact with
	*
	* \param world World to interact with
	* \param size (Width, Height) of the console
	* \param instance Lua instance that will interact with the world
	*/

	Console::Console(BaseWidget* parent, Nz::LuaState& state) :
	BaseWidget(parent),
	m_historyPosition(0),
	m_defaultFont(Nz::Font::GetDefault()),
	m_state(state),
	m_characterSize(24)
	{
		// History
		m_history = Add<TextAreaWidget>();
		m_history->EnableBackground(true);
		m_history->SetReadOnly(true);
		m_history->SetBackgroundColor(Nz::Color(80, 80, 160, 128));
		m_history->SetPadding(0.f, 0.f, 0.f, 4.f);

		// Input
		m_input = Add<TextAreaWidget>();
		m_input->EnableBackground(true);
		m_input->SetText(s_inputPrefix);
		m_input->SetTextColor(Nz::Color::Black);
		m_input->SetPadding(0.f, 2.f, 0.f, 2.f);

		m_input->OnTextAreaKeyReturn.Connect(this, &Console::ExecuteInput);

		// Protect input prefix from erasure/selection
		m_input->SetCursorPosition(s_inputPrefixSize);

		m_input->OnTextAreaCursorMove.Connect([](const TextAreaWidget* textArea, std::size_t* newCursorPos)
		{
			*newCursorPos = std::max(*newCursorPos, s_inputPrefixSize);
		});

		m_input->OnTextAreaKeyBackspace.Connect([](const TextAreaWidget* textArea, bool* ignoreDefaultAction)
		{
			if (textArea->GetGlyphIndex() <= s_inputPrefixSize)
				*ignoreDefaultAction = true;
		});

		// Handle history
		m_input->OnTextAreaKeyUp.Connect([&] (const TextAreaWidget* textArea, bool* ignoreDefaultAction)
		{
			*ignoreDefaultAction = true;

			if (m_historyPosition > 0)
				m_historyPosition--;

			m_input->SetText(s_inputPrefix + m_commandHistory[m_historyPosition]);
		});

		m_input->OnTextAreaKeyDown.Connect([&] (const TextAreaWidget* textArea, bool* ignoreDefaultAction)
		{
			*ignoreDefaultAction = true;

			if (++m_historyPosition >= m_commandHistory.size())
				m_historyPosition = 0;

			m_input->SetText(s_inputPrefix + m_commandHistory[m_historyPosition]);
		});

		// General
		SetPadding(0.f, 0.f, 0.f, 0.f);
	}

	/*!
	* \brief Adds a line to the console
	*
	* \param text New line of text
	* \param color Color for the text
	*/

	void Console::AddLine(const Nz::String& text, const Nz::Color& color)
	{
		m_historyLines.emplace_back(Line{ color, text });
		m_history->AppendText(text + '\n');
	}

	/*!
	* \brief Clears the console
	*/

	void Console::Clear()
	{
		m_historyLines.clear();
		m_history->Clear();
		m_input->SetText(s_inputPrefix);
	}

	void Console::ResizeToContent()
	{
	}

	/*!
	* \brief Sets the character size
	*
	* \param size Size of the font
	*/

	void Console::SetCharacterSize(unsigned int size)
	{
		m_characterSize = size;

		//m_historyDrawer.SetCharacterSize(m_characterSize);
		//m_historyTextSprite->Update(m_historyDrawer);
		//m_inputDrawer.SetCharacterSize(m_characterSize);
		//m_inputTextSprite->Update(m_inputDrawer);

		Layout();
	}

	/*!
	* \brief Sets the text font
	*
	* \param font Reference to a valid font
	*
	* \remark Produces a NazaraAssert if font is invalid or null
	*/

	void Console::SetTextFont(Nz::FontRef font)
	{
		NazaraAssert(font && font->IsValid(), "Invalid font");

		m_defaultFont = std::move(font);
		//m_historyDrawer.SetFont(m_defaultFont);
		//m_inputDrawer.SetFont(m_defaultFont);

		Layout();
	}

	/*!
	* \brief Performs this action when an input is added to the console
	*/

	void Console::ExecuteInput(const TextAreaWidget* textArea, bool* ignoreDefaultAction)
	{
		NazaraAssert(textArea == m_input, "Unexpected signal from an other text area");

		Nz::String input = m_input->GetText();
		Nz::String inputCmd = input.SubString(s_inputPrefixSize);
		m_input->SetText(s_inputPrefix);

		if (m_commandHistory.empty() || m_commandHistory.back() != inputCmd)
			m_commandHistory.push_back(inputCmd);

		m_historyPosition = m_commandHistory.size();

		AddLine(input); //< With the input prefix

		if (!m_state.Execute(inputCmd))
			AddLine(m_state.GetLastError(), Nz::Color::Red);
	}

	/*!
	* \brief Places the console according to its layout
	*/

	void Console::Layout()
	{
		Nz::Vector2f origin = GetContentOrigin();
		const Nz::Vector2f& size = GetContentSize();

		unsigned int lineHeight = m_defaultFont->GetSizeInfo(m_characterSize).lineHeight;
		float historyHeight = size.y - lineHeight;

		m_maxHistoryLines = static_cast<unsigned int>(std::ceil(historyHeight / lineHeight));
		float diff = historyHeight - m_maxHistoryLines * lineHeight;

		m_history->SetPosition(origin.x, origin.y + diff);
		m_history->SetSize({size.x, historyHeight - diff - 4.f});

		m_input->SetContentSize({size.x, size.y - historyHeight});
		m_input->SetPosition(origin.x, origin.y + historyHeight);
	}
}
