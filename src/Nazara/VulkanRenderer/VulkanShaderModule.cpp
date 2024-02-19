// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/VulkanRenderer/VulkanShaderModule.hpp>
#include <NZSL/Lexer.hpp>
#include <NZSL/Parser.hpp>
#include <NZSL/SpirvWriter.hpp>
#include <NZSL/Ast/AstSerializer.hpp>
#include <NZSL/SpirV/SpirvDecoder.hpp>

namespace Nz
{
	namespace
	{
		struct SpirvEntryPointExtractor : nzsl::SpirvDecoder
		{
			struct EntryPoint
			{
				nzsl::SpirvExecutionModel executionModel;
				std::string name;
			};

			std::vector<EntryPoint> entryPoints;

			bool HandleOpcode(const nzsl::SpirvInstruction& instruction, UInt32 /*wordCount*/) override
			{
				switch (instruction.op)
				{
					// All instructions that can appear before OpEntryPoint
					case nzsl::SpirvOp::OpCapability:
					case nzsl::SpirvOp::OpExtension:
					case nzsl::SpirvOp::OpExtInstImport:
					case nzsl::SpirvOp::OpMemoryModel:
						return true;

					case nzsl::SpirvOp::OpEntryPoint:
					{
						nzsl::SpirvExecutionModel executionModel = static_cast<nzsl::SpirvExecutionModel>(ReadWord());
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

	bool VulkanShaderModule::Create(Vk::Device& device, nzsl::ShaderStageTypeFlags shaderStages, const nzsl::Ast::Module& shaderModule, const nzsl::ShaderWriter::States& states)
	{
		nzsl::SpirvWriter::Environment env;

		nzsl::SpirvWriter writer;
		writer.SetEnv(env);

		std::vector<UInt32> code = writer.Generate(shaderModule, states);
		return Create(device, shaderStages, ShaderLanguage::SpirV, code.data(), code.size() * sizeof(UInt32), {});
	}

	bool VulkanShaderModule::Create(Vk::Device& device, nzsl::ShaderStageTypeFlags shaderStages, ShaderLanguage lang, const void* source, std::size_t sourceSize, const nzsl::ShaderWriter::States& states)
	{
		switch (lang)
		{
			case ShaderLanguage::GLSL:
			case ShaderLanguage::HLSL:
			case ShaderLanguage::MSL:
				break;

			case ShaderLanguage::NazaraBinary:
			{
				nzsl::Unserializer unserializer(source, sourceSize);
				auto shader = nzsl::Ast::UnserializeShader(unserializer);
				return Create(device, shaderStages, *shader, states);
			}

			case ShaderLanguage::NazaraShader:
			{
				std::vector<nzsl::Token> tokens = nzsl::Tokenize(std::string_view(static_cast<const char*>(source), sourceSize));

				nzsl::Parser parser;
				nzsl::Ast::ModulePtr shaderModule = parser.Parse(tokens);
				return Create(device, shaderStages, *shaderModule, states);
			}

			case ShaderLanguage::SpirV:
			{
				NazaraAssertFmt(sourceSize % sizeof(UInt32) == 0, "sourceSize must be a multiple of sizeof(UInt32), got {0}", sourceSize);

				SpirvEntryPointExtractor extractor;
				extractor.Decode(static_cast<const UInt32*>(source), sourceSize / sizeof(UInt32));

				nzsl::ShaderStageTypeFlags remainingStages = shaderStages;
				for (auto& entryPoint : extractor.entryPoints)
				{
					nzsl::ShaderStageType stageType;
					switch (entryPoint.executionModel)
					{
						case nzsl::SpirvExecutionModel::GLCompute: stageType = nzsl::ShaderStageType::Compute;  break;
						case nzsl::SpirvExecutionModel::Fragment:  stageType = nzsl::ShaderStageType::Fragment; break;
						case nzsl::SpirvExecutionModel::Vertex:    stageType = nzsl::ShaderStageType::Vertex;   break;
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

	void VulkanShaderModule::UpdateDebugName(std::string_view name)
	{
		m_shaderModule.SetDebugName(name);
	}
}
