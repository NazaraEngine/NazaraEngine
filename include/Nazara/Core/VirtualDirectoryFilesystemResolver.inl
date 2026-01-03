// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	inline VirtualDirectoryFilesystemResolver::VirtualDirectoryFilesystemResolver(std::filesystem::path physicalPath, OpenModeFlags fileOpenMode) :
	m_physicalPath(std::move(physicalPath)),
	m_fileOpenMode(fileOpenMode)
	{
	}
}
