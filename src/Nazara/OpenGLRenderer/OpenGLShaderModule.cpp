// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/OpenGLRenderer/OpenGLShaderModule.hpp>
#include <Nazara/Core/MemoryView.hpp>
#include <Nazara/OpenGLRenderer/Utils.hpp>
#include <NZSL/Lexer.hpp>
#include <NZSL/Parser.hpp>
#include <NZSL/Ast/AstSerializer.hpp>
#include <stdexcept>

namespace Nz
{
	OpenGLShaderModule::OpenGLShaderModule(OpenGLDevice& device, nzsl::ShaderStageTypeFlags shaderStages, const nzsl::Ast::Module& shaderModule, const nzsl::ShaderWriter::States& states) :
	m_device(device)
	{
		NazaraAssert(shaderStages != 0, "at least one shader stage must be specified");
		Create(device, shaderStages, shaderModule, states);
	}

	OpenGLShaderModule::OpenGLShaderModule(OpenGLDevice& device, nzsl::ShaderStageTypeFlags shaderStages, ShaderLanguage lang, const void* source, std::size_t sourceSize, const nzsl::ShaderWriter::States& states) :
	m_device(device)
	{
		NazaraAssert(shaderStages != 0, "at least one shader stage must be specified");

		switch (lang)
		{
			case ShaderLanguage::GLSL:
			{
				for (nzsl::ShaderStageType shaderStage : shaderStages)
				{
					NazaraAssert(shaderStages == shaderStage, "when supplying GLSL, only one shader stage type can be specified");

					auto& entry = m_shaders.emplace_back();
					entry.shader = GlslShader{ std::string(static_cast<const char*>(source), sourceSize) };
					entry.stage = shaderStage;
					break;
				}

				break;
			}

			case ShaderLanguage::NazaraBinary:
			{
				nzsl::Deserializer deserializer(source, sourceSize);
				auto shader = nzsl::Ast::DeserializeShader(deserializer);
				Create(device, shaderStages, *shader, states);
				break;
			}

			case ShaderLanguage::NazaraShader:
			{
				std::vector<nzsl::Token> tokens = nzsl::Tokenize(std::string_view(static_cast<const char*>(source), sourceSize));

				nzsl::Parser parser;
				nzsl::Ast::ModulePtr module = parser.Parse(tokens);
				Create(device, shaderStages, *module, states);
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

	nzsl::ShaderStageTypeFlags OpenGLShaderModule::Attach(GL::Program& program, const nzsl::GlslWriter::BindingMapping& bindingMapping, std::vector<ExplicitBinding>* explicitBindings) const
	{
		const auto& context = m_device.GetReferenceContext();
		const auto& contextParams = context.GetParams();

		nzsl::GlslWriter::Environment env;
		env.glES = (contextParams.type == GL::ContextType::OpenGL_ES);
		env.glMajorVersion = contextParams.glMajorVersion;
		env.glMinorVersion = contextParams.glMinorVersion;
		env.extCallback = [&](std::string_view ext)
		{
			return context.IsExtensionSupported(std::string(ext));
		};

		env.flipYPosition = true;

		if (!context.HasZeroToOneDepth())
			env.remapZPosition = true;

		nzsl::GlslWriter writer;
		writer.SetEnv(env);

		nzsl::ShaderStageTypeFlags stageFlags;
		for (const auto& shaderEntry : m_shaders)
		{
			GL::Shader shader;

			if (!shader.Create(m_device, ToOpenGL(shaderEntry.stage)))
				throw std::runtime_error("failed to create shader"); //< TODO: Handle error message

			if (!m_debugName.empty())
				shader.SetDebugName(m_debugName);

			std::visit([&](auto&& arg)
			{
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, GlslShader>)
					shader.SetSource(arg.sourceCode.data(), GLint(arg.sourceCode.size()));
				else if constexpr (std::is_same_v<T, ShaderStatement>)
				{
					nzsl::GlslWriter::Output output = writer.Generate(shaderEntry.stage, *arg.ast, bindingMapping, m_states);
					shader.SetSource(output.code.data(), GLint(output.code.size()));

					if (explicitBindings)
					{
						explicitBindings->reserve(explicitBindings->size() + output.explicitTextureBinding.size() + output.explicitUniformBlockBinding.size());
						for (const auto& [name, bindingPoint] : output.explicitTextureBinding)
						{
							auto& explicitBinding = explicitBindings->emplace_back();
							explicitBinding.name = name;
							explicitBinding.binding = bindingPoint;
							explicitBinding.isBlock = false;
						}

						for (const auto& [name, bindingPoint] : output.explicitUniformBlockBinding)
						{
							auto& explicitBinding = explicitBindings->emplace_back();
							explicitBinding.name = name;
							explicitBinding.binding = bindingPoint;
							explicitBinding.isBlock = true;
						}
					}
				}
				else
					static_assert(AlwaysFalse<T>::value, "non-exhaustive visitor");

			}, shaderEntry.shader);

			shader.Compile();

			CheckCompilationStatus(shader);

			program.AttachShader(shader.GetObjectId());
			// Shader object can be safely released now (it won't be deleted by the driver until program gets deleted)

			stageFlags |= shaderEntry.stage;
		}

		return stageFlags;
	}

	void OpenGLShaderModule::UpdateDebugName(std::string_view name)
	{
		m_debugName = name;
	}

	void OpenGLShaderModule::Create(OpenGLDevice& /*device*/, nzsl::ShaderStageTypeFlags shaderStages, const nzsl::Ast::Module& shaderModule, const nzsl::ShaderWriter::States& states)
	{
		m_states = states;
		m_states.sanitized = true; //< Shader is always sanitized (because of keywords)

		/*
		Always remove dead code with OpenGL (prevents errors on draw calls when no buffer is bound on a unused binding),
		also prevents compilation failure because of functions using discard in a vertex shader
		*/
		m_states.optimize = true;

		nzsl::Ast::SanitizeVisitor::Options options = nzsl::GlslWriter::GetSanitizeOptions();
		options.optionValues = states.optionValues;
		options.moduleResolver = states.shaderModuleResolver;

		nzsl::Ast::ModulePtr sanitized = nzsl::Ast::Sanitize(shaderModule, options);

		for (nzsl::ShaderStageType shaderStage : shaderStages)
		{
			auto& entry = m_shaders.emplace_back();
			entry.shader = ShaderStatement{ sanitized };
			entry.stage = shaderStage;
		}
	}

	void OpenGLShaderModule::CheckCompilationStatus(GL::Shader& shader)
	{
		std::string errorLog;
		if (!shader.GetCompilationStatus(&errorLog))
		{
			std::string source = shader.GetSource();

			throw std::runtime_error("Failed to compile shader: " + errorLog + "\nSource: " + source);
		}
	}
}
