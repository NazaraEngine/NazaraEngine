// Copyright (C) 2012 Jérôme Leclercq / Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PHYSICS_HPP
#define NAZARA_PHYSICS_HPP

#include <Nazara/Prerequesites.hpp>

class NAZARA_API NzPhysics
{
	public:
		NzPhysics();
		~NzPhysics();

		bool Initialize();
		void Uninitialize();

		static bool IsInitialized();

	private:
		static bool s_initialized;
};

#endif // NAZARA_PHYSICS_HPP
