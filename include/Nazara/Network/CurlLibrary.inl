// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/Debug.hpp>

namespace Nz
{
	inline CurlLibrary::CurlLibrary() :
	m_isInitialized(false)
	{
	}

	inline CurlLibrary::~CurlLibrary()
	{
		Unload();
	}

	inline bool CurlLibrary::IsLoaded() const
	{
		return m_isInitialized;
	}
}

#include <Nazara/Network/DebugOff.hpp>
