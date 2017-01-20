// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Console.hpp>
#include <Nazara/Core/Unicode.hpp>
#include <Nazara/Lua/LuaInstance.hpp>
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

	Console::Console(BaseWidget* parent, Nz::LuaInstance& instance) :
	BaseWidget(parent),
	m_historyPosition(0),
	m_defaultFont(Nz::Font::GetDefault()),
	m_instance(instance),
	m_characterSize(24)
	{
		// History
		m_history = Add<TextAreaWidget>();
		m_history->EnableBackground(true);
		m_history->SetReadOnly(true);
		m_history->SetBackgroundColor(Nz::Color(80, 80, 160, 128));

		// Input
		m_input = Add<TextAreaWidget>();
		m_input->EnableBackground(true);
		m_input->SetText(s_inputPrefix);
		m_input->SetTextColor(Nz::Color::Black);
	}

	/*!
	* \brief Adds a line to the console
	*
	* \param text New line of text
	* \param color Color for the text
	*/

	void Console::AddLine(const Nz::String& text, const Nz::Color& color)
	{
		AddLineInternal(text, color);
		RefreshHistory();
	}

	/*!
	* \brief Clears the console
	*/

	void Console::Clear()
	{
		m_historyLines.clear();
		RefreshHistory();
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
	* \brief Adds a line to the history of the console
	*
	* \param text New line of text
	* \param color Color for the text
	*/

	void Console::AddLineInternal(const Nz::String& text, const Nz::Color& color)
	{
		m_historyLines.emplace_back(Line{color, text});
	}

	/*!
	* \brief Performs this action when an input is added to the console
	*/

	void Console::ExecuteInput()
	{
		/*Nz::String input = m_inputDrawer.GetText();
		Nz::String inputCmd = input.SubString(s_inputPrefixSize);;
		m_inputDrawer.SetText(s_inputPrefix);

		if (m_commandHistory.empty() || m_commandHistory.back() != inputCmd)
			m_commandHistory.push_back(inputCmd);

		m_historyPosition = 0;

		AddLineInternal(input); //< With the input prefix

		if (!m_instance.Execute(inputCmd))
			AddLineInternal(m_instance.GetLastError(), Nz::Color::Red);

		RefreshHistory();*/
	}

	/*!
	* \brief Places the console according to its layout
	*/

	void Console::Layout()
	{
		const Nz::Vector2f& size = GetContentSize();

		unsigned int lineHeight = m_defaultFont->GetSizeInfo(m_characterSize).lineHeight;
		float historyHeight = size.y - lineHeight - 5.f - 2.f;

		m_maxHistoryLines = static_cast<unsigned int>(std::ceil(historyHeight / lineHeight));

		m_history->SetSize({size.x, historyHeight});
		m_history->SetPosition(0.f, historyHeight - m_maxHistoryLines * lineHeight);

		m_input->SetPosition(0.f, historyHeight + 2.f);
		m_input->SetSize({size.x, size.y - historyHeight});
	}

	/*!
	* \brief Refreshes the history of the console
	*/

	void Console::RefreshHistory()
	{
		m_history->Clear();

		auto it = m_historyLines.end();
		if (m_historyLines.size() > m_maxHistoryLines)
			it -= m_maxHistoryLines;
		else
			it = m_historyLines.begin();

		for (unsigned int i = 0; i < m_maxHistoryLines; ++i)
		{
			if (m_maxHistoryLines - i <= m_historyLines.size() && it != m_historyLines.end())
			{
				m_history->AppendText(it->text);
				++it;
			}

			m_history->AppendText(Nz::String('\n'));
		}
	}
}
