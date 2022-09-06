// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/Wrapper/Context.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz::GL
{
	inline bool Context::DidLastCallSucceed() const
	{
		if (!m_didCollectErrors)
			ProcessErrorStack();

		return !m_hadAnyError;
	}

	inline const OpenGLDevice* Context::GetDevice() const
	{
		return m_device;
	}

	inline ExtensionStatus Context::GetExtensionStatus(Extension extension) const
	{
		return m_extensionStatus[UnderlyingCast(extension)];
	}

	inline GLFunction Context::GetFunctionByIndex(std::size_t funcIndex) const
	{
		assert(funcIndex < m_originalFunctionPointer.size());
		return m_originalFunctionPointer[funcIndex];
	}

	inline const OpenGLVaoCache& Context::GetVaoCache() const
	{
		return m_vaoCache;
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

	inline bool Context::HasZeroToOneDepth() const
	{
		return m_hasZeroToOneDepth;
	}

	inline void Context::NotifyBufferDestruction(GLuint buffer) const
	{
		for (GLuint& boundBuffer : m_state.bufferTargets)
		{
			if (boundBuffer == buffer)
				boundBuffer = 0;
		}

		m_vaoCache.NotifyBufferDestruction(buffer);
	}

	inline void Context::NotifyFramebufferDestruction(GLuint fbo) const
	{
		if (m_state.boundDrawFBO == fbo)
			m_state.boundDrawFBO = 0;

		if (m_state.boundReadFBO == fbo)
			m_state.boundReadFBO = 0;
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

	inline void Context::NotifyVertexArrayDestruction(GLuint vao) const
	{
		if (m_state.boundVertexArray == vao)
			m_state.boundVertexArray = 0;
	}

	inline void Context::ResetColorWriteMasks() const
	{
		if (!m_state.renderStates.colorWrite)
		{
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			m_state.renderStates.colorWrite = true;
		}
	}

	inline void Context::ResetDepthWriteMasks() const
	{
		if (!m_state.renderStates.depthWrite)
		{
			glDepthMask(GL_TRUE);
			m_state.renderStates.depthWrite = true;
		}
	}

	inline void Context::ResetStencilWriteMasks() const
	{
		if (m_state.renderStates.stencilBack.writeMask != 0xFFFFFFFF || m_state.renderStates.stencilFront.writeMask != 0xFFFFFFFF)
		{
			glStencilMaskSeparate(GL_FRONT_AND_BACK, 0xFFFFFFFF);
			m_state.renderStates.stencilBack.writeMask = 0xFFFFFFFF;
			m_state.renderStates.stencilFront.writeMask = 0xFFFFFFFF;
		}
	}

}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
