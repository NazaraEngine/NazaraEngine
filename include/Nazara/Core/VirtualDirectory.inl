// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/VirtualDirectory.hpp>
#include <cassert>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	inline VirtualDirectory::VirtualDirectory(std::weak_ptr<VirtualDirectory> parentDirectory) :
	m_parent(std::move(parentDirectory))
	{
	}

	inline VirtualDirectory::VirtualDirectory(std::filesystem::path physicalPath, std::weak_ptr<VirtualDirectory> parentDirectory) :
	m_physicalPath(std::move(physicalPath)),
	m_parent(std::move(parentDirectory))
	{
	}

	template<typename F>
	void VirtualDirectory::Foreach(F&& callback, bool includeDots)
	{
		if (includeDots)
		{
			Entry ourselves = DirectoryEntry{ shared_from_this() };
			callback(std::string_view("."), ourselves);
			if (VirtualDirectoryPtr parent = m_parent.lock())
			{
				Entry parentEntry = DirectoryEntry{ parent };
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
					entry = PhysicalDirectoryEntry{ physicalEntry.path() };
				else
					continue;

				if (!CallbackReturn(callback, std::string_view(filename), entry))
					return;
			}
		}
	}
	
	template<typename F> bool VirtualDirectory::GetEntry(std::string_view path, F&& callback)
	{
		VirtualDirectoryPtr currentDir = shared_from_this();
		std::vector<std::string> physicalDirectoryParts;
		return SplitPath(path, [&](std::string_view dirName)
		{
			if (!physicalDirectoryParts.empty())
			{
				// Special case when traversing directory
				if (dirName == "..")
				{
					// Don't allow to escape virtual directory
					if (!physicalDirectoryParts.empty())
						physicalDirectoryParts.pop_back();
				}
				else if (dirName != ".")
					physicalDirectoryParts.emplace_back(dirName);

				return true;
			}

			return currentDir->GetEntryInternal(dirName, [&](const Entry& entry)
			{
				if (auto dirEntry = std::get_if<DirectoryEntry>(&entry))
				{
					currentDir = dirEntry->directory;
					return true;
				}
				else if (auto physDirEntry = std::get_if<PhysicalDirectoryEntry>(&entry))
				{
					// We're traversing a physical directory
					physicalDirectoryParts.emplace_back(dirName);
					return true;
				}

				return false;
			});
		}, 
		[&](std::string_view name)
		{
			if (!physicalDirectoryParts.empty())
			{
				if (physicalDirectoryParts.empty() && (name == "." || name == ".."))
				{
					// Prevent escaping the virtual directory
					Entry ourselves = DirectoryEntry{ shared_from_this() };
					return CallbackReturn(callback, ourselves);
				}

				assert(m_physicalPath);
				std::filesystem::path filePath = *m_physicalPath;
				for (const auto& part : physicalDirectoryParts)
					filePath /= part;

				filePath /= name;

				std::filesystem::file_status status = std::filesystem::status(filePath); //< FIXME: This will follow symlink, is this the intended behavior? (see symlink_status)

				Entry entry;
				if (std::filesystem::is_regular_file(status))
					entry = PhysicalFileEntry{ std::move(filePath) };
				else if (std::filesystem::is_directory(status))
					entry = PhysicalDirectoryEntry{ std::move(filePath) };
				else
					return false; //< either not known or of a special type

				return CallbackReturn(callback, entry);
			}
			else
				return currentDir->GetEntryInternal(name, callback);
		});
	}

	inline auto VirtualDirectory::StoreDirectory(std::string_view path, VirtualDirectoryPtr directory) -> DirectoryEntry&
	{
		std::shared_ptr<VirtualDirectory> dir;
		std::string_view entryName;
		if (!RetrieveDirectory(path, true, dir, entryName))
			throw std::runtime_error("invalid path");

		return dir->StoreInternal(std::string(entryName), DirectoryEntry{ std::move(directory) });
	}

	inline auto VirtualDirectory::StoreDirectory(std::string_view path, std::filesystem::path directoryPath) -> PhysicalDirectoryEntry&
	{
		std::shared_ptr<VirtualDirectory> dir;
		std::string_view entryName;
		if (!RetrieveDirectory(path, true, dir, entryName))
			throw std::runtime_error("invalid path");

		return dir->StoreInternal(std::string(entryName), PhysicalDirectoryEntry{ std::move(directoryPath) });
	}

	inline auto VirtualDirectory::StoreFile(std::string_view path, std::vector<UInt8> file) -> FileContentEntry&
	{
		std::shared_ptr<VirtualDirectory> dir;
		std::string_view entryName;
		if (!RetrieveDirectory(path, true, dir, entryName))
			throw std::runtime_error("invalid path");

		return dir->StoreInternal(std::string(entryName), FileContentEntry{ std::move(file) });
	}

	inline auto VirtualDirectory::StoreFile(std::string_view path, std::filesystem::path filePath) -> PhysicalFileEntry&
	{
		std::shared_ptr<VirtualDirectory> dir;
		std::string_view entryName;
		if (!RetrieveDirectory(path, true, dir, entryName))
			throw std::runtime_error("invalid path");

		return dir->StoreInternal(std::string(entryName), PhysicalFileEntry{ std::move(filePath) });
	}

	inline auto VirtualDirectory::StoreFile(std::string_view path, const void* data, std::size_t size) -> DataPointerEntry&
	{
		std::shared_ptr<VirtualDirectory> dir;
		std::string_view entryName;
		if (!RetrieveDirectory(path, true, dir, entryName))
			throw std::runtime_error("invalid path");

		return dir->StoreInternal(std::string(entryName), DataPointerEntry{ data, size });
	}
	
	template<typename F> bool VirtualDirectory::GetEntryInternal(std::string_view name, F&& callback)
	{
		if (name == ".")
		{
			Entry entry{ DirectoryEntry{ shared_from_this() } };
			return CallbackReturn(callback, entry);
		}
		else if (name == "..")
		{
			Entry entry;
			if (VirtualDirectoryPtr parent = m_parent.lock())
				entry = DirectoryEntry{ std::move(parent) };
			else
				entry = DirectoryEntry{ shared_from_this() };

			return CallbackReturn(callback, entry);
		}
		else
		{
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
						entry = PhysicalDirectoryEntry{ std::move(filePath) };
					else
						return false; //< either not known or of a special type

					return CallbackReturn(callback, entry);
				}

				return false;
			}

			return CallbackReturn(callback, it->entry);
		}
	}

	inline bool VirtualDirectory::RetrieveDirectory(std::string_view path, bool allowCreation, std::shared_ptr<VirtualDirectory>& directory, std::string_view& entryName)
	{
		directory = shared_from_this();

		return SplitPath(path, [&](std::string_view dirName)
		{
			bool dirFound = directory->GetEntryInternal(dirName, [&](const Entry& entry)
			{
				if (auto dirEntry = std::get_if<DirectoryEntry>(&entry))
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
			entryName = name;
		});
	}

	template<typename T>
	T& VirtualDirectory::StoreInternal(std::string name, T value)
	{
		auto it = std::lower_bound(m_content.begin(), m_content.end(), name, [](const ContentEntry& entry, std::string_view name)
		{
			return entry.name < name;
		});

		ContentEntry* entryPtr;
		if (it == m_content.end() || it->name == name)
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
		std::size_t pos;
		while ((pos = path.find_first_of("\\/:")) != std::string::npos)
		{
			if (!dirCB(path.substr(0, pos)))
				return false;

			path = path.substr(pos + 1);
		}

		return CallbackReturn(lastCB, path);
	}
}

#include <Nazara/Core/DebugOff.hpp>
