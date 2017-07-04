// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::ErrorFlags
	* \brief Core class that represents flags for error
	*/

	/*!
	* \brief Constructs a ErrorFlags object with flags
	*
	* \param flags Flags for the error
	* \param replace Replace the entirely the old flag if true, else do a "OR"
	*/

	ErrorFlags::ErrorFlags(UInt32 flags, bool replace) :
	m_previousFlags(Error::GetFlags())
	{
		SetFlags(flags, replace);
	}

	/*!
	* \brief Destructs the object and sets the old flag
	*/

	ErrorFlags::~ErrorFlags()
	{
		Error::SetFlags(m_previousFlags);
	}

	/*!
	* \brief Gets the previous flag
	* \return Previous flag
	*/

	UInt32 ErrorFlags::GetPreviousFlags() const
	{
		return m_previousFlags;
	}

	/*!
	* \brief Sets the flags
	*
	* \param flags Flags for the error
	* \param replace Replace the entirely the old flag if true, else do a "OR"
	*/

	void ErrorFlags::SetFlags(UInt32 flags, bool replace)
	{
		if (!replace)
			flags |= m_previousFlags;

		Error::SetFlags(flags);
	}
}
