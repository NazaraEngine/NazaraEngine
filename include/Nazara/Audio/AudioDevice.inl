// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Audio/AudioDevice.hpp>
#include <Nazara/Audio/Debug.hpp>

namespace Nz
{
	inline void AudioDevice::SetListenerRotation(const Quaternionf& rotation)
	{
		SetListenerDirection(rotation * Vector3f::Forward(), rotation * Vector3f::Up());
	}
}

#include <Nazara/Audio/DebugOff.hpp>
