// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Audio/OpenALDevice.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Audio/Debug.hpp>

namespace Nz
{
	inline ALenum OpenALDevice::TranslateAudioFormat(AudioFormat format) const
	{
		return m_audioFormatValues[UnderlyingCast(format)];
	}
}

#include <Nazara/Audio/DebugOff.hpp>
