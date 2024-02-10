// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Audio/Debug.hpp>

namespace Nz
{
	inline ALFunction OpenALDevice::GetFunctionByIndex(std::size_t funcIndex) const
	{
		assert(funcIndex < m_originalFunctionPointer.size());
		return m_originalFunctionPointer[funcIndex];
	}

	inline OpenALLibrary& OpenALDevice::GetLibrary()
	{
		return m_library;
	}

	inline const OpenALLibrary& OpenALDevice::GetLibrary() const
	{
		return m_library;
	}

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
