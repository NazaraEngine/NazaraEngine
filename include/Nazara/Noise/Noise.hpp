// Copyright (C) 2017 Rémi Bèges
// This file is part of the "Nazara Engine - Noise module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_NOISE_HPP
#define NAZARA_NOISE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Noise/Config.hpp>

namespace Nz
{
	class NAZARA_NOISE_API Noise
	{
		public:
			Noise() = delete;
			~Noise() = delete;

			static bool Initialize();

			static bool IsInitialized();

			static void Uninitialize();

		private:
			static unsigned int s_moduleReferenceCounter;
	};
}

#endif // NAZARA_NOISE_HPP
