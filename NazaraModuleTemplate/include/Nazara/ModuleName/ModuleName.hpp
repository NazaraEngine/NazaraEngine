// Copyright (C) 2012 AUTHORS
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MODULENAME_HPP
#define NAZARA_MODULENAME_HPP

#include <Nazara/Prerequesites.hpp>

class NAZARA_API NzModuleName
{
	public:
		NzModuleName();
		~NzModuleName();

		bool Initialize();
		void Uninitialize();

		static bool IsInitialized();

	private:
		static bool s_initialized;
};

#endif // NAZARA_MODULENAME_HPP
