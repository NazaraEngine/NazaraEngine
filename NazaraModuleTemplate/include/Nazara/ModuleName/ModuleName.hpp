// Copyright (C) YEAR AUTHORS
// This file is part of the "Nazara Engine - Module name"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MODULENAME_HPP
#define NAZARA_MODULENAME_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Initializer.hpp>
#include <Nazara/ModuleName/Config.hpp>

namespace Nz
{
	class NAZARA_MODULENAME_API ModuleName
	{
		public:
			ModuleName() = delete;
			~ModuleName() = delete;

			static bool Initialize();

			static bool IsInitialized();

			static void Uninitialize();

		private:
			static unsigned int s_moduleReferenceCounter;
	};	
}

#endif // NAZARA_MODULENAME_HPP
