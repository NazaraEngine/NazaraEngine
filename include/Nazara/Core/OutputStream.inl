// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>

namespace Nz
{
	inline OutputStream::OutputStream() :
	Stream(OpenMode_Current)
	{
	}

	template<typename T>
	OutputStream& OutputStream::operator<<(const T& value)
	{
		if (!Serialize(this, value))
			NazaraError("Failed to serialize value");

		return *this;
	}
}
