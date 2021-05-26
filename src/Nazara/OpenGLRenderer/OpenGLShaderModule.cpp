// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLShaderModule.hpp>
#include <Nazara/Core/MemoryView.hpp>
#include <Nazara/OpenGLRenderer/Utils.hpp>
#include <Nazara/Shader/GlslWriter.hpp>
#include <Nazara/Shader/ShaderLangLexer.hpp>
#include <Nazara/Shader/ShaderLangParser.hpp>
#include <Nazara/Shader/Ast/AstCloner.hpp>
#include <Nazara/Shader/Ast/AstSerializer.hpp>
#include <stdexcept>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	OpenGLShaderModule::OpenGLShaderModule(OpenGLDevice& device, ShaderStageTypeFlags shaderStages, ShaderAst::StatementPtr& shaderAst, const ShaderWriter::States& states)
	{
		NazaraAssert(shaderStages != 0, "at least one shader stage must be specified");
		Create(device, shaderStages, shaderAst, states);
	}

	OpenGLShaderModule::OpenGLShaderModule(OpenGLDevice& device, ShaderStageTypeFlags shaderStages, ShaderLanguage lang, const void* source, std::size_t sourceSize, const ShaderWriter::States& states)
	{
		NazaraAssert(shaderStages != 0, "at least one shader stage must be specified");

		switch (lang)
		{
			case ShaderLanguage::GLSL:
			{
				for (std::size_t i = 0; i < ShaderStageTypeCount; ++i)
				{
					ShaderStageType shaderStage = static_cast<ShaderStageType>(i);
					if (shaderStages.Test(shaderStage))
					{
						NazaraAssert(shaderStages == shaderStage, "when supplying GLSL, only one shader stage type can be specified");

						GL::Shader shader;
						if (!shader.Create(device, ToOpenGL(shaderStage)))
							throw std::runtime_error("failed to create shader"); //< TODO: Handle error message

						shader.SetSource(reinterpret_cast<const char*>(source), GLint(sourceSize));
						shader.Compile();
						CheckCompilationStatus(shader);

						m_shaders.emplace_back(std::move(shader));
						break;
					}
				}

				break;
			}

			case ShaderLanguage::NazaraBinary:
			{
				auto shader = ShaderAst::UnserializeShader(source, sourceSize);
				Create(device, shaderStages, shader, {});
				break;
			}

			case ShaderLanguage::NazaraShader:
			{
				std::vector<Nz::ShaderLang::Token> tokens = Nz::ShaderLang::Tokenize(std::string_view(static_cast<const char*>(source), sourceSize));

				Nz::ShaderLang::Parser parser;
				Nz::ShaderAst::StatementPtr shaderAst = parser.Parse(tokens);
				Create(device, shaderStages, shaderAst, states);
				break;
			}

			case ShaderLanguage::SpirV:
			{
				throw std::runtime_error("TODO");

				// TODO: Parse SpirV to extract entry points?

				/*if (!device.GetReferenceContext().IsExtensionSupported(GL::Extension::SpirV))
					throw std::runtime_error("SpirV is not supported by this OpenGL implementation");

				m_shader.SetBinarySource(GL_SHADER_BINARY_FORMAT_SPIR_V_ARB, source, GLsizei(sourceSize));
				m_shader.SpecializeShader("main", 0U, nullptr, nullptr);*/
				break;
			}

			default:
				throw std::runtime_error("Unsupported shader language");
		}
	}

	void OpenGLShaderModule::CheckCompilationStatus(GL::Shader& shader)
	{
		std::string errorLog;
		if (!shader.GetCompilationStatus(&errorLog))
			throw std::runtime_error("Failed to compile shader: " + errorLog);
	}

	void OpenGLShaderModule::Create(OpenGLDevice& device, ShaderStageTypeFlags shaderStages, ShaderAst::StatementPtr& shaderAst, const ShaderWriter::States& states)
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

		for (std::size_t i = 0; i < ShaderStageTypeCount; ++i)
		{
			ShaderStageType shaderStage = static_cast<ShaderStageType>(i);
			if (shaderStages.Test(shaderStage))
			{
				GL::Shader shader;

				if (!shader.Create(device, ToOpenGL(shaderStage)))
					throw std::runtime_error("failed to create shader"); //< TODO: Handle error message

				std::string code = writer.Generate(shaderStage, shaderAst, states);

				shader.SetSource(code.data(), GLint(code.size()));
				shader.Compile();

				CheckCompilationStatus(shader);

				m_shaders.emplace_back(std::move(shader));
			}
		}
	}
}
