// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/AppFilesystemComponent.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	template<typename T, typename... Args>
	std::shared_ptr<T> AppFilesystemComponent::GetOrLoad(std::string_view assetPath, Args&&... args)
	{
		std::shared_ptr<T> resource;
		for (const VirtualDirectoryPtr& virtualDir : m_virtualDirectories)
		{
			auto callback = [&](const VirtualDirectory::Entry& entry)
			{
				return std::visit([&](auto&& arg)
				{
					using Param = std::decay_t<decltype(arg)>;
					if constexpr (std::is_base_of_v<VirtualDirectory::DirectoryEntry, Param>)
					{
						NazaraError(std::string(assetPath) + " is a directory");
						return false;
					}
					else if constexpr (std::is_same_v<Param, VirtualDirectory::DataPointerEntry>)
					{
						resource = T::LoadFromMemory(arg.data, arg.size, std::forward<Args>(args)...);
						return true;
					}
					else if constexpr (std::is_same_v<Param, VirtualDirectory::FileContentEntry>)
					{
						resource = T::LoadFromMemory(&arg.data[0], arg.data.size(), std::forward<Args>(args)...);
						return true;
					}
					else if constexpr (std::is_same_v<Param, VirtualDirectory::PhysicalFileEntry>)
					{
						resource = T::LoadFromFile(arg.filePath, std::forward<Args>(args)...);
						return true;
					}
					else
						static_assert(AlwaysFalse<Param>(), "unhandled case");
				}, entry);
			};

			if (virtualDir->GetEntry(assetPath, callback) && resource)
				return resource;
		}

		return resource;
	}

	inline const VirtualDirectoryPtr& AppFilesystemComponent::RegisterPath(std::filesystem::path filepath)
	{
		return RegisterVirtualDirectory(std::make_shared<VirtualDirectory>(std::move(filepath)));
	}

	inline const VirtualDirectoryPtr& AppFilesystemComponent::RegisterVirtualDirectory(VirtualDirectoryPtr directory)
	{
		return m_virtualDirectories.emplace_back(std::move(directory));
	}

	inline void AppFilesystemComponent::UnregisterVirtualDirectory(const VirtualDirectoryPtr& directory)
	{
		auto it = std::find(m_virtualDirectories.begin(), m_virtualDirectories.end(), directory);
		if (it == m_virtualDirectories.end())
			m_virtualDirectories.erase(it);
	}
}

#include <Nazara/Core/DebugOff.hpp>
