// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/Wrapper/Context.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz::GL
{
	inline Context::Context(const OpenGLDevice* device) :
	m_device(device)
	{
	}

	inline const OpenGLDevice* Context::GetDevice() const
	{
		return m_device;
	}

	inline ExtensionStatus Context::GetExtensionStatus(Extension extension) const
	{
		return m_extensionStatus[UnderlyingCast(extension)];
	}

	inline const ContextParams& Context::GetParams() const
	{
		return m_params;
	}

	inline bool Context::IsExtensionSupported(Extension extension) const
	{
		return GetExtensionStatus(extension) != ExtensionStatus::NotSupported;
	}

	inline bool Context::IsExtensionSupported(const std::string& extension) const
	{
		return m_supportedExtensions.find(extension) != m_supportedExtensions.end();
	}

	inline void Context::NotifyBufferDestruction(GLuint buffer) const
	{
		for (GLuint& boundBuffer : m_state.bufferTargets)
		{
			if (boundBuffer == buffer)
				boundBuffer = 0;
		}
	}

	inline void Context::NotifyProgramDestruction(GLuint program) const
	{
		if (m_state.boundProgram == program)
			m_state.boundProgram = 0;
	}

	inline void Context::NotifySamplerDestruction(GLuint sampler) const
	{
		for (auto& unit : m_state.textureUnits)
		{
			if (unit.sampler == sampler)
				unit.sampler = 0;
		}
	}

	inline void Context::NotifyTextureDestruction(GLuint texture) const
	{
		for (auto& unit : m_state.textureUnits)
		{
			for (GLuint& boundTexture : unit.textureTargets)
			{
				if (boundTexture == texture)
					boundTexture = 0;
			}
		}
	}

	inline void Context::SetCurrentTextureUnit(UInt32 textureUnit) const
	{
		if (m_state.currentTextureUnit != textureUnit)
		{
			glActiveTexture(GL_TEXTURE0 + textureUnit);
			m_state.currentTextureUnit = textureUnit;
		}
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
