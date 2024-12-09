// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/File.hpp>
#include <Nazara/Core/MemoryView.hpp>
#include <Nazara/Core/OwnedMemoryStream.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <NazaraUtils/CallOnExit.hpp>
#include <NazaraUtils/FixedVector.hpp>
#include <algorithm>
#include <cassert>

namespace Nz
{
	inline VirtualDirectory::VirtualDirectory(std::weak_ptr<VirtualDirectory> parentDirectory) :
	m_parent(std::move(parentDirectory)),
	m_isUprootAllowed(false)
	{
	}

	inline VirtualDirectory::VirtualDirectory(std::shared_ptr<VirtualDirectoryResolver> resolver, std::weak_ptr<VirtualDirectory> parentDirectory) :
	m_resolver(std::move(resolver)),
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
	void VirtualDirectory::ForEach(F&& callback, bool includeDots)
	{
		if (includeDots)
		{
			Entry ourselves = DirectoryEntry{ shared_from_this() };
			callback(std::string_view("."), ourselves);
			if (VirtualDirectoryPtr parent = m_parent.lock())
			{
				Entry parentEntry = DirectoryEntry{ { parent } };
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

		if (m_resolver)
		{
			m_resolver->ForEach(weak_from_this(), [&](std::string_view filename, Entry&& entry)
			{
				// Check if this file/directory has been overridden by a virtual one
				auto it = std::lower_bound(m_content.begin(), m_content.end(), filename, [](const ContentEntry& entry, std::string_view name)
				{
					return entry.name < name;
				});
				if (it != m_content.end() && it->name == filename)
					return true; //< this was already returned

				return CallbackReturn(callback, filename, std::move(entry));
			});
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

				if constexpr (std::is_same_v<T, DirectoryEntry>)
					return CallbackReturn(callback, std::as_const(entry));
				else if constexpr (std::is_same_v<T, FileEntry>)
				{
					NazaraError("entry is a file");
					return false;
				}
				else
					static_assert(AlwaysFalse<T>(), "incomplete visitor");
			}, entry);
		});
	}

	template<typename F>
	bool VirtualDirectory::GetEntry(std::string_view path, F&& callback)
	{
		assert(!path.empty());

		VirtualDirectoryPtr currentDir = shared_from_this();

		HybridVector<std::string_view, 32> directoryParts;
		return SplitPath(path, [&](std::string_view dirName)
		{
			assert(!dirName.empty());

			// If we have directory parts, we're access this path across a resolver
			if (!directoryParts.empty())
			{
				// Special case when traversing directory
				if (dirName == ".." && !currentDir->IsUprootAllowed())
				{
					// Don't allow to escape virtual directory
					if (!directoryParts.empty())
						directoryParts.pop_back();
				}
				else if (dirName != ".")
					directoryParts.emplace_back(dirName);

				return true;
			}

			bool isFile = false;
			bool foundDir = currentDir->GetEntryInternal(dirName, false, [&](const Entry& entry)
			{
				if (auto dirEntry = std::get_if<DirectoryEntry>(&entry))
				{
					currentDir = dirEntry->directory;
					return true;
				}

				// not a directory
				isFile = true;
				return false;
			});

			if (foundDir)
				return true;

			if (!isFile && currentDir->GetResolver())
			{
				assert(directoryParts.empty());
				directoryParts.push_back(dirName);
				return true;
			}

			return false;
		},
		[&](std::string_view name)
		{
			if (!directoryParts.empty())
			{
				if (const auto& resolver = currentDir->GetResolver())
				{
					directoryParts.push_back(name);

					std::optional<Entry> entryOpt = resolver->Resolve({}, directoryParts.data(), directoryParts.size());
					if (!entryOpt)
						return false;

					return CallbackReturn(callback, *std::move(entryOpt));
				}
			}

			return currentDir->GetEntryInternal(name, true, callback);
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

				if constexpr (std::is_same_v<T, FileEntry>)
				{
					Stream& stream = *entry.stream;

					if (stream.IsMemoryMapped())
					{
						const void* ptr = stream.GetMappedPointer();
						UInt64 size = stream.GetSize();
						if (ptr && size > 0)
							return CallbackReturn(callback, ptr, size);
					}

					// Save and restore cursor position after the call
					std::size_t cursorPos = stream.GetCursorPos();
					CallOnExit restoreCursorPos([&] { stream.SetCursorPos(cursorPos); });

					stream.SetCursorPos(0);

					UInt64 fileSize = stream.GetSize();

					std::vector<UInt8> data;

					// Remember fileSize may be zero if file size isn't known in advance
					if (fileSize > 0)
					{
						data.resize(fileSize);
						if (stream.Read(&data[0], fileSize) != fileSize)
						{
							NazaraError("failed to read from stream");
							return false;
						}
					}
					else
					{
						// File size isn't know, read it block by block until the end of stream
						constexpr std::size_t blockSize = 4u * 1024;

						while (!stream.EndOfStream())
						{
							std::size_t offset = data.size();
							data.resize(offset + blockSize);
							if (std::size_t readSize = stream.Read(&data[offset], blockSize); readSize != blockSize)
							{
								if (stream.EndOfStream())
								{
									data.resize(offset + readSize);
									break;
								}
								else
								{
									NazaraError("failed to read from stream");
									return false;
								}
							}
						}
					}

					return CallbackReturn(callback, static_cast<const void*>(&data[0]), UInt64(data.size()));
				}
				else if constexpr (std::is_same_v<T, DirectoryEntry>)
				{
					NazaraError("entry is a directory");
					return false;
				}
				else
					static_assert(AlwaysFalse<T>(), "incomplete visitor");
			}, entry);
		});
	}

	template<typename F>
	bool VirtualDirectory::GetFileEntry(std::string_view path, F&& callback)
	{
		return GetEntry(path, [&](const Entry& entry)
		{
			return std::visit([&](auto&& entry)
			{
				using T = std::decay_t<decltype(entry)>;

				if constexpr (std::is_same_v<T, DirectoryEntry>)
				{
					NazaraError("entry is a directory");
					return false;
				}
				else if constexpr (std::is_same_v<T, FileEntry>)
					return CallbackReturn(callback, std::as_const(entry));
				else
					static_assert(AlwaysFalse<T>(), "incomplete visitor");
			}, entry);
		});
	}

	inline const std::shared_ptr<VirtualDirectoryResolver>& VirtualDirectory::GetResolver() const
	{
		return m_resolver;
	}

	inline bool VirtualDirectory::IsUprootAllowed() const
	{
		return m_isUprootAllowed;
	}

	inline auto VirtualDirectory::StoreDirectory(std::string_view path, std::shared_ptr<VirtualDirectoryResolver> resolver) -> DirectoryEntry&
	{
		assert(!path.empty());

		std::shared_ptr<VirtualDirectory> dir;
		std::string_view entryName;
		if (!CreateOrRetrieveDirectory(path, dir, entryName))
			throw std::runtime_error("invalid path");

		if (entryName == "." || entryName == "..")
			throw std::runtime_error("invalid entry name");

		DirectoryEntry entry;
		entry.directory = std::make_shared<VirtualDirectory>(std::move(resolver), dir);

		return dir->StoreInternal(std::string(entryName), std::move(entry));
	}

	inline auto VirtualDirectory::StoreDirectory(std::string_view path, VirtualDirectoryPtr directory) -> DirectoryEntry&
	{
		assert(!path.empty());

		std::shared_ptr<VirtualDirectory> dir;
		std::string_view entryName;
		if (!CreateOrRetrieveDirectory(path, dir, entryName))
			throw std::runtime_error("invalid path");

		if (entryName == "." || entryName == "..")
			throw std::runtime_error("invalid entry name");

		return dir->StoreInternal(std::string(entryName), DirectoryEntry{ { std::move(directory) } });
	}

	inline auto VirtualDirectory::StoreFile(std::string_view path, std::shared_ptr<Stream> stream) -> FileEntry&
	{
		assert(!path.empty());

		std::shared_ptr<VirtualDirectory> dir;
		std::string_view entryName;
		if (!CreateOrRetrieveDirectory(path, dir, entryName))
			throw std::runtime_error("invalid path");

		if (entryName == "." || entryName == "..")
			throw std::runtime_error("invalid entry name");

		return dir->StoreInternal(std::string(entryName), FileEntry{ std::move(stream) });
	}

	inline auto VirtualDirectory::StoreFile(std::string_view path, ByteArray content) -> FileEntry&
	{
		return StoreFile(path, std::make_shared<OwnedMemoryStream>(std::move(content)));
	}

	inline auto VirtualDirectory::StoreFile(std::string_view path, const void* data, std::size_t size) -> FileEntry&
	{
		return StoreFile(path, std::make_shared<MemoryView>(data, size));
	}

	template<typename F>
	bool VirtualDirectory::GetEntryInternal(std::string_view name, bool allowResolve, F&& callback)
	{
		if (name == ".")
		{
			Entry entry{ DirectoryEntry{ { shared_from_this() } } };
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
				Entry entry = DirectoryEntry{ { std::move(parentEntry) } };
				return CallbackReturn(callback, entry);
			}
		}

		auto it = std::lower_bound(m_content.begin(), m_content.end(), name, [](const ContentEntry& entry, std::string_view name)
		{
			return entry.name < name;
		});
		if (it == m_content.end() || it->name != name)
		{
			// Virtual file not found, check if it the custom resolver knows about it
			if (m_resolver && allowResolve)
			{
				std::optional<Entry> entryOpt = m_resolver->Resolve(weak_from_this(), &name, 1);
				if (!entryOpt)
					return false;

				return CallbackReturn(callback, *std::move(entryOpt));
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

			bool dirFound = directory->GetEntryInternal(dirName, true, [&](const Entry& entry)
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
	bool VirtualDirectory::CallbackReturn(F&& callback, Args&&... args)
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

