// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	ErrorFlags::ErrorFlags(UInt32 flags, bool replace) :
	m_previousFlags(Error::GetFlags())
	{
		SetFlags(flags, replace);
	}

	ErrorFlags::~ErrorFlags()
	{
		Error::SetFlags(m_previousFlags);
	}

	UInt32 ErrorFlags::GetPreviousFlags() const
	{
		return m_previousFlags;
	}

	void ErrorFlags::SetFlags(UInt32 flags, bool replace)
	{
		if (!replace)
			flags |= m_previousFlags;

		Error::SetFlags(flags);
	}
}
