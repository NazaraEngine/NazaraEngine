// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanShaderModule.hpp>
#include <Nazara/Shader/ShaderLangLexer.hpp>
#include <Nazara/Shader/ShaderLangParser.hpp>
#include <Nazara/Shader/SpirvDecoder.hpp>
#include <Nazara/Shader/SpirvWriter.hpp>
#include <Nazara/Shader/Ast/AstSerializer.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	namespace
	{
		struct SpirvEntryPointExtractor : SpirvDecoder
		{
			struct EntryPoint
			{
				SpirvExecutionModel executionModel;
				std::string name;
			};

			std::vector<EntryPoint> entryPoints;

			bool HandleOpcode(const SpirvInstruction& instruction, UInt32 /*wordCount*/) override
			{
				switch (instruction.op)
				{
					// All instructions that can appear before OpEntryPoint
					case SpirvOp::OpCapability:
					case SpirvOp::OpExtension:
					case SpirvOp::OpExtInstImport:
					case SpirvOp::OpMemoryModel:
						return true;

					case SpirvOp::OpEntryPoint:
					{
						SpirvExecutionModel executionModel = static_cast<SpirvExecutionModel>(ReadWord());
						ReadWord(); // func id
						std::string name = ReadString();

						entryPoints.push_back({
							executionModel,
							std::move(name)
						});

						return true;
					}

					// Return false for other instructions (which means OpEntryPoint will no longer appear from here)
					default:
						return false;
				}
			}
		};
	}

	bool VulkanShaderModule::Create(Vk::Device& device, ShaderStageTypeFlags shaderStages, const ShaderAst::Module& shaderModule, const ShaderWriter::States& states)
	{
		SpirvWriter::Environment env;

		SpirvWriter writer;
		writer.SetEnv(env);

		std::vector<UInt32> code = writer.Generate(shaderModule, states);
		return Create(device, shaderStages, ShaderLanguage::SpirV, code.data(), code.size() * sizeof(UInt32), {});
	}

	bool VulkanShaderModule::Create(Vk::Device& device, ShaderStageTypeFlags shaderStages, ShaderLanguage lang, const void* source, std::size_t sourceSize, const ShaderWriter::States& states)
	{
		switch (lang)
		{
			case ShaderLanguage::GLSL:
			case ShaderLanguage::HLSL:
			case ShaderLanguage::MSL:
				break;

			case ShaderLanguage::NazaraBinary:
			{
				auto shader = ShaderAst::UnserializeShader(source, sourceSize);
				return Create(device, shaderStages, *shader, {});
			}

			case ShaderLanguage::NazaraShader:
			{
				std::vector<ShaderLang::Token> tokens = ShaderLang::Tokenize(std::string_view(static_cast<const char*>(source), sourceSize));

				ShaderLang::Parser parser;
				ShaderAst::ModulePtr shaderModule = parser.Parse(tokens);
				return Create(device, shaderStages, *shaderModule, states);
			}

			case ShaderLanguage::SpirV:
			{
				SpirvEntryPointExtractor extractor;
				extractor.Decode(reinterpret_cast<const UInt32*>(source), sourceSize / sizeof(UInt32));

				ShaderStageTypeFlags remainingStages = shaderStages;
				for (auto& entryPoint : extractor.entryPoints)
				{
					ShaderStageType stageType;
					switch (entryPoint.executionModel)
					{
						case SpirvExecutionModel::Fragment:
							stageType = ShaderStageType::Fragment;
							break;

						case SpirvExecutionModel::Vertex:
							stageType = ShaderStageType::Vertex;
							break;

						default:
							continue; //< Ignore
					}

					if (!remainingStages.Test(stageType))
						continue;

					m_stages.push_back({
						stageType,
						std::move(entryPoint.name)
					});

					remainingStages.Clear(stageType);
				}

				if (remainingStages != 0)
				{
					NazaraError("Vulkan shader module does not handle all requested stage types");
					return false;
				}

				if (!m_shaderModule.Create(device, reinterpret_cast<const UInt32*>(source), sourceSize))
				{
					NazaraError("failed to create shader module");
					return false;
				}

				return true;
			}
		}

		NazaraError("this language is not supported");
		return false;
	}
}

#if defined(NAZARA_PLATFORM_WINDOWS)
#include <Nazara/Core/AntiWindows.hpp>
#endif
