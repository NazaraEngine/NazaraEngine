// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Stream.hpp>
#include <Nazara/Core/Debug.hpp>

NzStream::~NzStream() = default;

NzString NzStream::GetDirectory() const
{
	return NzString();
}

NzString NzStream::GetPath() const
{
	return NzString();
}

unsigned int NzStream::GetStreamOptions() const
{
	return m_streamOptions;
}

void NzStream::SetStreamOptions(unsigned int options)
{
	m_streamOptions = options;
}
