// Copyright (C) 2012 Alexandre Janniaux
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_HARDWAREINFOIMPL_POSIX_HPP
#define NAZARA_HARDWAREINFOIMPL_POSIX_HPP

#include <Nazara/Prerequesites.hpp>
#include <unistd.h>

class NzHardwareInfoImpl
{
	public:
		static void Cpuid(nzUInt32 code, nzUInt32 result[4]);
		static unsigned int GetProcessorCount();
		static bool IsCpuidSupported();
};

#endif // NAZARA_HARDWAREINFOIMPL_POSIX_HPP
