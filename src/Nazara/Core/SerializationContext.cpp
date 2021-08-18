// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/SerializationContext.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{

	/*!
	* \ingroup core
	* \class Nz::SerializationContext
	* \brief Structure containing a serialization/unserialization context states
	*/

	/*!
	 * Write bits to the stream (if any) and reset the current bit cursor
	
	* \see ResetWriteBitPosition
	*/
	void SerializationContext::FlushBits()
	{
		if (writeBitPos != 8)
		{
			ResetWriteBitPosition();

			// Serialize will reset the bit position
			if (!Serialize(*this, writeByte))
				NazaraWarning("Failed to flush bits");
		}
	}
}
