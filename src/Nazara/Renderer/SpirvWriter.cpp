// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/SpirvWriter.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Endianness.hpp>
#include <Nazara/Renderer/ShaderAstValidator.hpp>
#include <tsl/ordered_map.h>
#include <tsl/ordered_set.h>
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
		using ConstantVariant = ShaderNodes::Constant::Variant;

		class PreVisitor : public ShaderAstRecursiveVisitor, public ShaderVarVisitor
		{
			public:
				using BuiltinContainer = std::unordered_set<std::shared_ptr<const ShaderNodes::BuiltinVariable>>;
				using ConstantContainer = tsl::ordered_set<ConstantVariant>;
				using ExtInstList = std::unordered_set<std::string>;
				using LocalContainer = std::unordered_set<std::shared_ptr<const ShaderNodes::LocalVariable>>;
				using ParameterContainer = std::unordered_set< std::shared_ptr<const ShaderNodes::ParameterVariable>>;

				using ShaderAstRecursiveVisitor::Visit;
				using ShaderVarVisitor::Visit;

				void Visit(const ShaderNodes::Constant& node) override
				{
					std::visit([&](auto&& arg)
					{
						using T = std::decay_t<decltype(arg)>;

						if constexpr (std::is_same_v<T, bool> || std::is_same_v<T, float>)
							constants.emplace(arg);
						else if constexpr (std::is_same_v<T, Vector2f>)
						{
							constants.emplace(arg.x);
							constants.emplace(arg.y);
							constants.emplace(arg);
						}
						else if constexpr (std::is_same_v<T, Vector3f>)
						{
							constants.emplace(arg.x);
							constants.emplace(arg.y);
							constants.emplace(arg.z);
							constants.emplace(arg);
						}
						else if constexpr (std::is_same_v<T, Vector4f>)
						{
							constants.emplace(arg.x);
							constants.emplace(arg.y);
							constants.emplace(arg.z);
							constants.emplace(arg.w);
							constants.emplace(arg);
						}
						else
							static_assert(AlwaysFalse<T>::value, "non-exhaustive visitor");
					},
					node.value);

					ShaderAstRecursiveVisitor::Visit(node);
				}

				void Visit(const ShaderNodes::DeclareVariable& node) override
				{
					Visit(node.variable);

					ShaderAstRecursiveVisitor::Visit(node);
				}

				void Visit(const ShaderNodes::Identifier& node) override
				{
					Visit(node.var);

					ShaderAstRecursiveVisitor::Visit(node);
				}

				void Visit(const ShaderNodes::IntrinsicCall& node) override
				{
					ShaderAstRecursiveVisitor::Visit(node);

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
				ConstantContainer constants;
				ExtInstList extInsts;
				LocalContainer localVars;
				ParameterContainer paramVars;
		};
	}

	struct SpirvWriter::Opcode
	{
		SpvOp op;
	};

	struct SpirvWriter::Raw
	{
		const void* ptr;
		std::size_t size;
	};

	struct SpirvWriter::WordCount
	{
		unsigned int wc;
	};

	struct SpirvWriter::State
	{
		struct Func
		{
			UInt32 typeId;
			UInt32 id;
			std::vector<UInt32> paramsId;
		};

		struct ExtVar
		{
			UInt32 pointerTypeId;
			UInt32 varId;
		};

		std::unordered_map<std::string, UInt32> extensionInstructions;
		std::unordered_map<ShaderNodes::BuiltinEntry, UInt32> builtinIds;
		tsl::ordered_map<ConstantVariant, UInt32> constantIds;
		tsl::ordered_map<ShaderExpressionType, UInt32> typeIds;
		std::vector<Func> funcs;
		std::vector<ExtVar> inputIds;
		std::vector<ExtVar> outputIds;
		std::vector<ExtVar> uniformIds;
		std::vector<std::optional<FieldOffsets>> structFields;
		std::vector<UInt32> resultIds;
		UInt32 nextVarIndex = 1;

		// Output
		Section header;
		Section constants;
		Section debugInfo;
		Section annotations;
		Section types;
		Section instructions;
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

		state.structFields.resize(shader.GetStructCount());
		state.annotations.Append(Opcode{ SpvOpNop });
		state.constants.Append(Opcode{ SpvOpNop });
		state.debugInfo.Append(Opcode{ SpvOpNop });
		state.types.Append(Opcode{ SpvOpNop });

		PreVisitor preVisitor;
		for (const auto& func : shader.GetFunctions())
			preVisitor.Visit(func.statement);

		// Register all extended instruction sets
		for (const std::string& extInst : preVisitor.extInsts)
			state.extensionInstructions[extInst] = AllocateResultId();

		// Register all types
		for (const auto& func : shader.GetFunctions())
		{
			RegisterType(func.returnType);
			for (const auto& param : func.parameters)
				RegisterType(param.type);
		}

		for (const auto& input : shader.GetInputs())
			RegisterType(input.type);

		for (const auto& output : shader.GetOutputs())
			RegisterType(output.type);

		for (const auto& uniform : shader.GetUniforms())
			RegisterType(uniform.type);

		for (const auto& local : preVisitor.localVars)
			RegisterType(local->type);

		// Register constant types
		for (const auto& constant : preVisitor.constants)
		{
			std::visit([&](auto&& arg)
			{
				using T = std::decay_t<decltype(arg)>;

				if constexpr (std::is_same_v<T, bool>)
					RegisterType(ShaderNodes::BasicType::Boolean);
				else if constexpr (std::is_same_v<T, float>)
					RegisterType(ShaderNodes::BasicType::Float1);
				else if constexpr (std::is_same_v<T, Vector2f>)
					RegisterType(ShaderNodes::BasicType::Float2);
				else if constexpr (std::is_same_v<T, Vector3f>)
					RegisterType(ShaderNodes::BasicType::Float3);
				else if constexpr (std::is_same_v<T, Vector4f>)
					RegisterType(ShaderNodes::BasicType::Float4);
				else
					static_assert(AlwaysFalse<T>::value, "non-exhaustive visitor");
			}, constant);
		}

		AppendTypes();

		// Register result id and debug infos for global variables/functions
		for (const auto& input : shader.GetInputs())
		{
			auto& inputData = state.inputIds.emplace_back();
			inputData.pointerTypeId = AllocateResultId();
			inputData.varId = AllocateResultId();

			state.debugInfo.Append(Opcode{ SpvOpName }, inputData.varId, input.name);
			state.types.Append(Opcode{ SpvOpTypePointer }, inputData.pointerTypeId, SpvStorageClassInput, GetTypeId(input.type));
			state.types.Append(Opcode{ SpvOpVariable }, inputData.pointerTypeId, inputData.varId, SpvStorageClassInput);

			if (input.locationIndex)
				state.annotations.Append(Opcode{ SpvOpDecorate }, inputData.varId, SpvDecorationLocation, *input.locationIndex);
		}

		for (const auto& output : shader.GetOutputs())
		{
			auto& outputData = state.outputIds.emplace_back();
			outputData.pointerTypeId = AllocateResultId();
			outputData.varId = AllocateResultId();

			state.debugInfo.Append(Opcode{ SpvOpName }, outputData.varId, output.name);
			state.types.Append(Opcode{ SpvOpTypePointer }, outputData.pointerTypeId, SpvStorageClassOutput, GetTypeId(output.type));
			state.types.Append(Opcode{ SpvOpVariable }, outputData.pointerTypeId, outputData.varId, SpvStorageClassOutput);

			if (output.locationIndex)
				state.annotations.Append(Opcode{ SpvOpDecorate }, outputData.varId, SpvDecorationLocation, *output.locationIndex);
		}

		for (const auto& uniform : shader.GetUniforms())
		{
			auto& uniformData = state.uniformIds.emplace_back();
			uniformData.pointerTypeId = AllocateResultId();
			uniformData.varId = AllocateResultId();

			state.debugInfo.Append(Opcode{ SpvOpName }, uniformData.varId, uniform.name);
			state.types.Append(Opcode{ SpvOpTypePointer }, uniformData.pointerTypeId, SpvStorageClassUniform, GetTypeId(uniform.type));
			state.types.Append(Opcode{ SpvOpVariable }, uniformData.pointerTypeId, uniformData.varId, SpvStorageClassUniform);

			if (uniform.bindingIndex)
			{
				state.annotations.Append(Opcode{ SpvOpDecorate }, uniformData.varId, SpvDecorationBinding, *uniform.bindingIndex);
				state.annotations.Append(Opcode{ SpvOpDecorate }, uniformData.varId, SpvDecorationDescriptorSet, 0);
			}
		}

		for (const auto& func : shader.GetFunctions())
		{
			auto& funcData = state.funcs.emplace_back();
			funcData.id = AllocateResultId();
			funcData.typeId = AllocateResultId();

			state.debugInfo.Append(Opcode{ SpvOpName }, funcData.id, func.name);

			state.types.Append(Opcode{ SpvOpTypeFunction }, WordCount{ 3 + static_cast<unsigned int>(func.parameters.size()) });
			state.types.Append(funcData.typeId);
			state.types.Append(GetTypeId(func.returnType));

			for (const auto& param : func.parameters)
				state.types.Append(GetTypeId(param.type));
		}

		// Register constants
		for (const auto& constant : preVisitor.constants)
			state.constantIds[constant] = AllocateResultId();

		AppendConstants();

		for (std::size_t funcIndex = 0; funcIndex < shader.GetFunctionCount(); ++funcIndex)
		{
			const auto& func = shader.GetFunction(funcIndex);

			auto& funcData = state.funcs[funcIndex];

			state.instructions.Append(Opcode{ SpvOpNop });

			state.instructions.Append(Opcode{ SpvOpFunction }, GetTypeId(func.returnType), funcData.id, 0, funcData.typeId);

			for (const auto& param : func.parameters)
			{
				UInt32 paramResultId = AllocateResultId();
				funcData.paramsId.push_back(paramResultId);

				state.instructions.Append(Opcode{ SpvOpFunctionParameter }, GetTypeId(param.type), paramResultId);
			}

			Visit(func.statement);

			state.instructions.Append(Opcode{ SpvOpFunctionEnd });
		}

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

		std::vector<UInt32> ret;
		MergeBlocks(ret, state.header);
		MergeBlocks(ret, state.debugInfo);
		MergeBlocks(ret, state.annotations);
		MergeBlocks(ret, state.types);
		MergeBlocks(ret, state.constants);
		MergeBlocks(ret, state.instructions);

		return ret;
	}

	void SpirvWriter::SetEnv(Environment environment)
	{
		m_environment = std::move(environment);
	}

	UInt32 Nz::SpirvWriter::AllocateResultId()
	{
		return m_currentState->nextVarIndex++;
	}

	void SpirvWriter::AppendConstants()
	{
		for (const auto& [value, resultId] : m_currentState->constantIds)
		{
			UInt32 constantId = resultId;
			std::visit([&](auto&& arg)
			{
				using T = std::decay_t<decltype(arg)>;

				if constexpr (std::is_same_v<T, bool>)
					m_currentState->constants.Append(Opcode{ (arg) ? SpvOpConstantTrue : SpvOpConstantFalse }, constantId);
				else if constexpr (std::is_same_v<T, float>)
					m_currentState->constants.Append(Opcode{ SpvOpConstant }, GetTypeId(ShaderNodes::BasicType::Float1), constantId, Raw{ &arg, sizeof(arg) });
				else if constexpr (std::is_same_v<T, Vector2f>)
					m_currentState->constants.Append(Opcode{ SpvOpConstantComposite }, GetTypeId(ShaderNodes::BasicType::Float2), constantId, GetConstantId(arg.x), GetConstantId(arg.y));
				else if constexpr (std::is_same_v<T, Vector3f>)
					m_currentState->constants.Append(Opcode{ SpvOpConstantComposite }, GetTypeId(ShaderNodes::BasicType::Float3), constantId, GetConstantId(arg.x), GetConstantId(arg.y), GetConstantId(arg.z));
				else if constexpr (std::is_same_v<T, Vector4f>)
					m_currentState->constants.Append(Opcode{ SpvOpConstantComposite }, GetTypeId(ShaderNodes::BasicType::Float3), constantId, GetConstantId(arg.x), GetConstantId(arg.y), GetConstantId(arg.z), GetConstantId(arg.w));
				else
					static_assert(AlwaysFalse<T>::value, "non-exhaustive visitor");
			}, value);
		}
	}

	void SpirvWriter::AppendHeader()
	{
		m_currentState->header.Append(SpvMagicNumber); //< Spir-V magic number

		UInt32 version = (m_environment.spvMajorVersion << 16) | m_environment.spvMinorVersion << 8;
		m_currentState->header.Append(version); //< Spir-V version number (1.0 for compatibility)
		m_currentState->header.Append(0); //< Generator identifier (TODO: Register generator to Khronos)

		m_currentState->header.Append(m_currentState->nextVarIndex); //< Bound (ID count)
		m_currentState->header.Append(0); //< Instruction schema (required to be 0 for now)

		m_currentState->header.Append(Opcode{ SpvOpCapability }, SpvCapabilityShader);

		for (const auto& [extInst, resultId] : m_currentState->extensionInstructions)
			m_currentState->header.Append(Opcode{ SpvOpExtInstImport }, resultId, extInst);

		m_currentState->header.Append(Opcode{ SpvOpMemoryModel }, SpvAddressingModelLogical, SpvMemoryModelGLSL450);
	}

	void SpirvWriter::AppendStructType(std::size_t structIndex, UInt32 resultId)
	{
		const ShaderAst::Struct& s = m_context.shader->GetStruct(structIndex);

		m_currentState->types.Append(Opcode{ SpvOpTypeStruct }, WordCount{ static_cast<unsigned int>(1 + 1 + s.members.size()) });
		m_currentState->types.Append(resultId);

		m_currentState->debugInfo.Append(Opcode{ SpvOpName }, resultId, s.name);

		m_currentState->annotations.Append(Opcode{ SpvOpDecorate }, resultId, SpvDecorationBlock);

		FieldOffsets structOffsets(StructLayout_Std140);

		for (std::size_t memberIndex = 0; memberIndex < s.members.size(); ++memberIndex)
		{
			const auto& member = s.members[memberIndex];
			m_currentState->types.Append(GetTypeId(member.type));
			m_currentState->debugInfo.Append(Opcode{ SpvOpMemberName }, resultId, memberIndex, member.name);

			std::visit([&](auto&& arg)
			{
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, ShaderNodes::BasicType>)
				{
					std::size_t offset = [&] {
						switch (arg)
						{
							case ShaderNodes::BasicType::Boolean: return structOffsets.AddField(StructFieldType_Bool1);
							case ShaderNodes::BasicType::Float1:  return structOffsets.AddField(StructFieldType_Float1);
							case ShaderNodes::BasicType::Float2:  return structOffsets.AddField(StructFieldType_Float2);
							case ShaderNodes::BasicType::Float3:  return structOffsets.AddField(StructFieldType_Float3);
							case ShaderNodes::BasicType::Float4:  return structOffsets.AddField(StructFieldType_Float4);
							case ShaderNodes::BasicType::Mat4x4:  return structOffsets.AddMatrix(StructFieldType_Float1, 4, 4, true);
							case ShaderNodes::BasicType::Sampler2D: throw std::runtime_error("unexpected sampler2D as struct member");
							case ShaderNodes::BasicType::Void: throw std::runtime_error("unexpected void as struct member");
						}

						assert(false);
						throw std::runtime_error("unhandled type");
					}();

					m_currentState->annotations.Append(Opcode{ SpvOpMemberDecorate }, resultId, memberIndex, SpvDecorationOffset, offset);

					if (arg == ShaderNodes::BasicType::Mat4x4)
					{
						m_currentState->annotations.Append(Opcode{ SpvOpMemberDecorate }, resultId, memberIndex, SpvDecorationColMajor);
						m_currentState->annotations.Append(Opcode{ SpvOpMemberDecorate }, resultId, memberIndex, SpvDecorationMatrixStride, 16);
					}
				}
				else if constexpr (std::is_same_v<T, std::string>)
				{
					// Register struct members type
					const auto& structs = m_context.shader->GetStructs();
					auto it = std::find_if(structs.begin(), structs.end(), [&](const auto& s) { return s.name == arg; });
					if (it == structs.end())
						throw std::runtime_error("struct " + arg + " has not been defined");

					std::size_t nestedStructIndex = std::distance(structs.begin(), it);
					std::optional<FieldOffsets> nestedFieldOffset = m_currentState->structFields[nestedStructIndex];
					if (!nestedFieldOffset)
						throw std::runtime_error("struct dependency cycle");

					structOffsets.AddStruct(nestedFieldOffset.value());
				}
				else
					static_assert(AlwaysFalse<T>::value, "non-exhaustive visitor");
			}, member.type);
		}

		m_currentState->structFields[structIndex] = structOffsets;
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
					switch (arg)
					{
						case ShaderNodes::BasicType::Boolean:
							m_currentState->types.Append(Opcode{ SpvOpTypeBool }, resultId);
							break;

						case ShaderNodes::BasicType::Float1:
							m_currentState->types.Append(Opcode{ SpvOpTypeFloat }, resultId, 32);
							break;

						case ShaderNodes::BasicType::Float2:
						case ShaderNodes::BasicType::Float3:
						case ShaderNodes::BasicType::Float4:
						{
							UInt32 vecSize = UInt32(arg) - UInt32(ShaderNodes::BasicType::Float2) + 1;

							m_currentState->types.Append(Opcode{ SpvOpTypeVector }, resultId, GetTypeId(ShaderNodes::BasicType::Float1), vecSize);
							break;
						}

						case ShaderNodes::BasicType::Mat4x4:
						{
							m_currentState->types.Append(Opcode{ SpvOpTypeMatrix }, resultId, GetTypeId(ShaderNodes::BasicType::Float4), 4);
							break;
						}

						case ShaderNodes::BasicType::Sampler2D:
						{
							UInt32 imageTypeId = resultId - 1;

							m_currentState->types.Append(Opcode{ SpvOpTypeImage }, imageTypeId, GetTypeId(ShaderNodes::BasicType::Float1), SpvDim2D, 0, 0, 0, 1, SpvImageFormatUnknown);
							m_currentState->types.Append(Opcode{ SpvOpTypeSampledImage }, resultId, imageTypeId);
							break;
						}

						case ShaderNodes::BasicType::Void:
							m_currentState->types.Append(Opcode{ SpvOpTypeVoid }, resultId);
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

					std::size_t structIndex = std::distance(structs.begin(), it);
					AppendStructType(structIndex, resultId);
				}
				else
					static_assert(AlwaysFalse<T>::value, "non-exhaustive visitor");
			}, type);
		}
	}

	UInt32 SpirvWriter::GetConstantId(const ShaderNodes::Constant::Variant& value) const
	{
		auto typeIt = m_currentState->constantIds.find(value);
		assert(typeIt != m_currentState->constantIds.end());

		return typeIt->second;
	}

	UInt32 SpirvWriter::GetTypeId(const ShaderExpressionType& type) const
	{
		auto typeIt = m_currentState->typeIds.find(type);
		assert(typeIt != m_currentState->typeIds.end());

		return typeIt->second;
	}

	void SpirvWriter::PushResultId(UInt32 value)
	{
		m_currentState->resultIds.push_back(value);
	}

	UInt32 SpirvWriter::PopResultId()
	{
		if (m_currentState->resultIds.empty())
			throw std::runtime_error("invalid operation");

		UInt32 resultId = m_currentState->resultIds.back();
		m_currentState->resultIds.pop_back();

		return resultId;
	}

	UInt32 SpirvWriter::RegisterType(ShaderExpressionType type)
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
					switch (arg)
					{
						case ShaderNodes::BasicType::Boolean:
						case ShaderNodes::BasicType::Float1:
						case ShaderNodes::BasicType::Void:
							break; //< Nothing to do

						// In SPIR-V, vec3 (for example) depends on float
						case ShaderNodes::BasicType::Float2:
						case ShaderNodes::BasicType::Float3:
						case ShaderNodes::BasicType::Float4:
							RegisterType(ShaderNodes::BasicType::Float1);
							break;

						case ShaderNodes::BasicType::Mat4x4:
							RegisterType(ShaderNodes::BasicType::Float4);
							break;

						case ShaderNodes::BasicType::Sampler2D:
							RegisterType(ShaderNodes::BasicType::Float1);
							AllocateResultId(); //< Reserve a result id for the image type
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
						RegisterType(member.type);
				}
				else
					static_assert(AlwaysFalse<T>::value, "non-exhaustive visitor");
			}, type);

			it = m_currentState->typeIds.emplace(std::move(type), AllocateResultId()).first;
		}

		return it->second;
	}

	void SpirvWriter::Visit(const ShaderNodes::AccessMember& node)
	{
		Visit(node.structExpr);
	}

	void SpirvWriter::Visit(const ShaderNodes::AssignOp& node)
	{
		Visit(node.left);
		Visit(node.right);
	}

	void SpirvWriter::Visit(const ShaderNodes::Branch& node)
	{
		throw std::runtime_error("not yet implemented");
	}

	void SpirvWriter::Visit(const ShaderNodes::BinaryOp& node)
	{
		Visit(node.left);
		Visit(node.right);

		UInt32 resultId = AllocateResultId();
		UInt32 leftOperand = PopResultId();
		UInt32 rightOperand = PopResultId();

		SpvOp op = [&] {
			switch (node.op)
			{
				case ShaderNodes::BinaryType::Add: return SpvOpFAdd;
				case ShaderNodes::BinaryType::Substract: return SpvOpFSub;
				case ShaderNodes::BinaryType::Multiply: return SpvOpFMul;
				case ShaderNodes::BinaryType::Divide: return SpvOpFDiv;
				case ShaderNodes::BinaryType::Equality: return SpvOpFOrdEqual;
			}

			assert(false);
			throw std::runtime_error("unexpected binary operation");
		}();

		m_currentState->instructions.Append(Opcode{ op }, GetTypeId(ShaderNodes::BasicType::Float3), resultId, leftOperand, rightOperand);
	}

	void SpirvWriter::Visit(const ShaderNodes::Cast& node)
	{
		for (auto& expr : node.expressions)
		{
			if (!expr)
				break;

			Visit(expr);
		}
	}

	void SpirvWriter::Visit(const ShaderNodes::Constant& node)
	{
		std::visit([&] (const auto& value)
		{
			PushResultId(GetConstantId(value));
		}, node.value);
	}

	void SpirvWriter::Visit(const ShaderNodes::DeclareVariable& node)
	{
		if (node.expression)
			Visit(node.expression);
	}

	void SpirvWriter::Visit(const ShaderNodes::ExpressionStatement& node)
	{
		Visit(node.expression);
	}

	void SpirvWriter::Visit(const ShaderNodes::Identifier& node)
	{
		PushResultId(42);
	}

	void SpirvWriter::Visit(const ShaderNodes::IntrinsicCall& node)
	{
		for (auto& param : node.parameters)
			Visit(param);
	}

	void SpirvWriter::Visit(const ShaderNodes::Sample2D& node)
	{
		Visit(node.sampler);
		Visit(node.coordinates);
	}

	void SpirvWriter::Visit(const ShaderNodes::StatementBlock& node)
	{
		for (auto& statement : node.statements)
			Visit(statement);
	}

	void SpirvWriter::Visit(const ShaderNodes::SwizzleOp& node)
	{
		Visit(node.expression);
	}

	void SpirvWriter::MergeBlocks(std::vector<UInt32>& output, const Section& from)
	{
		std::size_t prevSize = output.size();
		output.resize(prevSize + from.data.size());
		std::copy(from.data.begin(), from.data.end(), output.begin() + prevSize);
	}

	std::size_t SpirvWriter::Section::Append(const Opcode& opcode, const WordCount& wordCount)
	{
		return Append(UInt32(opcode.op) | UInt32(wordCount.wc) << 16);
	}

	std::size_t SpirvWriter::Section::Append(const Raw& raw)
	{
		std::size_t offset = GetOutputOffset();

		const UInt8* ptr = static_cast<const UInt8*>(raw.ptr);

		std::size_t size4 = CountWord(raw);
		for (std::size_t i = 0; i < size4; ++i)
		{
			UInt32 codepoint = 0;
			for (std::size_t j = 0; j < 4; ++j)
			{
				std::size_t pos = i * 4 + j;
				if (pos < raw.size)
					codepoint |= UInt32(ptr[pos]) << (j * 8);
			}

#ifdef NAZARA_BIG_ENDIAN
			SwapBytes(codepoint);
#endif

			Append(codepoint);
		}

		return offset;
	}

	unsigned int SpirvWriter::Section::CountWord(const Raw& raw)
	{
		return (raw.size + sizeof(UInt32) - 1) / sizeof(UInt32);
	}
}
