// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_HARDWAREINFO_HPP
#define NAZARA_HARDWAREINFO_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Enums.hpp>

class NAZARA_API NzHardwareInfo
{
	public:
		static unsigned int GetProcessorCount();
		static nzProcessorVendor GetProcessorVendor();
		static void GetProcessorVendor(char vendor[12]);

		static bool HasCapability(nzProcessorCap capability);

		static bool Initialize();

		static bool Is64Bits();

		static void Uninitialize();
};

#endif // NAZARA_HARDWAREINFO_HPP
