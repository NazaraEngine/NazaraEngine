// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/ByteArray.hpp>
#include <Nazara/Core/AbstractHash.hpp>
#include <ostream>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::ByteArray
	* \brief Core class that represents an array of bytes
	*/

	/*!
	* \brief Gives a string representation in base 16
	* \return String in base 16
	*/

	String ByteArray::ToHex() const
	{
		std::size_t length = m_array.size() * 2;

		String hexOutput(length, '\0');
		for (std::size_t i = 0; i < m_array.size(); ++i)
			std::sprintf(&hexOutput[i * 2], "%02x", m_array[i]);

		return hexOutput;
	}

	/*!
	* \brief Output operator
	* \return The stream
	*
	* \param out The stream
	* \param byteArray The ByteArray to output
	*/

	std::ostream& operator<<(std::ostream& out, const Nz::ByteArray& byteArray)
	{
		out << byteArray.ToHex();
		return out;
	}
}
