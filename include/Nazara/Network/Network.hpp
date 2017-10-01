// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MODULENAME_HPP
#define NAZARA_MODULENAME_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Network/Config.hpp>

namespace Nz
{
	class NAZARA_NETWORK_API Network
	{
		public:
			Network() = delete;
			~Network() = delete;

			static bool Initialize();

			static bool IsInitialized();

			static void Uninitialize();

		private:
			static unsigned int s_moduleReferenceCounter;
    };
}

#endif // NAZARA_MODULENAME_HPP
