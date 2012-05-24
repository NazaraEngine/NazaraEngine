// Copyright (C) 2012 Rémi "Overdrivr" Bèges (remi{dot}beges{at}gmail{dot}com)
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_NOISE_HPP
#define NAZARA_NOISE_HPP

#include <Nazara/Prerequesites.hpp>

class NAZARA_API NzNoise
{
	public:
		NzNoise();
		~NzNoise();

		bool Initialize();
		void Uninitialize();

		static bool IsInitialized();

	private:
		static bool s_initialized;
};

#endif // NOISE_H
