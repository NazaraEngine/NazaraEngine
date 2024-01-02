// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_RESOURCELOADER_HPP
#define NAZARA_CORE_RESOURCELOADER_HPP

#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceParameters.hpp>
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
	class ResourceLoader
	{
		static_assert(std::is_base_of<ResourceParameters, Parameters>::value, "ResourceParameters must be a base of Parameters");

		friend Type;

		public:
			struct Entry;
			using ExtensionSupport = std::function<bool(std::string_view extension)>;
			using FileLoader = std::function<Result<std::shared_ptr<Type>, ResourceLoadingError>(const std::filesystem::path& filePath, const Parameters& parameters)>;
			using MemoryLoader = std::function<Result<std::shared_ptr<Type>, ResourceLoadingError>(const void* data, std::size_t size, const Parameters& parameters)>;
			using ParameterFilter = std::function<bool(const Parameters& parameters)>;
			using StreamLoader = std::function<Result<std::shared_ptr<Type>, ResourceLoadingError>(Stream& stream, const Parameters& parameters)>;

			ResourceLoader() = default;
			ResourceLoader(const ResourceLoader&) = delete;
			ResourceLoader(ResourceLoader&&) noexcept = default;
			~ResourceLoader() = default;

			void Clear();

			bool IsExtensionSupported(std::string_view extension) const;

			std::shared_ptr<Type> LoadFromFile(const std::filesystem::path& filePath, const Parameters& parameters = Parameters()) const;
			std::shared_ptr<Type> LoadFromMemory(const void* data, std::size_t size, const Parameters& parameters = Parameters()) const;
			std::shared_ptr<Type> LoadFromStream(Stream& stream, const Parameters& parameters = Parameters()) const;

			const Entry* RegisterLoader(Entry loader);
			void UnregisterLoader(const Entry* loader);

			ResourceLoader& operator=(const ResourceLoader&) = delete;
			ResourceLoader& operator=(ResourceLoader&&) noexcept = default;

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

#include <Nazara/Core/ResourceLoader.inl>

#endif // NAZARA_CORE_RESOURCELOADER_HPP
