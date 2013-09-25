// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Debug.hpp>

NzErrorFlags::NzErrorFlags(nzUInt32 flags, bool replace) :
m_previousFlags(NzError::GetFlags())
{
	SetFlags(flags, replace);
}

NzErrorFlags::~NzErrorFlags()
{
	NzError::SetFlags(m_previousFlags);
}

nzUInt32 NzErrorFlags::GetPreviousFlags() const
{
	return m_previousFlags;
}

void NzErrorFlags::SetFlags(nzUInt32 flags, bool replace)
{
	if (!replace)
		flags |= m_previousFlags;

	NzError::SetFlags(flags);
}
