// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_RESOURCESAVER_HPP
#define NAZARA_CORE_RESOURCESAVER_HPP

#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceParameters.hpp>
#include <filesystem>
#include <list>
#include <string>
#include <tuple>
#include <type_traits>

namespace Nz
{
	class Stream;

	template<typename Type, typename Parameters>
	class ResourceSaver
	{
		static_assert(std::is_base_of<ResourceParameters, Parameters>::value, "ResourceParameters must be a base of Parameters");

		friend Type;

		public:
			struct Entry;
			using FormatSupport = std::function<bool(std::string_view format)>;
			using FileSaver = std::function<bool(const Type& resource, const std::filesystem::path& filePath, const Parameters& parameters)>;
			using StreamSaver = std::function<bool(const Type& resource, std::string_view format, Stream& stream, const Parameters& parameters)>;

			ResourceSaver() = default;
			ResourceSaver(const ResourceSaver&) = delete;
			ResourceSaver(ResourceSaver&&) noexcept = default;
			~ResourceSaver() = default;

			void Clear();

			bool IsExtensionSupported(std::string_view extension) const;

			bool SaveToFile(const Type& resource, const std::filesystem::path& filePath, const Parameters& parameters = Parameters()) const;
			bool SaveToStream(const Type& resource, Stream& stream, std::string_view format, const Parameters& parameters = Parameters()) const;

			const Entry* RegisterSaver(Entry saver);
			void UnregisterSaver(const Entry* saver);

			ResourceSaver& operator=(const ResourceSaver&) = delete;
			ResourceSaver& operator=(ResourceSaver&&) noexcept = default;

			struct Entry
			{
				FormatSupport formatSupport;
				FileSaver fileSaver;
				StreamSaver streamSaver;
			};

		private:
			std::vector<std::unique_ptr<Entry>> m_savers;
	};
}

#include <Nazara/Core/ResourceSaver.inl>

#endif // NAZARA_CORE_RESOURCESAVER_HPP
