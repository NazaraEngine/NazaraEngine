// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio2 module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_AUDIO2_AUDIODEVICEINFO_HPP
#define NAZARA_AUDIO2_AUDIODEVICEINFO_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Audio2/Enums.hpp>
#include <array>

namespace Nz
{
	struct AudioDeviceId
	{
		alignas(8) std::array<char, 256> data;
	};

	struct AudioDeviceInfo
	{
		AudioDeviceId deviceId;
		AudioDeviceType deviceType;
		std::array<char, 256> deviceName;
		bool isDefault;
	};
}

#endif // NAZARA_AUDIO2_AUDIODEVICEINFO_HPP
