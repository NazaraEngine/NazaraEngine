// Copyright (C) 2012 AUTHORS
// This file is part of the "Nazara Engine - Module name"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MODULENAME_HPP
#define NAZARA_MODULENAME_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Initializer.hpp>

class NAZARA_API NzModuleName
{
	public:
		NzModuleName() = delete;
		~NzModuleName() = delete;

		static bool Initialize();

		static bool IsInitialized();

		static void Uninitialize();

	private:
		static unsigned int s_moduleReferenceCouter;
};

#endif // NAZARA_MODULENAME_HPP
