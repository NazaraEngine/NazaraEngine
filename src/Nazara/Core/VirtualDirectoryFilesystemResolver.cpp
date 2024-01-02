// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/VirtualDirectoryFilesystemResolver.hpp>
#include <NazaraUtils/PathUtils.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	void VirtualDirectoryFilesystemResolver::ForEach(std::weak_ptr<VirtualDirectory> parent, FunctionRef<bool(std::string_view name, VirtualDirectory::Entry&& entry)> callback) const
	{
		for (auto&& physicalEntry : std::filesystem::directory_iterator(m_physicalPath))
		{
			std::string filename = PathToString(physicalEntry.path().filename());

			std::filesystem::file_status status = physicalEntry.status();

			VirtualDirectory::Entry entry;
			if (physicalEntry.is_regular_file())
			{
				if (!callback(filename, VirtualDirectory::FileEntry{ std::make_shared<File>(physicalEntry.path(), m_fileOpenMode) }))
					return;
			}
			else if (physicalEntry.is_directory())
			{
				VirtualDirectoryPtr virtualDir = std::make_shared<VirtualDirectory>(std::make_shared<VirtualDirectoryFilesystemResolver>(physicalEntry.path()), parent);
				if (!callback(filename, VirtualDirectory::DirectoryEntry{ { std::move(virtualDir) } }))
					return;
			}
		}
	}

	std::optional<VirtualDirectory::Entry> VirtualDirectoryFilesystemResolver::Resolve(std::weak_ptr<VirtualDirectory> parent, const std::string_view* parts, std::size_t partCount) const
	{
		std::filesystem::path filePath = m_physicalPath;
		for (std::size_t i = 0; i < partCount; ++i)
			filePath /= Utf8Path(parts[i]);

		std::filesystem::file_status status = std::filesystem::status(filePath); //< FIXME: This will follow symlink, is this the intended behavior? (see symlink_status)

		VirtualDirectory::Entry entry;
		if (std::filesystem::is_regular_file(status))
			return VirtualDirectory::FileEntry{ std::make_shared<File>(std::move(filePath), m_fileOpenMode) };
		else if (std::filesystem::is_directory(status))
		{
			VirtualDirectoryPtr virtualDir = std::make_shared<VirtualDirectory>(std::make_shared<VirtualDirectoryFilesystemResolver>(std::move(filePath)), parent);
			return VirtualDirectory::DirectoryEntry{ { std::move(virtualDir) } };
		}
		else
			return std::nullopt; //< either not known or of a special type
	}
}
