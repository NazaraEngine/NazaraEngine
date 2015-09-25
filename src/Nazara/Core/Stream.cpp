// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Stream.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	Stream::~Stream() = default;

	String Stream::GetDirectory() const
	{
		return String();
	}

	String Stream::GetPath() const
	{
		return String();
	}

	unsigned int Stream::GetStreamOptions() const
	{
		return m_streamOptions;
	}

	void Stream::SetStreamOptions(unsigned int options)
	{
		m_streamOptions = options;
	}
}
