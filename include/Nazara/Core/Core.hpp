// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_HPP
#define NAZARA_CORE_HPP

#include <Nazara/Prerequesites.hpp>

namespace Nz
{
	class NAZARA_CORE_API Core
	{
		public:
			Core() = delete;
			~Core() = delete;

			static bool Initialize();

			static bool IsInitialized();

			static void Uninitialize();

		private:
			static unsigned int s_moduleReferenceCounter;
	};
}

#endif // NAZARA_CORE_HPP
