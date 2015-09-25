// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/ByteArray.hpp>
#include <Nazara/Core/AbstractHash.hpp>
#include <ostream>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	std::ostream& operator<<(std::ostream& out, const ByteArray& byteArray)
	{
		out << byteArray.ToString();
		return out;
	}

	bool ByteArray::FillHash(AbstractHash* hash) const
	{
		hash->Append(GetConstBuffer(), GetSize());

		return true;
	}
}
