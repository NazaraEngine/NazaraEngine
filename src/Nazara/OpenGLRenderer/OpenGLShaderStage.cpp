// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLShaderStage.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/MemoryView.hpp>
#include <Nazara/OpenGLRenderer/Utils.hpp>
#include <Nazara/Shader/GlslWriter.hpp>
#include <Nazara/Shader/ShaderAst.hpp>
#include <Nazara/Shader/ShaderAstSerializer.hpp>
#include <stdexcept>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	OpenGLShaderStage::OpenGLShaderStage(OpenGLDevice& device, const ShaderAst& shaderAst, const ShaderWriter::States& states)
	{
		if (!m_shader.Create(device, ToOpenGL(shaderAst.GetStage())))
			throw std::runtime_error("failed to create shader"); //< TODO: Handle error message

		Create(device, shaderAst, states);
		CheckCompilationStatus();
	}

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

			case ShaderLanguage::NazaraBinary:
			{
				auto shader = UnserializeShader(source, sourceSize);
				if (shader.GetStage() != type)
					throw std::runtime_error("incompatible shader stage");

				Create(device, shader, {});
				break;
			}

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

		CheckCompilationStatus();
	}

	void OpenGLShaderStage::CheckCompilationStatus()
	{
		std::string errorLog;
		if (!m_shader.GetCompilationStatus(&errorLog))
			throw std::runtime_error("Failed to compile shader: " + errorLog);
	}

	void OpenGLShaderStage::Create(OpenGLDevice& device, const ShaderAst& shaderAst, const ShaderWriter::States& states)
	{
		const auto& context = device.GetReferenceContext();
		const auto& contextParams = context.GetParams();

		GlslWriter::Environment env;
		env.glES = (contextParams.type == GL::ContextType::OpenGL_ES);
		env.glMajorVersion = contextParams.glMajorVersion;
		env.glMinorVersion = contextParams.glMinorVersion;
		env.extCallback = [&](const std::string_view& ext)
		{
			return context.IsExtensionSupported(std::string(ext));
		};
		env.flipYPosition = true;

		GlslWriter writer;
		writer.SetEnv(env);

		std::string code = writer.Generate(shaderAst, states);

		NazaraError(code);

		m_shader.SetSource(code.data(), code.size());
		m_shader.Compile();
	}
}
