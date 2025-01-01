// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_RESOURCE_HPP
#define NAZARA_CORE_RESOURCE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Export.hpp>
#include <filesystem>

namespace Nz
{
	class NAZARA_CORE_API Resource
	{
		public:
			Resource() = default;
			Resource(const Resource&) = default;
			Resource(Resource&&) noexcept = default;
			virtual ~Resource();

			const std::filesystem::path& GetFilePath() const;

			void SetFilePath(std::filesystem::path filePath);

			Resource& operator=(const Resource&) = default;
			Resource& operator=(Resource&&) noexcept = default;

		private:
			std::filesystem::path m_filePath;
	};
}

#endif // NAZARA_CORE_RESOURCE_HPP
