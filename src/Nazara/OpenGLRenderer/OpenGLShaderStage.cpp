// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLShaderStage.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/OpenGLRenderer/Utils.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	bool OpenGLShaderStage::Create(OpenGLDevice& device, ShaderStageType type, ShaderLanguage lang, const void* source, std::size_t sourceSize)
	{
		if (!m_shader.Create(device, ToOpenGL(type)))
			return false; //< TODO: Handle error message

		switch (lang)
		{
			case ShaderLanguage::GLSL:
				m_shader.SetSource(reinterpret_cast<const char*>(source), GLint(sourceSize));
				break;

			case ShaderLanguage::SpirV:
			{
				if (!device.GetReferenceContext().IsExtensionSupported("GL_ARB_gl_spirv"))
				{
					NazaraError("Spir-V is not supported");
					return false;
				}

				constexpr GLenum SHADER_BINARY_FORMAT_SPIR_V = 0x9551;

				m_shader.SetBinarySource(SHADER_BINARY_FORMAT_SPIR_V, source, GLsizei(sourceSize));
				break;
			}

			default:
				NazaraError("Unsupported shader language");
				return false;
		}

		std::string errorLog;
		if (!m_shader.Compile(&errorLog))
		{
			NazaraError("Failed to compile shader: " + errorLog);
			return false;
		}

		return true;
	}
}
