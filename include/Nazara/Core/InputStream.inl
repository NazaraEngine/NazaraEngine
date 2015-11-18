// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>

namespace Nz
{
	inline InputStream::InputStream() :
	Stream(OpenMode_Current)
	{
	}

	template<typename T>
	InputStream& InputStream::operator>>(T& value)
	{
		if (!Unserialize(this, &value, m_dataEndianness))
			NazaraError("Failed to unserialize value");

		return *this;
	}
}
