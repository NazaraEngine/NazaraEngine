// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NDK/Console.hpp>
#include <Nazara/Core/Unicode.hpp>
#include <Nazara/Platform/Event.hpp>
#include <NDK/Components/GraphicsComponent.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/Widgets.hpp>
#include <NDK/World.hpp>

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

	Console::Console(BaseWidget* parent) :
	BaseWidget(parent),
	m_historyPosition(0),
	m_defaultFont(Nz::Font::GetDefault()),
	m_characterSize(24),
	m_maxHistoryLines(200)
	{
		// History
		m_history = Add<RichTextAreaWidget>();
		m_history->EnableBackground(true);
		m_history->EnableLineWrap(true);
		m_history->SetReadOnly(true);
		m_history->SetBackgroundColor(Nz::Color(80, 80, 160, 128));

		m_historyArea = Add<ScrollAreaWidget>(m_history);

		// Input
		m_input = Add<TextAreaWidget>();
		m_input->EnableBackground(true);
		m_input->SetText(s_inputPrefix);
		m_input->SetTextColor(Nz::Color::Black);

		m_input->OnTextAreaKeyReturn.Connect(this, &Console::ExecuteInput);

		// Protect input prefix from erasure/selection
		m_input->SetCursorPosition(s_inputPrefixSize);

		m_input->OnTextAreaCursorMove.Connect([](const AbstractTextAreaWidget* textArea, Nz::Vector2ui* newCursorPos)
		{
			newCursorPos->x = std::max(newCursorPos->x, static_cast<unsigned int>(s_inputPrefixSize));
		});

		m_input->OnTextAreaSelection.Connect([](const AbstractTextAreaWidget* textArea, Nz::Vector2ui* start, Nz::Vector2ui* end)
		{
			start->x = std::max(start->x, static_cast<unsigned int>(s_inputPrefixSize));
			end->x = std::max(end->x, static_cast<unsigned int>(s_inputPrefixSize));
		});

		m_input->OnTextAreaKeyBackspace.Connect([](const AbstractTextAreaWidget* textArea, bool* ignoreDefaultAction)
		{
			if (textArea->GetGlyphIndex() <= s_inputPrefixSize)
				*ignoreDefaultAction = true;
		});

		// Handle history
		m_input->OnTextAreaKeyUp.Connect([&] (const AbstractTextAreaWidget* textArea, bool* ignoreDefaultAction)
		{
			*ignoreDefaultAction = true;

			if (m_historyPosition > 0)
				m_historyPosition--;

			m_input->SetText(s_inputPrefix + m_commandHistory[m_historyPosition]);
		});

		m_input->OnTextAreaKeyDown.Connect([&] (const AbstractTextAreaWidget* textArea, bool* ignoreDefaultAction)
		{
			*ignoreDefaultAction = true;

			if (++m_historyPosition >= m_commandHistory.size())
				m_historyPosition = 0;

			m_input->SetText(s_inputPrefix + m_commandHistory[m_historyPosition]);
		});
	}

	/*!
	* \brief Adds a line to the console
	*
	* \param text New line of text
	* \param color Color for the text
	*/
	void Console::AddLine(const Nz::String& text, const Nz::Color& color)
	{
		if (m_historyLines.size() >= m_maxHistoryLines)
			m_historyLines.erase(m_historyLines.begin());

		m_historyLines.emplace_back(Line{ color, text });
		m_history->SetTextColor(color); 
		m_history->AppendText(text + '\n');
		m_history->Resize(m_history->GetPreferredSize());
		m_historyArea->Resize(m_historyArea->GetSize());
		m_historyArea->ScrollToRatio(1.f);
	}

	/*!
	* \brief Clears the console
	*
	* Clears the console history and input
	*/
	void Console::Clear()
	{
		m_historyLines.clear();
		m_history->Clear();
		m_history->Resize(m_history->GetPreferredSize());
		m_historyArea->Resize(m_historyArea->GetSize());
		m_input->SetText(s_inputPrefix);
	}

	/*!
	* \brief Clears the console focus
	*
	* Clear console input widget focus (if owned)
	*/
	void Console::ClearFocus()
	{
		m_input->ClearFocus();
	}

	/*!
	* \brief Sets the character size
	*
	* \param size Size of the font
	*/
	void Console::SetCharacterSize(unsigned int size)
	{
		m_characterSize = size;

		m_history->SetCharacterSize(size);
		m_input->SetCharacterSize(size);

		Layout();
	}

	/*!
	* \brief Give the console input focus
	*
	*/
	void Console::SetFocus()
	{
		m_input->SetFocus();
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
		m_history->SetTextFont(m_defaultFont);
		m_input->SetTextFont(m_defaultFont);

		Layout();
	}

	/*!
	* \brief Performs this action when an input is added to the console
	*/
	void Console::ExecuteInput(const AbstractTextAreaWidget* textArea, bool* ignoreDefaultAction)
	{
		NazaraAssert(textArea == m_input, "Unexpected signal from an other text area");

		*ignoreDefaultAction = true;

		Nz::String input = m_input->GetText();
		Nz::String inputCmd = input.SubString(s_inputPrefixSize);
		m_input->SetText(s_inputPrefix);

		if (m_commandHistory.empty() || m_commandHistory.back() != inputCmd)
			m_commandHistory.push_back(inputCmd);

		m_historyPosition = m_commandHistory.size();

		AddLine(input); //< With the input prefix

		OnCommand(this, inputCmd);
	}

	/*!
	* \brief Places the console according to its layout
	*/
	void Console::Layout()
	{
		Nz::Vector2f origin = Nz::Vector2f(GetPosition());
		const Nz::Vector2f& size = GetSize();

		unsigned int lineHeight = m_defaultFont->GetSizeInfo(m_characterSize).lineHeight;
		float historyHeight = size.y - lineHeight;

		m_historyArea->SetPosition(origin.x, origin.y);
		m_historyArea->Resize({ size.x, historyHeight - 4.f });

		m_input->Resize({size.x, size.y - historyHeight});
		m_input->SetPosition(origin.x, origin.y - historyHeight);
	}
}
