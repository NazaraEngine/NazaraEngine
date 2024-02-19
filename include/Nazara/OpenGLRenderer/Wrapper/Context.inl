// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz::GL
{
	inline bool Context::DidLastCallSucceed() const
	{
		if (!m_didCollectErrors)
			ProcessErrorStack();

		return !m_hadAnyError;
	}

	inline bool Context::GetBoolean(GLenum name) const
	{
		GLboolean value;
		glGetBooleanv(name, &value);

		return value != GL_FALSE;
	}

	inline bool Context::GetBoolean(GLenum name, GLuint index) const
	{
		GLboolean value;
		glGetBooleani_v(name, index, &value);

		return value != GL_FALSE;
	}

	inline const OpenGLDevice* Context::GetDevice() const
	{
		return m_device;
	}

	inline ExtensionStatus Context::GetExtensionStatus(Extension extension) const
	{
		return m_extensionStatus[extension];
	}

	inline float Context::GetFloat(GLenum name) const
	{
		GLfloat value;
		glGetFloatv(name, &value);

		return value;
	}

	inline GLFunction Context::GetFunctionByIndex(std::size_t funcIndex) const
	{
		assert(funcIndex < m_originalFunctionPointer.size());
		return m_originalFunctionPointer[funcIndex];
	}

	template<typename T>
	T Context::GetInteger(GLenum name) const
	{
		if constexpr (std::is_same_v<T, Int64> || std::is_same_v<T, UInt64>)
		{
			GLint64 value;
			glGetInteger64v(name, &value);

			return SafeCast<T>(value);
		}
		else
		{
			GLint value;
			glGetIntegerv(name, &value);

			return SafeCast<T>(value);
		}
	}

	template<typename T>
	T Context::GetInteger(GLenum name, GLuint index) const
	{
		if constexpr (std::is_same_v<T, Int64> || std::is_same_v<T, UInt64>)
		{
			GLint64 value;
			glGetInteger64i_v(name, index, &value);

			return SafeCast<T>(value);
		}
		else
		{
			GLint value;
			glGetIntegeri_v(name, index, &value);

			return SafeCast<T>(value);
		}
	}

	inline const ContextParams& Context::GetParams() const
	{
		return m_params;
	}

	inline const OpenGLVaoCache& Context::GetVaoCache() const
	{
		return m_vaoCache;
	}

	inline bool Context::IsExtensionSupported(Extension extension) const
	{
		return GetExtensionStatus(extension) != ExtensionStatus::NotSupported;
	}

	inline bool Context::IsExtensionSupported(std::string_view extension) const
	{
		return m_supportedExtensions.contains(extension);
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
		if (m_state.renderStates.colorWriteMask != ColorComponentAll)
		{
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			m_state.renderStates.colorWriteMask = ColorComponentAll;
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

