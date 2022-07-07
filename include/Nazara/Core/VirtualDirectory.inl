// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/VirtualDirectory.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <algorithm>
#include <cassert>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	inline VirtualDirectory::VirtualDirectory(std::weak_ptr<VirtualDirectory> parentDirectory) :
	m_parent(std::move(parentDirectory)),
	m_isUprootAllowed(false)
	{
	}

	inline VirtualDirectory::VirtualDirectory(std::filesystem::path physicalPath, std::weak_ptr<VirtualDirectory> parentDirectory) :
	m_physicalPath(std::move(physicalPath)),
	m_parent(std::move(parentDirectory)),
	m_isUprootAllowed(false)
	{
	}

	inline void VirtualDirectory::AllowUproot(bool uproot)
	{
		m_isUprootAllowed = uproot;
	}

	inline bool VirtualDirectory::Exists(std::string_view path)
	{
		return GetEntry(path, [](const auto&) {});
	}

	template<typename F>
	void VirtualDirectory::Foreach(F&& callback, bool includeDots)
	{
		if (includeDots)
		{
			Entry ourselves = VirtualDirectoryEntry{ shared_from_this() };
			callback(std::string_view("."), ourselves);
			if (VirtualDirectoryPtr parent = m_parent.lock())
			{
				Entry parentEntry = VirtualDirectoryEntry{ parent };
				if (!CallbackReturn(callback, std::string_view(".."), parentEntry))
					return;
			}
			else if (!CallbackReturn(callback, std::string_view(".."), ourselves))
					return;
		}

		for (auto&& entry : m_content)
		{
			if (!CallbackReturn(callback, std::string_view(entry.name), std::as_const(entry.entry)))
				return;
		}

		if (m_physicalPath)
		{
			for (auto&& physicalEntry : std::filesystem::directory_iterator(*m_physicalPath))
			{
				std::string filename = physicalEntry.path().filename().generic_u8string();

				// Check if physical file/directory has been overridden by a virtual one
				auto it = std::lower_bound(m_content.begin(), m_content.end(), filename, [](const ContentEntry& entry, std::string_view name)
				{
					return entry.name < name;
				});
				if (it != m_content.end() && it->name == filename)
					continue;

				std::filesystem::file_status status = physicalEntry.status();

				Entry entry;
				if (std::filesystem::is_regular_file(status))
					entry = PhysicalFileEntry{ physicalEntry.path() };
				else if (std::filesystem::is_directory(status))
				{
					VirtualDirectoryPtr virtualDir = std::make_shared<VirtualDirectory>(physicalEntry.path(), weak_from_this());
					entry = PhysicalDirectoryEntry{ std::move(virtualDir), physicalEntry.path() };
				}
				else
					continue;

				if (!CallbackReturn(callback, std::string_view(filename), entry))
					return;
			}
		}
	}
	
	template<typename F>
	bool VirtualDirectory::GetDirectoryEntry(std::string_view path, F&& callback)
	{
		return GetEntry(path, [&](const Entry& entry)
		{
			return std::visit([&](auto&& entry)
			{
				using T = std::decay_t<decltype(entry)>;

				if constexpr (std::is_same_v<T, VirtualDirectoryEntry> || std::is_same_v<T, PhysicalDirectoryEntry>)
				{
					return CallbackReturn(callback, static_cast<const DirectoryEntry&>(entry));
				}
				else if constexpr (std::is_same_v<T, DataPointerEntry> || std::is_same_v<T, FileContentEntry> || std::is_same_v<T, PhysicalFileEntry>)
				{
					NazaraError("entry is a file");
					return false;
				}
				else
					static_assert(AlwaysFalse<T>(), "incomplete visitor");
			}, entry);
		});
	}

	template<typename F> bool VirtualDirectory::GetEntry(std::string_view path, F&& callback)
	{
		assert(!path.empty());

		VirtualDirectoryPtr currentDir = shared_from_this();
		std::optional<std::filesystem::path> physicalPathBase;
		std::vector<std::string> physicalDirectoryParts;
		return SplitPath(path, [&](std::string_view dirName)
		{
			assert(!dirName.empty());

			if (physicalPathBase)
			{
				// Special case when traversing directory
				if (dirName == ".." && !m_isUprootAllowed)
				{
					// Don't allow to escape virtual directory
					if (!physicalDirectoryParts.empty())
						physicalDirectoryParts.pop_back();
					else
						physicalPathBase.reset();
				}
				else if (dirName != ".")
					physicalDirectoryParts.emplace_back(dirName);

				return true;
			}

			return currentDir->GetEntryInternal(dirName, [&](const Entry& entry)
			{
				if (auto dirEntry = std::get_if<VirtualDirectoryEntry>(&entry))
				{
					currentDir = dirEntry->directory;
					return true;
				}
				else if (auto physDirEntry = std::get_if<PhysicalDirectoryEntry>(&entry))
				{
					assert(!physicalPathBase);

					// We're traversing a physical directory
					physicalPathBase = physDirEntry->filePath;
					return true;
				}

				return false;
			});
		}, 
		[&](std::string_view name)
		{
			if (physicalPathBase)
			{
				std::filesystem::path filePath = *physicalPathBase;
				for (const auto& part : physicalDirectoryParts)
					filePath /= part;

				filePath /= name;

				std::filesystem::file_status status = std::filesystem::status(filePath); //< FIXME: This will follow symlink, is this the intended behavior? (see symlink_status)

				Entry entry;
				if (std::filesystem::is_regular_file(status))
					entry = PhysicalFileEntry{ std::move(filePath) };
				else if (std::filesystem::is_directory(status))
				{
					VirtualDirectoryPtr virtualDir = std::make_shared<VirtualDirectory>(filePath, weak_from_this());
					entry = PhysicalDirectoryEntry{ std::move(virtualDir), std::move(filePath) };
				}
				else
					return false; //< either not known or of a special type

				return CallbackReturn(callback, entry);
			}
			else
				return currentDir->GetEntryInternal(name, callback);
		});
	}

	template<typename F>
	bool VirtualDirectory::GetFileContent(std::string_view path, F&& callback)
	{
		return GetEntry(path, [&](const Entry& entry)
		{
			return std::visit([&](auto&& entry)
			{
				using T = std::decay_t<decltype(entry)>;

				using P1 = const void*;
				using P2 = std::size_t;

				if constexpr (std::is_same_v<T, DataPointerEntry>)
				{
					return CallbackReturn(callback, static_cast<P1>(entry.data), SafeCast<P2>(entry.size));
				}
				else if constexpr (std::is_same_v<T, FileContentEntry>)
				{
					return CallbackReturn(callback, static_cast<P1>(entry.data.data()), SafeCast<P2>(entry.data.size()));
				}
				else if constexpr (std::is_same_v<T, PhysicalFileEntry>)
				{
					std::optional<std::vector<UInt8>> source = File::ReadWhole(entry.filePath);
					if (!source.has_value())
						return false;

					return CallbackReturn(callback, static_cast<P1>(source->data()), SafeCast<P2>(source->size()));
				}
				else if constexpr (std::is_same_v<T, VirtualDirectoryEntry> || std::is_same_v<T, PhysicalDirectoryEntry>)
				{
					NazaraError("entry is a directory");
					return false;
				}
				else
					static_assert(AlwaysFalse<T>(), "incomplete visitor");
			}, entry);
		});
	}

	inline bool VirtualDirectory::IsUprootAllowed() const
	{
		return m_isUprootAllowed;
	}

	inline auto VirtualDirectory::StoreDirectory(std::string_view path, VirtualDirectoryPtr directory) -> VirtualDirectoryEntry&
	{
		assert(!path.empty());

		std::shared_ptr<VirtualDirectory> dir;
		std::string_view entryName;
		if (!CreateOrRetrieveDirectory(path, dir, entryName))
			throw std::runtime_error("invalid path");

		if (entryName == "." || entryName == "..")
			throw std::runtime_error("invalid entry name");

		return dir->StoreInternal(std::string(entryName), VirtualDirectoryEntry{ std::move(directory) });
	}

	inline auto VirtualDirectory::StoreDirectory(std::string_view path, std::filesystem::path directoryPath) -> PhysicalDirectoryEntry&
	{
		assert(!path.empty());

		std::shared_ptr<VirtualDirectory> dir;
		std::string_view entryName;
		if (!CreateOrRetrieveDirectory(path, dir, entryName))
			throw std::runtime_error("invalid path");

		if (entryName == "." || entryName == "..")
			throw std::runtime_error("invalid entry name");

		PhysicalDirectoryEntry entry;
		entry.directory = std::make_shared<VirtualDirectory>(directoryPath, dir);
		entry.filePath = std::move(directoryPath);

		return dir->StoreInternal(std::string(entryName), std::move(entry));
	}

	inline auto VirtualDirectory::StoreFile(std::string_view path, std::vector<UInt8> file) -> FileContentEntry&
	{
		assert(!path.empty());

		std::shared_ptr<VirtualDirectory> dir;
		std::string_view entryName;
		if (!CreateOrRetrieveDirectory(path, dir, entryName))
			throw std::runtime_error("invalid path");

		if (entryName == "." || entryName == "..")
			throw std::runtime_error("invalid entry name");

		return dir->StoreInternal(std::string(entryName), FileContentEntry{ std::move(file) });
	}

	inline auto VirtualDirectory::StoreFile(std::string_view path, std::filesystem::path filePath) -> PhysicalFileEntry&
	{
		assert(!path.empty());

		std::shared_ptr<VirtualDirectory> dir;
		std::string_view entryName;
		if (!CreateOrRetrieveDirectory(path, dir, entryName))
			throw std::runtime_error("invalid path");

		if (entryName == "." || entryName == "..")
			throw std::runtime_error("invalid entry name");

		return dir->StoreInternal(std::string(entryName), PhysicalFileEntry{ std::move(filePath) });
	}

	inline auto VirtualDirectory::StoreFile(std::string_view path, const void* data, std::size_t size) -> DataPointerEntry&
	{
		assert(!path.empty());

		std::shared_ptr<VirtualDirectory> dir;
		std::string_view entryName;
		if (!CreateOrRetrieveDirectory(path, dir, entryName))
			throw std::runtime_error("invalid path");

		if (entryName == "." || entryName == "..")
			throw std::runtime_error("invalid entry name");

		return dir->StoreInternal(std::string(entryName), DataPointerEntry{ data, size });
	}
	
	template<typename F> bool VirtualDirectory::GetEntryInternal(std::string_view name, F&& callback)
	{
		if (name == ".")
		{
			Entry entry{ VirtualDirectoryEntry{ shared_from_this() } };
			return CallbackReturn(callback, entry);
		}

		if (name == "..")
		{
			VirtualDirectoryPtr parentEntry;
			if (VirtualDirectoryPtr parent = m_parent.lock())
				parentEntry = std::move(parent);
			else if (!m_isUprootAllowed)
				parentEntry = shared_from_this();

			if (parentEntry)
			{
				Entry entry = VirtualDirectoryEntry{ std::move(parentEntry) };
				return CallbackReturn(callback, entry);
			}
		}

		auto it = std::lower_bound(m_content.begin(), m_content.end(), name, [](const ContentEntry& entry, std::string_view name)
		{
			return entry.name < name;
		});
		if (it == m_content.end() || it->name != name)
		{
			// Virtual file not found, check if it has a physical one
			if (m_physicalPath)
			{
				std::filesystem::path filePath = *m_physicalPath / name;
				std::filesystem::file_status status = std::filesystem::status(filePath); //< FIXME: This will follow symlink, is this the intended behavior? (see symlink_status)

				Entry entry;
				if (std::filesystem::is_regular_file(status))
					entry = PhysicalFileEntry{ std::move(filePath) };
				else if (std::filesystem::is_directory(status))
				{
					VirtualDirectoryPtr virtualDir = std::make_shared<VirtualDirectory>(filePath, weak_from_this());
					entry = PhysicalDirectoryEntry{ std::move(virtualDir), std::move(filePath) };
				}
				else
					return false; //< either not known or of a special type

				return CallbackReturn(callback, entry);
			}

			return false;
		}

		return CallbackReturn(callback, it->entry);
	}

	inline bool VirtualDirectory::CreateOrRetrieveDirectory(std::string_view path, std::shared_ptr<VirtualDirectory>& directory, std::string_view& entryName)
{
		directory = shared_from_this();

		bool allowCreation = true;
		return SplitPath(path, [&](std::string_view dirName)
		{
			assert(!dirName.empty());

			bool dirFound = directory->GetEntryInternal(dirName, [&](const Entry& entry)
			{
				if (auto dirEntry = std::get_if<VirtualDirectoryEntry>(&entry))
					directory = dirEntry->directory;
				else
					allowCreation = false; //< does exist but is not a directory
			});

			if (dirFound)
				return true;

			// Try to create a new directory
			if (!allowCreation)
				return false;

			auto newDirectory = std::make_shared<VirtualDirectory>(directory);
			directory->StoreDirectory(dirName, newDirectory);

			directory = std::move(newDirectory);
			return true;
		}, 
		[&](std::string_view name)
		{
			if (name.empty())
				return false;

			entryName = name;
			return true;
		});
	}

	template<typename T>
	T& VirtualDirectory::StoreInternal(std::string name, T value)
	{
		assert(!name.empty());

		auto it = std::lower_bound(m_content.begin(), m_content.end(), name, [](const ContentEntry& entry, std::string_view name)
		{
			return entry.name < name;
		});

		ContentEntry* entryPtr;
		if (it == m_content.end() || it->name != name)
			entryPtr = &*m_content.emplace(it);
		else
			entryPtr = &*it;

		entryPtr->entry = std::move(value);
		entryPtr->name = std::move(name);

		return std::get<T>(entryPtr->entry);
	}

	template<typename F, typename... Args>
	bool VirtualDirectory::CallbackReturn(F&& callback, Args&& ...args)
	{
		using Ret = decltype(callback(std::forward<Args>(args)...));
		if constexpr (std::is_void_v<Ret>)
		{
			callback(std::forward<Args>(args)...);
			return true;
		}
		else
		{
			static_assert(std::is_same_v<Ret, bool>, "callback must either return a boolean or nothing");
			return callback(std::forward<Args>(args)...);
		}
	}

	template<typename F1, typename F2>
	bool VirtualDirectory::SplitPath(std::string_view path, F1&& dirCB, F2&& lastCB)
	{
		std::string_view nextPart;
		auto HandlePart = [&](std::string_view part)
		{
			if (part.empty())
				return true; //< "a//b" == "a/b"

			if (!nextPart.empty() && !CallbackReturn(dirCB, nextPart))
				return false;

			nextPart = part;
			return true;
		};

		return SplitStringAny(path, R"(\/:)", HandlePart) && CallbackReturn(lastCB, nextPart);
	}
}

#include <Nazara/Core/DebugOff.hpp>
