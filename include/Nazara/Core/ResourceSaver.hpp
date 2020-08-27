// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RESOURCESAVER_HPP
#define NAZARA_RESOURCESAVER_HPP

#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceParameters.hpp>
#include <Nazara/Core/String.hpp>
#include <filesystem>
#include <list>
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
			using ExtensionGetter = bool (*)(const std::string& extension);
			using FormatQuerier = bool (*)(const std::string& format);
			using FileSaver = bool (*)(const Type& resource, const std::filesystem::path& filePath, const Parameters& parameters);
			using StreamSaver = bool (*)(const Type& resource, const std::string& format, Stream& stream, const Parameters& parameters);

			ResourceSaver() = delete;
			~ResourceSaver() = delete;

			static bool IsFormatSupported(const std::string& extension);

			static bool SaveToFile(const Type& resource, const std::filesystem::path& filePath, const Parameters& parameters = Parameters());
			static bool SaveToStream(const Type& resource, Stream& stream, const std::string& format, const Parameters& parameters = Parameters());

			static void RegisterSaver(FormatQuerier formatQuerier, StreamSaver streamSaver, FileSaver fileSaver = nullptr);
			static void UnregisterSaver(FormatQuerier formatQuerier, StreamSaver streamSaver, FileSaver fileSaver = nullptr);

		private:
			using Saver = std::tuple<FormatQuerier, StreamSaver, FileSaver>;
			using SaverList = std::list<Saver>;
	};
}

#include <Nazara/Core/ResourceSaver.inl>

#endif // NAZARA_RESOURCESAVER_HPP
