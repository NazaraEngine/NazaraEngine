// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_RESOURCELOADER_HPP
#define NAZARA_CORE_RESOURCELOADER_HPP

#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceParameters.hpp>
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
			using ExtensionSupport = std::function<bool(const std::string_view& extension)>;
			using FileLoader = std::function<std::shared_ptr<Type>(const std::filesystem::path& filePath, const Parameters& parameters)>;
			using MemoryLoader = std::function<std::shared_ptr<Type>(const void* data, std::size_t size, const Parameters& parameters)>;
			using StreamChecker = std::function<Ternary(Stream& stream, const Parameters& parameters)>;
			using StreamLoader = std::function<std::shared_ptr<Type>(Stream& stream, const Parameters& parameters)>;

			ResourceLoader() = default;
			ResourceLoader(const ResourceLoader&) = delete;
			ResourceLoader(ResourceLoader&&) noexcept = default;
			~ResourceLoader() = default;

			void Clear();

			bool IsExtensionSupported(const std::string_view& extension) const;

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
				StreamChecker streamChecker;
				StreamLoader streamLoader;
			};

		private:
			std::vector<std::unique_ptr<Entry>> m_loaders;
	};
}

#include <Nazara/Core/ResourceLoader.inl>

#endif // NAZARA_CORE_RESOURCELOADER_HPP
