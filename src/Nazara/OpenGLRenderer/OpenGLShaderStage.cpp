// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLShaderStage.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/OpenGLRenderer/Utils.hpp>
#include <stdexcept>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	OpenGLShaderStage::OpenGLShaderStage(OpenGLDevice& device, ShaderStageType type, ShaderLanguage lang, const void* source, std::size_t sourceSize)
	{
		if (!m_shader.Create(device, ToOpenGL(type)))
			throw std::runtime_error("failed to create shader"); //< TODO: Handle error message

		switch (lang)
		{
			case ShaderLanguage::GLSL:
				m_shader.SetSource(reinterpret_cast<const char*>(source), GLint(sourceSize));
				m_shader.Compile();
				break;

			case ShaderLanguage::SpirV:
			{
				if (!device.GetReferenceContext().IsExtensionSupported(GL::Extension::SpirV))
					throw std::runtime_error("SpirV is not supported by this OpenGL implementation");

				m_shader.SetBinarySource(GL_SHADER_BINARY_FORMAT_SPIR_V_ARB, source, GLsizei(sourceSize));
				m_shader.SpecializeShader("main", 0U, nullptr, nullptr);
				break;
			}

			default:
				throw std::runtime_error("Unsupported shader language");
		}

		std::string errorLog;
		if (!m_shader.GetCompilationStatus(&errorLog))
			throw std::runtime_error("Failed to compile shader: " + errorLog);
	}
}
