// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PLATFORM_HPP
#define NAZARA_PLATFORM_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Platform/Config.hpp>

namespace Nz
{
	class NAZARA_PLATFORM_API Platform
	{
		public:
			Platform() = delete;
			~Platform() = delete;

			static bool Initialize();

			static bool IsInitialized();

			static void Uninitialize();

		private:
			static unsigned int s_moduleReferenceCounter;
	};
}

#endif // NAZARA_PLATFORM_HPP
