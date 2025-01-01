// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Export.hpp


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
