// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_ASSETSAVER_HPP
#define NAZARA_CORE_ASSETSAVER_HPP

#include <Nazara/Core/Asset.hpp>
#include <Nazara/Core/AssetParameters.hpp>
#include <Nazara/Core/Enums.hpp>
#include <filesystem>
#include <list>
#include <string>
#include <tuple>
#include <type_traits>

namespace Nz
{
	class Stream;

	template<typename Type, typename Parameters>
	class AssetSaver
	{
		static_assert(std::is_base_of<AssetParameters, Parameters>::value, "AssetParameters must be a base of Parameters");

		friend Type;

		public:
			struct Entry;
			using FormatSupport = std::function<bool(std::string_view format)>;
			using FileSaver = std::function<bool(const Type& resource, const std::filesystem::path& filePath, const Parameters& parameters)>;
			using StreamSaver = std::function<bool(const Type& resource, const std::string& format, Stream& stream, const Parameters& parameters)>;

			AssetSaver() = default;
			AssetSaver(const AssetSaver&) = delete;
			AssetSaver(AssetSaver&&) noexcept = default;
			~AssetSaver() = default;

			void Clear();

			bool IsExtensionSupported(std::string_view extension) const;

			bool SaveToFile(const Type& resource, const std::filesystem::path& filePath, const Parameters& parameters = Parameters()) const;
			bool SaveToStream(const Type& resource, Stream& stream, const std::string& format, const Parameters& parameters = Parameters()) const;

			const Entry* RegisterSaver(Entry saver);
			void UnregisterSaver(const Entry* saver);

			AssetSaver& operator=(const AssetSaver&) = delete;
			AssetSaver& operator=(AssetSaver&&) noexcept = default;

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

#include <Nazara/Core/AssetSaver.inl>

#endif // NAZARA_CORE_ASSETSAVER_HPP
