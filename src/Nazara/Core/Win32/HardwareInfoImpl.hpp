// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_WIN32_HARDWAREINFOIMPL_HPP
#define NAZARA_CORE_WIN32_HARDWAREINFOIMPL_HPP

#include <Nazara/Prerequisites.hpp>

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

#endif // NAZARA_CORE_WIN32_HARDWAREINFOIMPL_HPP
