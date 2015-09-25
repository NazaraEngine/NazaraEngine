// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	Resource::~Resource() = default;

	const String& Resource::GetFilePath() const
	{
		return m_filePath;
	}

	void Resource::SetFilePath(const String& filePath)
	{
		m_filePath = filePath;
	}
}
