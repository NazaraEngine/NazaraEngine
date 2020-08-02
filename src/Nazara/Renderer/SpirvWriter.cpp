// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/SpirvWriter.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Renderer/ShaderValidator.hpp>
#include <tsl/ordered_map.h>
#include <SpirV/spirv.h>
#include <SpirV/GLSL.std.450.h>
#include <cassert>
#include <stdexcept>
#include <type_traits>
#include <vector>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	namespace
	{
		class PreVisitor : public ShaderRecursiveVisitor, public ShaderVarVisitor
		{
			public:
				using BuiltinContainer = std::unordered_set<std::shared_ptr<const ShaderNodes::BuiltinVariable>>;
				using ExtInstList = std::unordered_set<std::string>;
				using LocalContainer = std::unordered_set<std::shared_ptr<const ShaderNodes::LocalVariable>>;
				using ParameterContainer = std::unordered_set< std::shared_ptr<const ShaderNodes::ParameterVariable>>;

				using ShaderRecursiveVisitor::Visit;
				using ShaderVarVisitor::Visit;

				void Visit(const ShaderNodes::DeclareVariable& node) override
				{
					Visit(node.variable);
				}

				void Visit(const ShaderNodes::Identifier& node) override
				{
					Visit(node.var);
				}

				void Visit(const ShaderNodes::IntrinsicCall& node) override
				{
					ShaderRecursiveVisitor::Visit(node);

					switch (node.intrinsic)
					{
						// Require GLSL.std.450
						case ShaderNodes::IntrinsicType::CrossProduct:
							extInsts.emplace("GLSL.std.450");
							break;

						// Part of SPIR-V core
						case ShaderNodes::IntrinsicType::DotProduct:
							break;
					}
				}

				void Visit(const ShaderNodes::BuiltinVariable& var) override
				{
					builtinVars.insert(std::static_pointer_cast<const ShaderNodes::BuiltinVariable>(var.shared_from_this()));
				}

				void Visit(const ShaderNodes::InputVariable& var) override
				{
					/* Handled by ShaderAst */
				}

				void Visit(const ShaderNodes::LocalVariable& var) override
				{
					localVars.insert(std::static_pointer_cast<const ShaderNodes::LocalVariable>(var.shared_from_this()));
				}

				void Visit(const ShaderNodes::OutputVariable& var) override
				{
					/* Handled by ShaderAst */
				}

				void Visit(const ShaderNodes::ParameterVariable& var) override
				{
					paramVars.insert(std::static_pointer_cast<const ShaderNodes::ParameterVariable>(var.shared_from_this()));
				}

				void Visit(const ShaderNodes::UniformVariable& var) override
				{
					/* Handled by ShaderAst */
				}

				BuiltinContainer builtinVars;
				ExtInstList extInsts;
				LocalContainer localVars;
				ParameterContainer paramVars;
		};
	}

	struct SpirvWriter::Opcode
	{
		SpvOp op;
	};

	struct SpirvWriter::State
	{
		std::size_t boundIndex;
		std::unordered_map<std::string, UInt32> extensionInstructions;
		std::unordered_map<ShaderNodes::BuiltinEntry, UInt32> builtinIds;
		tsl::ordered_map<ShaderExpressionType, UInt32> typeIds;
		std::vector<UInt32> funcIds;
		std::vector<UInt32> funcTypeIds;
		std::vector<UInt32> inputIds;
		std::vector<UInt32> outputIds;
		std::vector<UInt32> uniformIds;
		UInt32 nextVarIndex = 1;

		// Output
		std::vector<UInt32>* output;
		std::vector<UInt32> header;
		std::vector<UInt32> info;
		std::vector<UInt32> instructions;
	};

	SpirvWriter::SpirvWriter() :
	m_currentState(nullptr)
	{
	}

	std::vector<UInt32> SpirvWriter::Generate(const ShaderAst& shader)
	{
		std::string error;
		if (!ValidateShader(shader, &error))
			throw std::runtime_error("Invalid shader AST: " + error);

		m_context.shader = &shader;

		State state;
		m_currentState = &state;
		CallOnExit onExit([this]()
		{
			m_currentState = nullptr;
		});

		PreVisitor preVisitor;
		for (const auto& func : shader.GetFunctions())
			preVisitor.Visit(func.statement);

		// Register all extended instruction sets
		for (const std::string& extInst : preVisitor.extInsts)
			m_currentState->extensionInstructions[extInst] = AllocateResultId();

		// Register all types
		state.output = &state.instructions;

		for (const auto& func : shader.GetFunctions())
		{
			ProcessType(func.returnType);
			for (const auto& param : func.parameters)
				ProcessType(param.type);

			m_currentState->funcTypeIds.push_back(AllocateResultId());
		}

		for (const auto& input : shader.GetInputs())
			ProcessType(input.type);

		for (const auto& output : shader.GetOutputs())
			ProcessType(output.type);

		for (const auto& uniform : shader.GetUniforms())
			ProcessType(uniform.type);

		for (const auto& local : preVisitor.localVars)
			ProcessType(local->type);

		// Register result id and debug infos for global variables/functions
		state.output = &state.info;

		for (const auto& input : shader.GetInputs())
		{
			UInt32 resultId = AllocateResultId();
			Append(Opcode{ SpvOpName }, resultId, input.name);

			m_currentState->inputIds.push_back(resultId);
		}

		for (const auto& output : shader.GetOutputs())
		{
			UInt32 resultId = AllocateResultId();
			Append(Opcode{ SpvOpName }, resultId, output.name);

			m_currentState->outputIds.push_back(resultId);
		}

		for (const auto& uniform : shader.GetUniforms())
		{
			UInt32 resultId = AllocateResultId();
			Append(Opcode{ SpvOpName }, resultId, uniform.name);

			m_currentState->uniformIds.push_back(resultId);
		}

		for (const auto& func : shader.GetFunctions())
		{
			UInt32 resultId = AllocateResultId();
			Append(Opcode{ SpvOpName }, resultId, func.name);

			m_currentState->funcIds.push_back(resultId);
		}

		state.output = &state.header;

		AppendHeader();

		/*assert(m_context.shader);
		switch (m_context.shader->GetStage())
		{
			case ShaderStageType::Fragment:
				break;
			case ShaderStageType::Vertex:
				break;

			default:
				break;
		}*/

		state.header[state.boundIndex] = state.nextVarIndex;

		std::vector<UInt32> ret;
		ret.reserve(state.header.size() + state.info.size() + state.instructions.size());

		MergeBlocks(ret, state.header);
		MergeBlocks(ret, state.info);
		MergeBlocks(ret, state.instructions);

		return ret;
	}

	void SpirvWriter::SetEnv(Environment environment)
	{
		m_environment = std::move(environment);
	}

	std::size_t Nz::SpirvWriter::Append(UInt32 value)
	{
		std::size_t offset = GetOutputOffset();
		m_currentState->output->push_back(value);

		return offset;
	}

	std::size_t SpirvWriter::Append(const Opcode& opcode, unsigned int wordCount)
	{
		return Append(UInt32(opcode.op) | UInt32(wordCount) << 16);
	}

	UInt32 Nz::SpirvWriter::AllocateResultId()
	{
		return m_currentState->nextVarIndex++;
	}

	void SpirvWriter::AppendHeader()
	{
		Append(SpvMagicNumber); //< Spir-V magic number

		UInt32 version = (m_environment.spvMajorVersion << 16) | m_environment.spvMinorVersion << 8;
		Append(version); //< Spir-V version number (1.0 for compatibility)
		Append(0); //< Generator identifier (TODO: Register generator to Khronos)

		m_currentState->boundIndex = Append(0); //< Bound (ID count), will be filled later
		Append(0); //< Instruction schema (required to be 0 for now)

		Append(Opcode{ SpvOpCapability }, SpvCapabilityShader);

		for (const auto& [extInst, resultId] : m_currentState->extensionInstructions)
			Append(Opcode{ SpvOpExtInstImport }, resultId, extInst);

		Append(Opcode{ SpvOpMemoryModel }, SpvAddressingModelLogical, SpvMemoryModelGLSL450);
	}

	void SpirvWriter::AppendTypes()
	{
		for (const auto& [type, typeId] : m_currentState->typeIds.values_container())
		{
			UInt32 resultId = typeId;

			// Register sub-types, if any
			std::visit([&](auto&& arg)
			{
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, ShaderNodes::BasicType>)
				{
					// In SPIR-V, vec3 (for example) depends on float
					UInt32 depResultId;
					if (ShaderNodes::Node::GetComponentCount(arg) != 1)
						depResultId = ProcessType(ShaderNodes::Node::GetComponentType(arg));

					switch (arg)
					{
						case ShaderNodes::BasicType::Boolean:
							Append(Opcode{ SpvOpTypeBool }, resultId);
							break;

						case ShaderNodes::BasicType::Float1:
							Append(Opcode{ SpvOpTypeFloat }, resultId);
							break;

						case ShaderNodes::BasicType::Float2:
						case ShaderNodes::BasicType::Float3:
						case ShaderNodes::BasicType::Float4:
						case ShaderNodes::BasicType::Mat4x4:
						case ShaderNodes::BasicType::Sampler2D:
						case ShaderNodes::BasicType::Void:
							Append(Opcode{ SpvOpTypeVoid }, resultId);
							break;
					}
				}
				else if constexpr (std::is_same_v<T, std::string>)
				{
					// Register struct members type
					const auto& structs = m_context.shader->GetStructs();
					auto it = std::find_if(structs.begin(), structs.end(), [&](const auto& s) { return s.name == arg; });
					if (it == structs.end())
						throw std::runtime_error("struct " + arg + " has not been defined");

					const ShaderAst::Struct& s = *it;
					for (const auto& member : s.members)
						ProcessType(member.type);
				}
				else
					static_assert(AlwaysFalse<T>::value, "non-exhaustive visitor");
			}, type);
		}
	}

	std::size_t SpirvWriter::GetOutputOffset() const
	{
		assert(m_currentState);
		return m_currentState->output->size();
	}

	UInt32 SpirvWriter::ProcessType(ShaderExpressionType type)
	{
		auto it = m_currentState->typeIds.find(type);
		if (it == m_currentState->typeIds.end())
		{
			// Register sub-types, if any
			std::visit([&](auto&& arg)
			{
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, ShaderNodes::BasicType>)
				{
					// In SPIR-V, vec3 (for example) depends on float
					if (ShaderNodes::Node::GetComponentCount(arg) != 1)
						ProcessType(ShaderNodes::Node::GetComponentType(arg));
				}
				else if constexpr (std::is_same_v<T, std::string>)
				{
					// Register struct members type
					const auto& structs = m_context.shader->GetStructs();
					auto it = std::find_if(structs.begin(), structs.end(), [&](const auto& s) { return s.name == arg; });
					if (it == structs.end())
						throw std::runtime_error("struct " + arg + " has not been defined");

					const ShaderAst::Struct& s = *it;
					for (const auto& member : s.members)
						ProcessType(member.type);
				}
				else
					static_assert(AlwaysFalse<T>::value, "non-exhaustive visitor");
			}, type);

			it = m_currentState->typeIds.emplace(std::move(type), AllocateResultId()).first;
		}

		return it->second;
	}

	void SpirvWriter::Visit(const ShaderNodes::ExpressionPtr& expr, bool encloseIfRequired)
	{
	}
	void SpirvWriter::Visit(const ShaderNodes::AccessMember& /*node*/)
	{
	}
	void SpirvWriter::Visit(const ShaderNodes::AssignOp& /*node*/)
	{
	}
	void SpirvWriter::Visit(const ShaderNodes::Branch& /*node*/)
	{
	}
	void SpirvWriter::Visit(const ShaderNodes::BinaryOp& /*node*/)
	{
	}

	void SpirvWriter::Visit(const ShaderNodes::Cast& /*node*/)
	{
	}
	void SpirvWriter::Visit(const ShaderNodes::Constant& /*node*/)
	{
	}
	void SpirvWriter::Visit(const ShaderNodes::DeclareVariable& /*node*/)
	{
	}
	void SpirvWriter::Visit(const ShaderNodes::ExpressionStatement& /*node*/)
	{
	}
	void SpirvWriter::Visit(const ShaderNodes::Identifier& /*node*/)
	{
	}

	void SpirvWriter::Visit(const ShaderNodes::IntrinsicCall& /*node*/)
	{
	}

	void SpirvWriter::Visit(const ShaderNodes::Sample2D& /*node*/)
	{
	}
	void SpirvWriter::Visit(const ShaderNodes::StatementBlock& /*node*/)
	{
	}
	void SpirvWriter::Visit(const ShaderNodes::SwizzleOp& /*node*/)
	{
	}

	void SpirvWriter::MergeBlocks(std::vector<UInt32>& output, const std::vector<UInt32>& from)
	{
		std::size_t prevSize = output.size();
		output.resize(prevSize + from.size());
		std::copy(from.begin(), from.end(), output.begin() + prevSize);
	}
}
