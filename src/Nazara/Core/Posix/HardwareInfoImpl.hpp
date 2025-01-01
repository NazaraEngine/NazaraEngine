// Copyright (C) 2025 Alexandre Janniaux
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_POSIX_HARDWAREINFOIMPL_HPP
#define NAZARA_CORE_POSIX_HARDWAREINFOIMPL_HPP

#include <NazaraUtils/Prerequisites.hpp>

namespace Nz::PlatformImpl
{
	class HardwareInfoImpl
	{
		public:
			static void Cpuid(UInt32 functionId, UInt32 subFunctionId, UInt32 registers[4]);
			static unsigned int GetProcessorCount();
			static UInt64 GetTotalMemory();
			static bool IsCpuidSupported();
	};
}

#endif // NAZARA_CORE_POSIX_HARDWAREINFOIMPL_HPP
