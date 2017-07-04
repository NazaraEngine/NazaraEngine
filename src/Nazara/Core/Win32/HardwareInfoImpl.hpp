// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_HARDWAREINFOIMPL_WINDOWS_HPP
#define NAZARA_HARDWAREINFOIMPL_WINDOWS_HPP

#include <Nazara/Prerequesites.hpp>

namespace Nz
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

#endif // NAZARA_HARDWAREINFOIMPL_WINDOWS_HPP
