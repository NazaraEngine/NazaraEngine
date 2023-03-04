// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Android/AndroidAssetDirResolver.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	inline AndroidAssetDirResolver::AndroidAssetDirResolver(AAssetManager* manager, std::string basePath, AAssetDir* dir) :
	m_basePath(std::move(basePath)),
	m_assetDir(dir),
	m_manager(manager)
	{
	}
}

#include <Nazara/Core/DebugOff.hpp>
