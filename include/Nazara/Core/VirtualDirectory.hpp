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
	class VirtualDirectory;

	using VirtualDirectoryPtr = std::shared_ptr<VirtualDirectory>;

	class VirtualDirectory : public std::enable_shared_from_this<VirtualDirectory>
	{
		public:
			struct DataPointerEntry;
			struct DirectoryEntry;
			struct FileContentEntry;
			struct PhysicalDirectoryEntry;
			struct PhysicalFileEntry;
			struct VirtualDirectoryEntry;

			using Entry = std::variant<DataPointerEntry, FileContentEntry, PhysicalDirectoryEntry, PhysicalFileEntry, VirtualDirectoryEntry>;

			inline VirtualDirectory(std::weak_ptr<VirtualDirectory> parentDirectory = {});
			inline VirtualDirectory(std::filesystem::path physicalPath, std::weak_ptr<VirtualDirectory> parentDirectory = {});
			VirtualDirectory(const VirtualDirectory&) = delete;
			VirtualDirectory(VirtualDirectory&&) = delete;
			~VirtualDirectory() = default;

			inline void AllowUproot(bool uproot = true);

			inline bool Exists(std::string_view path);

			template<typename F> void Foreach(F&& callback, bool includeDots = false);

			template<typename F> bool GetDirectoryEntry(std::string_view path, F&& callback);
			template<typename F> bool GetEntry(std::string_view path, F&& callback);
			template<typename F> bool GetFileContent(std::string_view path, F&& callback);

			inline bool IsUprootAllowed() const;

			inline VirtualDirectoryEntry& StoreDirectory(std::string_view path, VirtualDirectoryPtr directory);
			inline PhysicalDirectoryEntry& StoreDirectory(std::string_view path, std::filesystem::path directoryPath);
			inline FileContentEntry& StoreFile(std::string_view path, std::vector<UInt8> file);
			inline PhysicalFileEntry& StoreFile(std::string_view path, std::filesystem::path filePath);
			inline DataPointerEntry& StoreFile(std::string_view path, const void* data, std::size_t size);

			VirtualDirectory& operator=(const VirtualDirectory&) = delete;
			VirtualDirectory& operator=(VirtualDirectory&&) = delete;

			// File entries
			struct DataPointerEntry
			{
				const void* data;
				std::size_t size;
			};

			struct FileContentEntry
			{
				std::vector<UInt8> data;
			};

			struct PhysicalFileEntry
			{
				std::filesystem::path filePath;
			};

			// Directory entries
			struct DirectoryEntry
			{
				VirtualDirectoryPtr directory;
			};

			struct PhysicalDirectoryEntry : DirectoryEntry
			{
				std::filesystem::path filePath;
			};

			struct VirtualDirectoryEntry : DirectoryEntry
			{
			};

		private:
			template<typename F> bool GetEntryInternal(std::string_view name, F&& callback);
			inline bool CreateOrRetrieveDirectory(std::string_view path, std::shared_ptr<VirtualDirectory>& directory, std::string_view& entryName);

			template<typename T> T& StoreInternal(std::string name, T value);

			template<typename F, typename... Args> static bool CallbackReturn(F&& callback, Args&&... args);
			template<typename F1, typename F2> static bool SplitPath(std::string_view path, F1&& dirCB, F2&& fileCB);

			struct ContentEntry
			{
				std::string name;
				Entry entry;
			};

			std::optional<std::filesystem::path> m_physicalPath;
			std::vector<ContentEntry> m_content;
			std::weak_ptr<VirtualDirectory> m_parent;
			bool m_isUprootAllowed;
	};
}

#include <Nazara/Core/VirtualDirectory.inl>

#endif // NAZARA_CORE_VIRTUALDIRECTORY_HPP
