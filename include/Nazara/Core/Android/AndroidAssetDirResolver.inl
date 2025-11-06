// Copyright (C) 2025 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	inline AndroidAssetDirResolver::AndroidAssetDirResolver(AAssetManager* manager, std::string basePath, AAssetDir* dir) :
	m_basePath(std::move(basePath)),
	m_assetDir(dir),
	m_manager(manager)
	{
	}
}
