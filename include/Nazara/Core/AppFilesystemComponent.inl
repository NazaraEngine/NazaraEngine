// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ResourceRegistry.hpp>
#include <stdexcept>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	namespace Detail
	{
		template<typename, typename = void>
		struct ResourceParameterHasMerge : std::false_type {};

		template<typename T>
		struct ResourceParameterHasMerge<T, std::void_t<decltype(std::declval<T>().Merge(std::declval<T>()))>> : std::true_type {};
	}

	inline AppFilesystemComponent::AppFilesystemComponent(ApplicationBase& app) :
	ApplicationComponent(app)
	{
		RegisterResourceTypes();
	}

	template<typename T>
	const typename T::Params* AppFilesystemComponent::GetDefaultResourceParameters() const
	{
		std::size_t resourceIndex = ResourceRegistry<T>::GetResourceId();
		if (resourceIndex >= m_defaultParameters.size())
			return nullptr;

		return static_cast<const typename T::Params*>(m_defaultParameters[resourceIndex].get());
	}

	template<typename T, typename... ExtraArgs>
	std::shared_ptr<T> AppFilesystemComponent::Load(std::string_view assetPath, ExtraArgs&&... args)
	{
		return Load<T>(assetPath, typename T::Params{}, std::forward<ExtraArgs>(args)...);
	}

	template<typename T, typename... ExtraArgs>
	std::shared_ptr<T> AppFilesystemComponent::Load(std::string_view assetPath, typename T::Params params, ExtraArgs&&... args)
	{
		if constexpr (Detail::ResourceParameterHasMerge<typename T::Params>::value)
		{
			if (const auto* defaultParams = GetDefaultResourceParameters<T>())
				params.Merge(*defaultParams);
		}

		return LoadImpl<T>(assetPath, params, std::forward<ExtraArgs>(args)...);
	}

	template<typename T, typename... ExtraArgs>
	std::shared_ptr<T> AppFilesystemComponent::Open(std::string_view assetPath, ExtraArgs&&... args)
	{
		return Open<T>(assetPath, typename T::Params{}, std::forward<ExtraArgs>(args)...);
	}

	template<typename T, typename... ExtraArgs>
	std::shared_ptr<T> AppFilesystemComponent::Open(std::string_view assetPath, typename T::Params params, ExtraArgs&&... args)
	{
		if constexpr (Detail::ResourceParameterHasMerge<typename T::Params>::value)
		{
			if (const auto* defaultParams = GetDefaultResourceParameters<T>())
				params.Merge(*defaultParams);
		}

		return OpenImpl<T>(assetPath, params, std::forward<ExtraArgs>(args)...);
	}

	template<typename T>
	void AppFilesystemComponent::SetDefaultResourceParameters(typename T::Params params)
	{
		std::size_t resourceIndex = ResourceRegistry<T>::GetResourceId();
		if (resourceIndex >= m_defaultParameters.size())
			m_defaultParameters.resize(resourceIndex + 1);

		m_defaultParameters[resourceIndex] = std::make_unique<typename T::Params>(std::move(params));
	}

	inline void AppFilesystemComponent::RegisterResourceTypes()
	{
		// TODO: Switch to hash-based approach like entt?

		if (ResourceRegistry<Font>::GetResourceId() != 0)
			throw std::runtime_error("Font has wrong resource index, please initialize AppFilesystemComponent before using ResourceRegistry");

		if (ResourceRegistry<Image>::GetResourceId() != 1)
			throw std::runtime_error("Image has wrong resource index, please initialize AppFilesystemComponent before using ResourceRegistry");

		if (ResourceRegistry<ImageStream>::GetResourceId() != 2)
			throw std::runtime_error("ImageStream has wrong resource index, please initialize AppFilesystemComponent before using ResourceRegistry");

		if (ResourceRegistry<Material>::GetResourceId() != 3)
			throw std::runtime_error("Material has wrong resource index, please initialize AppFilesystemComponent before using ResourceRegistry");

		if (ResourceRegistry<MaterialInstance>::GetResourceId() != 4)
			throw std::runtime_error("MaterialInstance has wrong resource index, please initialize AppFilesystemComponent before using ResourceRegistry");

		if (ResourceRegistry<Mesh>::GetResourceId() != 5)
			throw std::runtime_error("Mesh has wrong resource index, please initialize AppFilesystemComponent before using ResourceRegistry");

		if (ResourceRegistry<SoundBuffer>::GetResourceId() != 6)
			throw std::runtime_error("SoundBuffer has wrong resource index, please initialize AppFilesystemComponent before using ResourceRegistry");

		if (ResourceRegistry<SoundStream>::GetResourceId() != 7)
			throw std::runtime_error("SoundStream has wrong resource index, please initialize AppFilesystemComponent before using ResourceRegistry");

		if (ResourceRegistry<Texture>::GetResourceId() != 8)
			throw std::runtime_error("Texture has wrong resource index, please initialize AppFilesystemComponent before using ResourceRegistry");
	}

	template<typename T, typename... ExtraArgs>
	std::shared_ptr<T> AppFilesystemComponent::LoadImpl(std::string_view assetPath, const typename T::Params& params, ExtraArgs&&... args)
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
					NazaraError(std::string(assetPath) + " is a directory");
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
	std::shared_ptr<T> AppFilesystemComponent::OpenImpl(std::string_view assetPath, const typename T::Params& params, ExtraArgs&&... args)
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
					NazaraError(std::string(assetPath) + " is a directory");
					return false;
				}
				else if constexpr (std::is_same_v<Param, VirtualDirectory::FileEntry>)
				{
					// If stream has an associated filesystem path, use OpenFromFile instead to make sure the File stream is kept alive
					// (a temporary FileEntry may be created when resolving a filesystem folder)
					if (std::filesystem::path filePath = arg.stream->GetPath(); !filePath.empty())
						resource = T::OpenFromFile(*arg.stream, params, std::forward<ExtraArgs>(args)...);
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

#include <Nazara/Core/DebugOff.hpp>
