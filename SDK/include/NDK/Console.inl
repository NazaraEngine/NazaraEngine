// Copyright (C) 2015 Jérôme Leclercq
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

	inline const TextAreaWidget* Console::GetHistory() const
	{
		return m_history;
	}

	/*!
	* \brief Gets the entity representing the input of the console
	* \return Input of the console
	*/

	inline const TextAreaWidget* Console::GetInput() const
	{
		return m_input;
	}

	/*!
	* \brief Gets the font used by the console
	* \return A reference to the font currenty used
	*/

	inline const Nz::FontRef& Console::GetTextFont() const
	{
		return m_defaultFont;
	}
}
