// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_ASSET_HPP
#define NAZARA_CORE_ASSET_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Config.hpp>
#include <filesystem>

namespace Nz
{
	class NAZARA_CORE_API Asset
	{
		public:
			Asset() = default;
			Asset(const Asset&) = default;
			Asset(Asset&&) noexcept = default;
			virtual ~Asset();

			const std::filesystem::path& GetFilePath() const;

			void SetFilePath(std::filesystem::path filePath);

			Asset& operator=(const Asset&) = default;
			Asset& operator=(Asset&&) noexcept = default;

		private:
			std::filesystem::path m_filePath;
	};
}

#endif // NAZARA_CORE_ASSET_HPP
