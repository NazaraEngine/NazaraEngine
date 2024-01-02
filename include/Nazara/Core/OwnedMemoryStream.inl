// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \brief Constructs an OwnedMemoryStream object
	*
	* \param openMode Reading/writing mode for the stream
	*/
	inline OwnedMemoryStream::OwnedMemoryStream(OpenModeFlags openMode)
	{
		SetBuffer(&m_ownedByteArray, openMode);
	}

	/*!
	* \brief Constructs an OwnedMemoryStream object
	*
	* \param byteArray Content
	* \param openMode Reading/writing mode for the stream
	*/
	inline OwnedMemoryStream::OwnedMemoryStream(ByteArray byteArray, OpenModeFlags openMode) :
	m_ownedByteArray(std::move(byteArray))
	{
		SetBuffer(&m_ownedByteArray, openMode);
	}
}

#include <Nazara/Core/DebugOff.hpp>
