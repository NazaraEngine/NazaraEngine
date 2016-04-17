// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <algorithm>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup core
	* \brief Gets the platform endianness
	* \return Type of the endianness
	*/

	inline constexpr Endianness GetPlatformEndianness()
	{
		#if defined(NAZARA_BIG_ENDIAN)
		return Endianness_BigEndian;
		#elif defined(NAZARA_LITTLE_ENDIAN)
		return Endianness_LittleEndian;
		#endif
	}

	/*!
	* \ingroup core
	* \brief Swaps the byte for endianness operations
	*
	* \param buffer Raw memory
	* \param size Size to change endianness
	*
	* \remark If size is greather than the preallocated buffer, the behaviour is undefined
	*/

	inline void SwapBytes(void* buffer, unsigned int size)
	{
		UInt8* bytes = static_cast<UInt8*>(buffer);
		unsigned int i = 0;
		unsigned int j = size - 1;

		while (i < j)
			std::swap(bytes[i++], bytes[j--]);
	}
}

#include <Nazara/Core/DebugOff.hpp>
