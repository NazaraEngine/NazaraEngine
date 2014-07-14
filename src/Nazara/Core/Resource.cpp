// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/Debug.hpp>

NzResource::~NzResource() = default;

NzString NzResource::GetFilePath() const
{
	return m_filePath;
}

void NzResource::SetFilePath(const NzString& filePath)
{
	m_filePath = filePath;
}
