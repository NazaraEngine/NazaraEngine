// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/FilesystemAppComponent.hpp>
#include <Nazara/Core/VirtualDirectoryFilesystemResolver.hpp>

namespace Nz
{
	const VirtualDirectoryPtr& FilesystemAppComponent::Mount(std::string_view dirPath, std::filesystem::path filepath)
	{
		return Mount(dirPath, std::make_shared<VirtualDirectory>(std::make_shared<VirtualDirectoryFilesystemResolver>(std::move(filepath))));
	}

	const VirtualDirectoryPtr& FilesystemAppComponent::Mount(std::string_view dirPath, VirtualDirectoryPtr directory)
	{
		if (dirPath.empty())
		{
			m_rootDirectory = std::move(directory);
			return m_rootDirectory;
		}

		if (!m_rootDirectory)
			m_rootDirectory = std::make_shared<VirtualDirectory>();

		return m_rootDirectory->StoreDirectory(dirPath, std::move(directory)).directory;
	}

	void FilesystemAppComponent::MountDefaultDirectories()
	{
		m_rootDirectory = std::make_shared<VirtualDirectory>(std::make_shared<VirtualDirectoryFilesystemResolver>(std::filesystem::current_path()));
	}
}
