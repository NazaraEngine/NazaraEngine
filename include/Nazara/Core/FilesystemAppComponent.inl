// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Error.hpp>
#include <NazaraUtils/Hash.hpp>
#include <NazaraUtils/TypeName.hpp>
#include <stdexcept>

namespace Nz
{
	namespace Detail
	{
		template<typename, typename = void>
		struct ResourceParameterHasMerge : std::false_type {};

		template<typename T>
		struct ResourceParameterHasMerge<T, std::void_t<decltype(std::declval<T>().Merge(std::declval<T>()))>> : std::true_type {};
	}

	inline FilesystemAppComponent::FilesystemAppComponent(ApplicationBase& app) :
	ApplicationComponent(app)
	{
	}

	template<typename T>
	const typename T::Params* FilesystemAppComponent::GetDefaultResourceParameters() const
	{
		constexpr UInt64 typeHash = FNV1a64(TypeName<T>());

		auto it = m_defaultParameters.find(typeHash);
		if (it == m_defaultParameters.end())
			return nullptr;

		return static_cast<const typename T::Params*>(it->second.get());
	}

	inline VirtualDirectoryPtr FilesystemAppComponent::GetDirectory(std::string_view dirPath)
	{
		VirtualDirectoryPtr dir;
		m_rootDirectory->GetDirectoryEntry(dirPath, [&](const Nz::VirtualDirectory::DirectoryEntry& dirEntry)
		{
			dir = dirEntry.directory;
		});

		return dir;
	}

	inline std::shared_ptr<Stream> FilesystemAppComponent::GetFile(std::string_view filepath)
	{
		std::shared_ptr<Stream> fileStream;
		m_rootDirectory->GetFileEntry(filepath, [&](const Nz::VirtualDirectory::FileEntry& fileEntry)
		{
			fileStream = fileEntry.stream;
		});

		return fileStream;
	}

	template<typename F>
	bool FilesystemAppComponent::GetFileContent(std::string_view filepath, F&& callback)
	{
		return m_rootDirectory->GetFileContent(filepath, callback);
	}

	template<typename F>
	void FilesystemAppComponent::IterateOnDirectory(std::string_view dirPath, F&& callback)
	{
		m_rootDirectory->Foreach(callback);
	}

	template<typename T, typename... ExtraArgs>
	std::shared_ptr<T> FilesystemAppComponent::Load(std::string_view assetPath, ExtraArgs&&... args)
	{
		return Load<T>(assetPath, typename T::Params{}, std::forward<ExtraArgs>(args)...);
	}

	template<typename T, typename... ExtraArgs>
	std::shared_ptr<T> FilesystemAppComponent::Load(std::string_view assetPath, typename T::Params params, ExtraArgs&&... args)
	{
		if constexpr (Detail::ResourceParameterHasMerge<typename T::Params>::value)
		{
			if (const auto* defaultParams = GetDefaultResourceParameters<T>())
				params.Merge(*defaultParams);
		}

		return LoadImpl<T>(assetPath, params, std::forward<ExtraArgs>(args)...);
	}

	template<typename T, typename... ExtraArgs>
	std::shared_ptr<T> FilesystemAppComponent::Open(std::string_view assetPath, ExtraArgs&&... args)
	{
		return Open<T>(assetPath, typename T::Params{}, std::forward<ExtraArgs>(args)...);
	}

	template<typename T, typename... ExtraArgs>
	std::shared_ptr<T> FilesystemAppComponent::Open(std::string_view assetPath, typename T::Params params, ExtraArgs&&... args)
	{
		if constexpr (Detail::ResourceParameterHasMerge<typename T::Params>::value)
		{
			if (const auto* defaultParams = GetDefaultResourceParameters<T>())
				params.Merge(*defaultParams);
		}

		return OpenImpl<T>(assetPath, params, std::forward<ExtraArgs>(args)...);
	}

	template<typename T>
	void FilesystemAppComponent::SetDefaultResourceParameters(typename T::Params params)
	{
		constexpr UInt64 typeHash = FNV1a64(TypeName<T>());

		m_defaultParameters[typeHash] = std::make_unique<typename T::Params>(std::move(params));
	}

	template<typename T, typename... ExtraArgs>
	std::shared_ptr<T> FilesystemAppComponent::LoadImpl(std::string_view assetPath, const typename T::Params& params, ExtraArgs&&... args)
	{
		std::shared_ptr<T> resource;
		if (!m_rootDirectory)
			return resource;

		auto callback = [&](const VirtualDirectory::Entry& entry)
		{
			return std::visit([&](auto&& arg)
			{
				using Param = std::decay_t<decltype(arg)>;
				if constexpr (std::is_base_of_v<VirtualDirectory::DirectoryEntry, Param>)
				{
					NazaraErrorFmt("{} is a directory", assetPath);
					return false;
				}
				else if constexpr (std::is_same_v<Param, VirtualDirectory::FileEntry>)
				{
					resource = T::LoadFromStream(*arg.stream, params, std::forward<ExtraArgs>(args)...);
					return true;
				}
				else
					static_assert(AlwaysFalse<Param>(), "unhandled case");
			}, entry);
		};

		m_rootDirectory->GetEntry(assetPath, callback);
		return resource;
	}

	template<typename T, typename... ExtraArgs>
	std::shared_ptr<T> FilesystemAppComponent::OpenImpl(std::string_view assetPath, const typename T::Params& params, ExtraArgs&&... args)
	{
		std::shared_ptr<T> resource;
		if (!m_rootDirectory)
			return resource;

		auto callback = [&](const VirtualDirectory::Entry& entry)
		{
			return std::visit([&](auto&& arg)
			{
				using Param = std::decay_t<decltype(arg)>;
				if constexpr (std::is_base_of_v<VirtualDirectory::DirectoryEntry, Param>)
				{
					NazaraErrorFmt("{} is a directory", assetPath);
					return false;
				}
				else if constexpr (std::is_same_v<Param, VirtualDirectory::FileEntry>)
				{
					// If stream has an associated filesystem path, use OpenFromFile instead to make sure the File stream is kept alive
					// (a temporary FileEntry may be created when resolving a filesystem folder)
					if (std::filesystem::path filePath = arg.stream->GetPath(); !filePath.empty())
						resource = T::OpenFromFile(filePath, params, std::forward<ExtraArgs>(args)...);
					else
						resource = T::OpenFromStream(*arg.stream, params, std::forward<ExtraArgs>(args)...);

					return true;
				}
				else
					static_assert(AlwaysFalse<Param>(), "unhandled case");
			}, entry);
		};

		m_rootDirectory->GetEntry(assetPath, callback);
		return resource;
	}
}

