// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Export.hpp


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
