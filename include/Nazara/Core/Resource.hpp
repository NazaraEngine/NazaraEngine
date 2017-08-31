// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RESOURCE_HPP
#define NAZARA_RESOURCE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/String.hpp>

namespace Nz
{
	class NAZARA_CORE_API Resource
	{
		public:
			Resource() = default;
			Resource(const Resource&) = default;
			Resource(Resource&&) = default;
			virtual ~Resource();

			const String& GetFilePath() const;

			void SetFilePath(const String& filePath);

			Resource& operator=(const Resource&) = default;
			Resource& operator=(Resource&&) = default;

		private:
			String m_filePath;
	};
}

#endif // NAZARA_RESOURCE_HPP
