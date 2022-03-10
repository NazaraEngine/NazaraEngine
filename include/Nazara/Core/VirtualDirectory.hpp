// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_VIRTUALDIRECTORY_HPP
#define NAZARA_CORE_VIRTUALDIRECTORY_HPP

#include <Nazara/Prerequisites.hpp>
#include <filesystem>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace Nz
{
	class VirtualDirectory : public std::enable_shared_from_this<VirtualDirectory>
	{
		public:
			struct DataPointerEntry;
			struct FileContentEntry;
			using PhysicalFileEntry = std::filesystem::path;
			using VirtualDirectoryEntry = std::shared_ptr<VirtualDirectory>;

			using Entry = std::variant<DataPointerEntry, FileContentEntry, PhysicalFileEntry, VirtualDirectoryEntry>;

			inline VirtualDirectory(VirtualDirectoryEntry parentDirectory = nullptr);
			inline VirtualDirectory(std::filesystem::path physicalPath, VirtualDirectoryEntry parentDirectory = nullptr);
			VirtualDirectory(const VirtualDirectory&) = delete;
			VirtualDirectory(VirtualDirectory&&) = delete;
			~VirtualDirectory() = default;

			template<typename F> void Foreach(F&& cb, bool includeDots = false);

			inline bool GetEntry(std::string_view path, Entry* entry);

			inline VirtualDirectoryEntry& StoreDirectory(std::string_view path, VirtualDirectoryEntry directory);
			inline VirtualDirectoryEntry& StoreDirectory(std::string_view path, std::filesystem::path directoryPath);
			inline FileContentEntry& StoreFile(std::string_view path, std::vector<UInt8> file);
			inline PhysicalFileEntry& StoreFile(std::string_view path, std::filesystem::path filePath);
			inline DataPointerEntry& StoreFile(std::string_view path, const void* data, std::size_t size);

			VirtualDirectory& operator=(const VirtualDirectory&) = delete;
			VirtualDirectory& operator=(VirtualDirectory&&) = delete;

			struct DataPointerEntry
			{
				const void* data;
				std::size_t size;
			};

			struct FileContentEntry
			{
				std::shared_ptr<std::vector<UInt8>> data;
			};

		private:
			inline void EnsureDots();
			inline bool GetEntryInternal(std::string_view name, Entry* entry);
			inline bool RetrieveDirectory(std::string_view path, bool allowCreation, std::shared_ptr<VirtualDirectory>& directory, std::string_view& entryName);

			inline VirtualDirectoryEntry& StoreDirectoryInternal(std::string name, std::filesystem::path directoryPath);
			inline VirtualDirectoryEntry& StoreDirectoryInternal(std::string name, VirtualDirectoryEntry directory);
			inline FileContentEntry& StoreFileInternal(std::string name, std::vector<UInt8> fileContent);
			inline PhysicalFileEntry& StoreFileInternal(std::string name, std::filesystem::path filePath);
			inline DataPointerEntry& StoreFileInternal(std::string name, const void* data, std::size_t size);

			template<typename F1, typename F2> static bool SplitPath(std::string_view path, F1&& dirCB, F2&& fileCB);

			std::map<std::string /*name*/, Entry, std::less<>> m_content;
			std::optional<std::filesystem::path> m_physicalPath;
			VirtualDirectoryEntry m_parent;
			bool m_wereDotRegistered;
	};
}

#include <Nazara/Core/VirtualDirectory.inl>

#endif // NAZARA_CORE_VIRTUALDIRECTORY_HPP
