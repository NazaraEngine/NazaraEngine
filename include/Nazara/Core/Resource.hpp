// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_RESOURCE_HPP
#define NAZARA_CORE_RESOURCE_HPP

#include <Nazara/Prerequisites.hpp>
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

			void SetFilePath(const std::filesystem::path& filePath);

			Resource& operator=(const Resource&) = default;
			Resource& operator=(Resource&&) noexcept = default;

		private:
			std::filesystem::path m_filePath;
	};
}

#endif // NAZARA_CORE_RESOURCE_HPP
