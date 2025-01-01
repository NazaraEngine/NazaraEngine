// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_VIRTUALDIRECTORY_HPP
#define NAZARA_CORE_VIRTUALDIRECTORY_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Stream.hpp>
#include <NazaraUtils/FixedVector.hpp>
#include <NazaraUtils/FunctionRef.hpp>
#include <memory>
#include <optional>
#include <string>
#include <variant>

namespace Nz
{
	class VirtualDirectory;
	class VirtualDirectoryResolver;

	using VirtualDirectoryPtr = std::shared_ptr<VirtualDirectory>;

	class VirtualDirectory : public std::enable_shared_from_this<VirtualDirectory>
	{
		public:
			struct DirectoryEntry;
			struct FileEntry;

			using Entry = std::variant<DirectoryEntry, FileEntry>;

			inline VirtualDirectory(std::weak_ptr<VirtualDirectory> parentDirectory = {});
			inline VirtualDirectory(std::shared_ptr<VirtualDirectoryResolver> resolver, std::weak_ptr<VirtualDirectory> parentDirectory = {});
			VirtualDirectory(const VirtualDirectory&) = delete;
			VirtualDirectory(VirtualDirectory&&) = delete;
			~VirtualDirectory() = default;

			inline void AllowUproot(bool uproot = true);

			inline bool Exists(std::string_view path);

			template<typename F> void ForEach(F&& callback, bool includeDots = false);

			template<typename F> bool GetDirectoryEntry(std::string_view path, F&& callback);
			template<typename F> bool GetEntry(std::string_view path, F&& callback);
			template<typename F> bool GetFileContent(std::string_view path, F&& callback);
			template<typename F> bool GetFileEntry(std::string_view path, F&& callback);
			inline const std::shared_ptr<VirtualDirectoryResolver>& GetResolver() const;

			inline bool IsUprootAllowed() const;

			inline DirectoryEntry& StoreDirectory(std::string_view path, std::shared_ptr<VirtualDirectoryResolver> resolver);
			inline DirectoryEntry& StoreDirectory(std::string_view path, VirtualDirectoryPtr directory);
			inline FileEntry& StoreFile(std::string_view path, std::shared_ptr<Stream> stream);
			inline FileEntry& StoreFile(std::string_view path, ByteArray content);
			inline FileEntry& StoreFile(std::string_view path, const void* data, std::size_t size);

			VirtualDirectory& operator=(const VirtualDirectory&) = delete;
			VirtualDirectory& operator=(VirtualDirectory&&) = delete;

			// File entries
			struct FileEntry
			{
				std::shared_ptr<Stream> stream;
			};

			// Directory entries
			struct DirectoryEntry
			{
				VirtualDirectoryPtr directory;
			};

		private:
			template<typename F> bool GetEntryInternal(std::string_view name, bool allowResolve, F&& callback);
			inline bool CreateOrRetrieveDirectory(std::string_view path, std::shared_ptr<VirtualDirectory>& directory, std::string_view& entryName);

			template<typename T> T& StoreInternal(std::string name, T value);

			template<typename F, typename... Args> static bool CallbackReturn(F&& callback, Args&&... args);
			template<typename F1, typename F2> static bool SplitPath(std::string_view path, F1&& dirCB, F2&& fileCB);

			struct ContentEntry
			{
				std::string name;
				Entry entry;
			};

			HybridVector<ContentEntry, 3> m_content;
			std::shared_ptr<VirtualDirectoryResolver> m_resolver;
			std::weak_ptr<VirtualDirectory> m_parent;
			bool m_isUprootAllowed;
	};

	class NAZARA_CORE_API VirtualDirectoryResolver
	{
		public:
			VirtualDirectoryResolver() = default;
			VirtualDirectoryResolver(const VirtualDirectoryResolver&) = delete;
			VirtualDirectoryResolver(VirtualDirectoryResolver&&) = delete;
			virtual ~VirtualDirectoryResolver();

			virtual void ForEach(std::weak_ptr<VirtualDirectory> parent, FunctionRef<bool (std::string_view name, VirtualDirectory::Entry&& entry)> callback) const = 0;

			virtual std::optional<VirtualDirectory::Entry> Resolve(std::weak_ptr<VirtualDirectory> parent, const std::string_view* parts, std::size_t partCount) const = 0;

			VirtualDirectoryResolver& operator=(const VirtualDirectoryResolver&) = delete;
			VirtualDirectoryResolver& operator=(VirtualDirectoryResolver&&) = delete;
	};
}

#include <Nazara/Core/VirtualDirectory.inl>

#endif // NAZARA_CORE_VIRTUALDIRECTORY_HPP
