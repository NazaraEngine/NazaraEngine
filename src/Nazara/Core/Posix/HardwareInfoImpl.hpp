// Copyright (C) 2015 Alexandre Janniaux
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
		static void Cpuid(nzUInt32 functionId, nzUInt32 subFunctionId, nzUInt32 registers[4]);
		static unsigned int GetProcessorCount();
		static nzUInt64 GetTotalMemory();
		static bool IsCpuidSupported();
};

#endif // NAZARA_HARDWAREINFOIMPL_POSIX_HPP
