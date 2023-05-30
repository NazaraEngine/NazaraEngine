// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Audio/Debug.hpp>

namespace Nz
{
	inline bool OpenALDevice::IsExtensionSupported(OpenALExtension extension) const
	{
		return m_extensionStatus[extension];
	}

	inline ALenum OpenALDevice::TranslateAudioFormat(AudioFormat format) const
	{
		return m_audioFormatValues[format];
	}
}

#include <Nazara/Audio/DebugOff.hpp>
