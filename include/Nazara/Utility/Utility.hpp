// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UTILITY_HPP
#define NAZARA_UTILITY_HPP

#include <Nazara/Prerequesites.hpp>

class NAZARA_API NzUtility
{
	public:
		NzUtility();
		~NzUtility();

		bool Initialize();
		void Uninitialize();

		static bool IsInitialized();

	private:
		static bool s_initialized;
};

#endif // NAZARA_UTILITY_HPP
