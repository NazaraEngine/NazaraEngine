// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_ASSETLOADER_HPP
#define NAZARA_CORE_ASSETLOADER_HPP

#include <Nazara/Core/Asset.hpp>
#include <Nazara/Core/AssetParameters.hpp>
#include <Nazara/Core/Enums.hpp>
#include <NazaraUtils/Result.hpp>
#include <filesystem>
#include <functional>
#include <memory>
#include <type_traits>
#include <vector>

namespace Nz
{
	class Stream;

	template<typename Type, typename Parameters>
	class AssetLoader
	{
		static_assert(std::is_base_of<AssetParameters, Parameters>::value, "AssetParameters must be a base of Parameters");

		friend Type;

		public:
			struct Entry;
			using ExtensionSupport = std::function<bool(std::string_view extension)>;
			using FileLoader = std::function<Result<std::shared_ptr<Type>, AssetLoadingError>(const std::filesystem::path& filePath, const Parameters& parameters)>;
			using MemoryLoader = std::function<Result<std::shared_ptr<Type>, AssetLoadingError>(const void* data, std::size_t size, const Parameters& parameters)>;
			using ParameterFilter = std::function<bool(const Parameters& parameters)>;
			using StreamLoader = std::function<Result<std::shared_ptr<Type>, AssetLoadingError>(Stream& stream, const Parameters& parameters)>;

			AssetLoader() = default;
			AssetLoader(const AssetLoader&) = delete;
			AssetLoader(AssetLoader&&) noexcept = default;
			~AssetLoader() = default;

			void Clear();

			bool IsExtensionSupported(std::string_view extension) const;

			std::shared_ptr<Type> LoadFromFile(const std::filesystem::path& filePath, const Parameters& parameters = Parameters()) const;
			std::shared_ptr<Type> LoadFromMemory(const void* data, std::size_t size, const Parameters& parameters = Parameters()) const;
			std::shared_ptr<Type> LoadFromStream(Stream& stream, const Parameters& parameters = Parameters()) const;

			const Entry* RegisterLoader(Entry loader);
			void UnregisterLoader(const Entry* loader);

			AssetLoader& operator=(const AssetLoader&) = delete;
			AssetLoader& operator=(AssetLoader&&) noexcept = default;

			struct Entry
			{
				ExtensionSupport extensionSupport;
				FileLoader fileLoader;
				MemoryLoader memoryLoader;
				ParameterFilter parameterFilter;
				StreamLoader streamLoader;
			};

		private:
			std::vector<std::unique_ptr<Entry>> m_loaders;
	};
}

#include <Nazara/Core/AssetLoader.inl>

#endif // NAZARA_CORE_ASSETLOADER_HPP
