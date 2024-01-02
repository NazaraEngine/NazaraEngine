// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Audio/Debug.hpp>

namespace Nz
{
	inline OpenALLibrary::OpenALLibrary() :
	m_hasCaptureSupport(false),
	m_isLoaded(false)
	{
	}

	inline OpenALLibrary::~OpenALLibrary()
	{
		Unload();
	}

	inline bool OpenALLibrary::IsLoaded() const
	{
		return m_isLoaded;
	}
}

#include <Nazara/Audio/DebugOff.hpp>
