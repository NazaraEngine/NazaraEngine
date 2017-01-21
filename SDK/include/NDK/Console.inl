// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <Nazara/Core/Error.hpp>
#include "Console.hpp"

namespace Ndk
{
	/*!
	* \brief Gets the character size
	* \return Height of the character
	*/

	inline unsigned int Console::GetCharacterSize() const
	{
		return m_characterSize;
	}

	/*!
	* \brief Gets the entity representing the history of the console
	* \return History of the console
	*/

	inline const EntityHandle& Console::GetHistory() const
	{
		return m_history;
	}

	/*!
	* \brief Gets the entity representing the background of the console's history
	* \return Background history of the console
	*/

	inline const EntityHandle& Console::GetHistoryBackground() const
	{
		return m_historyBackground;
	}

	/*!
	* \brief Gets the entity representing the input of the console
	* \return Input of the console
	*/

	inline const EntityHandle& Console::GetInput() const
	{
		return m_input;
	}

	/*!
	* \brief Gets the entity representing the background of the console's input
	* \return Background input of the console
	*/

	inline const EntityHandle& Console::GetInputBackground() const
	{
		return m_inputBackground;
	}

	/*!
	* \brief Gets the size of the console
	* \return Size (Width, Height) of the console
	*/

	inline const Nz::Vector2f& Console::GetSize() const
	{
		return m_size;
	}

	/*!
	* \brief Gets the font used by the console
	* \return A reference to the font currenty used
	*/

	inline const Nz::FontRef& Console::GetTextFont() const
	{
		return m_defaultFont;
	}

	/*!
	* \brief Checks whether the console is visible
	* \return true If it is the case
	*/

	inline bool Console::IsVisible() const
	{
		return m_opened;
	}
}
