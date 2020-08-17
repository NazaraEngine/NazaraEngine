// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/SpirvWriter.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Endianness.hpp>
#include <Nazara/Core/StackVector.hpp>
#include <Nazara/Shader/ShaderAstCloner.hpp>
#include <Nazara/Shader/ShaderAstValidator.hpp>
#include <Nazara/Shader/SpirvData.hpp>
#include <Nazara/Shader/SpirvSection.hpp>
#include <tsl/ordered_map.h>
#include <tsl/ordered_set.h>
#include <SpirV/spirv.h>
#include <SpirV/GLSL.std.450.h>
#include <cassert>
#include <stdexcept>
#include <type_traits>
#include <vector>
#include <Nazara/Shader/Debug.hpp>

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

				void Visit(ShaderNodes::AccessMember& node) override
				{
					constants.emplace(Int32(node.memberIndex));

					ShaderAstRecursiveVisitor::Visit(node);
				}

				void Visit(ShaderNodes::Constant& node) override
				{
					std::visit([&](auto&& arg)
					{
						using T = std::decay_t<decltype(arg)>;

						if constexpr (std::is_same_v<T, bool> || std::is_same_v<T, float> || std::is_same_v<T, Int32>)
							constants.emplace(arg);
						else if constexpr (std::is_same_v<T, Vector2f> || std::is_same_v<T, Vector2i32>)
						{
							constants.emplace(arg.x);
							constants.emplace(arg.y);
							constants.emplace(arg);
						}
						else if constexpr (std::is_same_v<T, Vector3f> || std::is_same_v<T, Vector3i32>)
						{
							constants.emplace(arg.x);
							constants.emplace(arg.y);
							constants.emplace(arg.z);
							constants.emplace(arg);
						}
						else if constexpr (std::is_same_v<T, Vector4f> || std::is_same_v<T, Vector4i32>)
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

				void Visit(ShaderNodes::DeclareVariable& node) override
				{
					Visit(node.variable);

					ShaderAstRecursiveVisitor::Visit(node);
				}

				void Visit(ShaderNodes::Identifier& node) override
				{
					Visit(node.var);

					ShaderAstRecursiveVisitor::Visit(node);
				}

				void Visit(ShaderNodes::IntrinsicCall& node) override
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

				void Visit(ShaderNodes::BuiltinVariable& var) override
				{
					builtinVars.insert(std::static_pointer_cast<const ShaderNodes::BuiltinVariable>(var.shared_from_this()));
				}

				void Visit(ShaderNodes::InputVariable& var) override
				{
					/* Handled by ShaderAst */
				}

				void Visit(ShaderNodes::LocalVariable& var) override
				{
					localVars.insert(std::static_pointer_cast<const ShaderNodes::LocalVariable>(var.shared_from_this()));
				}

				void Visit(ShaderNodes::OutputVariable& var) override
				{
					/* Handled by ShaderAst */
				}

				void Visit(ShaderNodes::ParameterVariable& var) override
				{
					paramVars.insert(std::static_pointer_cast<const ShaderNodes::ParameterVariable>(var.shared_from_this()));
				}

				void Visit(ShaderNodes::UniformVariable& var) override
				{
					/* Handled by ShaderAst */
				}

				BuiltinContainer builtinVars;
				ConstantContainer constants;
				ExtInstList extInsts;
				LocalContainer localVars;
				ParameterContainer paramVars;
		};

		class AssignVisitor : public ShaderAstRecursiveVisitor
		{
			public:
				void Visit(ShaderNodes::AccessMember& node) override
				{
				}

				void Visit(ShaderNodes::Identifier& node) override
				{
				}

				void Visit(ShaderNodes::SwizzleOp& node) override
				{
				}
		};

		template<typename T>
		constexpr ShaderNodes::BasicType GetBasicType()
		{
			if constexpr (std::is_same_v<T, bool>)
				return ShaderNodes::BasicType::Boolean;
			else if constexpr (std::is_same_v<T, float>)
				return(ShaderNodes::BasicType::Float1);
			else if constexpr (std::is_same_v<T, Int32>)
				return(ShaderNodes::BasicType::Int1);
			else if constexpr (std::is_same_v<T, Vector2f>)
				return(ShaderNodes::BasicType::Float2);
			else if constexpr (std::is_same_v<T, Vector3f>)
				return(ShaderNodes::BasicType::Float3);
			else if constexpr (std::is_same_v<T, Vector4f>)
				return(ShaderNodes::BasicType::Float4);
			else if constexpr (std::is_same_v<T, Vector2i32>)
				return(ShaderNodes::BasicType::Int2);
			else if constexpr (std::is_same_v<T, Vector3i32>)
				return(ShaderNodes::BasicType::Int3);
			else if constexpr (std::is_same_v<T, Vector4i32>)
				return(ShaderNodes::BasicType::Int4);
			else
				static_assert(AlwaysFalse<T>::value, "unhandled type");
		}
	}

	struct SpirvWriter::ExtVar
	{
		UInt32 pointerTypeId;
		UInt32 typeId;
		UInt32 varId;
		std::optional<UInt32> valueId;
	};

	struct SpirvWriter::State
	{
		struct Func
		{
			UInt32 typeId;
			UInt32 id;
			std::vector<UInt32> paramsId;
		};

		std::unordered_map<std::string, UInt32> extensionInstructions;
		std::unordered_map<ShaderNodes::BuiltinEntry, ExtVar> builtinIds;
		std::unordered_map<std::string, UInt32> varToResult;
		tsl::ordered_map<ConstantVariant, UInt32> constantIds;
		tsl::ordered_map<ShaderExpressionType, UInt32> typeIds;
		std::vector<Func> funcs;
		tsl::ordered_map<std::string, ExtVar> inputIds;
		tsl::ordered_map<std::string, ExtVar> outputIds;
		tsl::ordered_map<std::string, ExtVar> uniformIds;
		std::vector<std::optional<FieldOffsets>> structFields;
		std::vector<UInt32> resultIds;
		UInt32 nextVarIndex = 1;

		// Output
		SpirvSection header;
		SpirvSection constants;
		SpirvSection debugInfo;
		SpirvSection annotations;
		SpirvSection types;
		SpirvSection instructions;
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

		std::vector<ShaderNodes::StatementPtr> functionStatements;

		ShaderAstCloner cloner;

		PreVisitor preVisitor;
		for (const auto& func : shader.GetFunctions())
		{
			functionStatements.emplace_back(cloner.Clone(func.statement));
			preVisitor.Visit(func.statement);
		}

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

		for (const auto& builtin : preVisitor.builtinVars)
			RegisterType(builtin->type);

		// Register constant types
		for (const auto& constant : preVisitor.constants)
		{
			std::visit([&](auto&& arg)
			{
				using T = std::decay_t<decltype(arg)>;
				RegisterType(GetBasicType<T>());
			}, constant);
		}

		AppendTypes();

		// Register result id and debug infos for global variables/functions
		for (const auto& builtin : preVisitor.builtinVars)
		{
			const ShaderExpressionType& builtinExprType = builtin->type;
			assert(std::holds_alternative<ShaderNodes::BasicType>(builtinExprType));

			ShaderNodes::BasicType builtinType = std::get<ShaderNodes::BasicType>(builtinExprType);

			ExtVar builtinData;
			builtinData.pointerTypeId = AllocateResultId();
			builtinData.typeId = GetTypeId(builtinType);
			builtinData.varId = AllocateResultId();

			SpvBuiltIn spvBuiltin;
			std::string debugName;
			switch (builtin->entry)
			{
				case ShaderNodes::BuiltinEntry::VertexPosition:
					debugName = "builtin_VertexPosition";
					spvBuiltin = SpvBuiltInPosition;
					break;

				default:
					throw std::runtime_error("unexpected builtin type");
			}

			state.debugInfo.Append(SpirvOp::OpName, builtinData.varId, debugName);
			state.types.Append(SpirvOp::OpTypePointer, builtinData.pointerTypeId, SpvStorageClassOutput, builtinData.typeId);
			state.types.Append(SpirvOp::OpVariable, builtinData.pointerTypeId, builtinData.varId, SpvStorageClassOutput);

			state.annotations.Append(SpirvOp::OpDecorate, builtinData.varId, SpvDecorationBuiltIn, spvBuiltin);

			state.builtinIds.emplace(builtin->entry, builtinData);
		}

		for (const auto& input : shader.GetInputs())
		{
			ExtVar inputData;
			inputData.pointerTypeId = AllocateResultId();
			inputData.typeId = GetTypeId(input.type);
			inputData.varId = AllocateResultId();

			state.inputIds.emplace(input.name, inputData);

			state.debugInfo.Append(SpirvOp::OpName, inputData.varId, input.name);
			state.types.Append(SpirvOp::OpTypePointer, inputData.pointerTypeId, SpvStorageClassInput, inputData.typeId);
			state.types.Append(SpirvOp::OpVariable, inputData.pointerTypeId, inputData.varId, SpvStorageClassInput);

			if (input.locationIndex)
				state.annotations.Append(SpirvOp::OpDecorate, inputData.varId, SpvDecorationLocation, *input.locationIndex);
		}

		for (const auto& output : shader.GetOutputs())
		{
			ExtVar outputData;
			outputData.pointerTypeId = AllocateResultId();
			outputData.typeId = GetTypeId(output.type);
			outputData.varId = AllocateResultId();

			state.outputIds.emplace(output.name, outputData);

			state.debugInfo.Append(SpirvOp::OpName, outputData.varId, output.name);
			state.types.Append(SpirvOp::OpTypePointer, outputData.pointerTypeId, SpvStorageClassOutput, outputData.typeId);
			state.types.Append(SpirvOp::OpVariable, outputData.pointerTypeId, outputData.varId, SpvStorageClassOutput);

			if (output.locationIndex)
				state.annotations.Append(SpirvOp::OpDecorate, outputData.varId, SpvDecorationLocation, *output.locationIndex);
		}

		for (const auto& uniform : shader.GetUniforms())
		{
			ExtVar uniformData;
			uniformData.pointerTypeId = AllocateResultId();
			uniformData.typeId = GetTypeId(uniform.type);
			uniformData.varId = AllocateResultId();

			state.uniformIds.emplace(uniform.name, uniformData);

			state.debugInfo.Append(SpirvOp::OpName, uniformData.varId, uniform.name);
			state.types.Append(SpirvOp::OpTypePointer, uniformData.pointerTypeId, SpvStorageClassUniform, uniformData.typeId);
			state.types.Append(SpirvOp::OpVariable, uniformData.pointerTypeId, uniformData.varId, SpvStorageClassUniform);

			if (uniform.bindingIndex)
			{
				state.annotations.Append(SpirvOp::OpDecorate, uniformData.varId, SpvDecorationBinding, *uniform.bindingIndex);
				state.annotations.Append(SpirvOp::OpDecorate, uniformData.varId, SpvDecorationDescriptorSet, 0);
			}
		}

		for (const auto& func : shader.GetFunctions())
		{
			auto& funcData = state.funcs.emplace_back();
			funcData.id = AllocateResultId();
			funcData.typeId = AllocateResultId();

			state.debugInfo.Append(SpirvOp::OpName, funcData.id, func.name);

			state.types.AppendVariadic(SpirvOp::OpTypeFunction, [&](const auto& appender)
			{
				appender(funcData.typeId);
				appender(GetTypeId(func.returnType));

				for (const auto& param : func.parameters)
					appender(GetTypeId(param.type));
			});
		}

		// Register constants
		for (const auto& constant : preVisitor.constants)
			state.constantIds[constant] = AllocateResultId();

		AppendConstants();

		std::size_t entryPointIndex = std::numeric_limits<std::size_t>::max();

		for (std::size_t funcIndex = 0; funcIndex < shader.GetFunctionCount(); ++funcIndex)
		{
			const auto& func = shader.GetFunction(funcIndex);
			if (func.name == "main")
				entryPointIndex = funcIndex;

			auto& funcData = state.funcs[funcIndex];

			state.instructions.Append(SpirvOp::OpFunction, GetTypeId(func.returnType), funcData.id, 0, funcData.typeId);

			state.instructions.Append(SpirvOp::OpLabel, AllocateResultId());

			for (const auto& param : func.parameters)
			{
				UInt32 paramResultId = AllocateResultId();
				funcData.paramsId.push_back(paramResultId);

				state.instructions.Append(SpirvOp::OpFunctionParameter, GetTypeId(param.type), paramResultId);
			}

			Visit(functionStatements[funcIndex]);

			if (func.returnType == ShaderNodes::BasicType::Void)
				state.instructions.Append(SpirvOp::OpReturn);

			state.instructions.Append(SpirvOp::OpFunctionEnd);
		}

		assert(entryPointIndex != std::numeric_limits<std::size_t>::max());

		AppendHeader();

		SpvExecutionModel execModel;
		const auto& entryFuncData = shader.GetFunction(entryPointIndex);
		const auto& entryFunc = m_currentState->funcs[entryPointIndex];

		assert(m_context.shader);
		switch (m_context.shader->GetStage())
		{
			case ShaderStageType::Fragment:
				execModel = SpvExecutionModelFragment;
				break;

			case ShaderStageType::Vertex:
				execModel = SpvExecutionModelVertex;
				break;

			default:
				throw std::runtime_error("not yet implemented");
		}

		// OpEntryPoint Vertex %main "main" %outNormal %inNormals %outTexCoords %inTexCoord %_ %inPos

		std::size_t nameSize = state.header.CountWord(entryFuncData.name);

		state.header.AppendVariadic(SpirvOp::OpEntryPoint, [&](const auto& appender)
		{
			appender(execModel);
			appender(entryFunc.id);
			appender(entryFuncData.name);

			for (const auto& [name, varData] : m_currentState->builtinIds)
				appender(varData.varId);

			for (const auto& [name, varData] : m_currentState->inputIds)
				appender(varData.varId);

			for (const auto& [name, varData] : m_currentState->outputIds)
				appender(varData.varId);
		});

		if (m_context.shader->GetStage() == ShaderStageType::Fragment)
			state.header.Append(SpirvOp::OpExecutionMode, entryFunc.id, SpvExecutionModeOriginUpperLeft);

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
					m_currentState->constants.Append((arg) ? SpirvOp::OpConstantTrue : SpirvOp::OpConstantFalse, constantId);
				else if constexpr (std::is_same_v<T, float> || std::is_same_v<T, int>)
					m_currentState->constants.Append(SpirvOp::OpConstant, GetTypeId(GetBasicType<T>()), constantId, SpirvSection::Raw{ &arg, sizeof(arg) });
				else if constexpr (std::is_same_v<T, Vector2f> || std::is_same_v<T, Vector2i>)
					m_currentState->constants.Append(SpirvOp::OpConstantComposite, GetTypeId(GetBasicType<T>()), constantId, GetConstantId(arg.x), GetConstantId(arg.y));
				else if constexpr (std::is_same_v<T, Vector3f> || std::is_same_v<T, Vector3i>)
					m_currentState->constants.Append(SpirvOp::OpConstantComposite, GetTypeId(GetBasicType<T>()), constantId, GetConstantId(arg.x), GetConstantId(arg.y), GetConstantId(arg.z));
				else if constexpr (std::is_same_v<T, Vector4f> || std::is_same_v<T, Vector4i>)
					m_currentState->constants.Append(SpirvOp::OpConstantComposite, GetTypeId(GetBasicType<T>()), constantId, GetConstantId(arg.x), GetConstantId(arg.y), GetConstantId(arg.z), GetConstantId(arg.w));
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

		m_currentState->header.Append(SpirvOp::OpCapability, SpvCapabilityShader);

		for (const auto& [extInst, resultId] : m_currentState->extensionInstructions)
			m_currentState->header.Append(SpirvOp::OpExtInstImport, resultId, extInst);

		m_currentState->header.Append(SpirvOp::OpMemoryModel, SpvAddressingModelLogical, SpvMemoryModelGLSL450);
	}

	void SpirvWriter::AppendStructType(std::size_t structIndex, UInt32 resultId)
	{
		const ShaderAst::Struct& s = m_context.shader->GetStruct(structIndex);

		m_currentState->types.Append(SpirvOp::OpTypeStruct, SpirvSection::OpSize{ static_cast<unsigned int>(1 + 1 + s.members.size()) });
		m_currentState->types.Append(resultId);

		m_currentState->debugInfo.Append(SpirvOp::OpName, resultId, s.name);

		m_currentState->annotations.Append(SpirvOp::OpDecorate, resultId, SpvDecorationBlock);

		FieldOffsets structOffsets(StructLayout_Std140);

		for (std::size_t memberIndex = 0; memberIndex < s.members.size(); ++memberIndex)
		{
			const auto& member = s.members[memberIndex];
			m_currentState->types.Append(GetTypeId(member.type));
			m_currentState->debugInfo.Append(SpirvOp::OpMemberName, resultId, memberIndex, member.name);

			std::visit([&](auto&& arg)
			{
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, ShaderNodes::BasicType>)
				{
					std::size_t offset = [&] {
						switch (arg)
						{
							case ShaderNodes::BasicType::Boolean:   return structOffsets.AddField(StructFieldType_Bool1);
							case ShaderNodes::BasicType::Float1:    return structOffsets.AddField(StructFieldType_Float1);
							case ShaderNodes::BasicType::Float2:    return structOffsets.AddField(StructFieldType_Float2);
							case ShaderNodes::BasicType::Float3:    return structOffsets.AddField(StructFieldType_Float3);
							case ShaderNodes::BasicType::Float4:    return structOffsets.AddField(StructFieldType_Float4);
							case ShaderNodes::BasicType::Int1:      return structOffsets.AddField(StructFieldType_Int1);
							case ShaderNodes::BasicType::Int2:      return structOffsets.AddField(StructFieldType_Int2);
							case ShaderNodes::BasicType::Int3:      return structOffsets.AddField(StructFieldType_Int3);
							case ShaderNodes::BasicType::Int4:      return structOffsets.AddField(StructFieldType_Int4);
							case ShaderNodes::BasicType::Mat4x4:    return structOffsets.AddMatrix(StructFieldType_Float1, 4, 4, true);
							case ShaderNodes::BasicType::Sampler2D: throw std::runtime_error("unexpected sampler2D as struct member");
							case ShaderNodes::BasicType::Void:      throw std::runtime_error("unexpected void as struct member");
						}

						assert(false);
						throw std::runtime_error("unhandled type");
					}();

					m_currentState->annotations.Append(SpirvOp::OpMemberDecorate, resultId, memberIndex, SpvDecorationOffset, offset);

					if (arg == ShaderNodes::BasicType::Mat4x4)
					{
						m_currentState->annotations.Append(SpirvOp::OpMemberDecorate, resultId, memberIndex, SpvDecorationColMajor);
						m_currentState->annotations.Append(SpirvOp::OpMemberDecorate, resultId, memberIndex, SpvDecorationMatrixStride, 16);
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
							m_currentState->types.Append(SpirvOp::OpTypeBool, resultId);
							break;

						case ShaderNodes::BasicType::Float1:
							m_currentState->types.Append(SpirvOp::OpTypeFloat, resultId, 32);
							break;

						case ShaderNodes::BasicType::Float2:
						case ShaderNodes::BasicType::Float3:
						case ShaderNodes::BasicType::Float4:
						case ShaderNodes::BasicType::Int2:
						case ShaderNodes::BasicType::Int3:
						case ShaderNodes::BasicType::Int4:
						{
							ShaderNodes::BasicType baseType = ShaderNodes::Node::GetComponentType(arg);

							UInt32 vecSize = UInt32(arg) - UInt32(baseType) + 1;

							m_currentState->types.Append(SpirvOp::OpTypeVector, resultId, GetTypeId(baseType), vecSize);
							break;
						}

						case ShaderNodes::BasicType::Int1:
							m_currentState->types.Append(SpirvOp::OpTypeInt, resultId, 32, 1);
							break;

						case ShaderNodes::BasicType::Mat4x4:
						{
							m_currentState->types.Append(SpirvOp::OpTypeMatrix, resultId, GetTypeId(ShaderNodes::BasicType::Float4), 4);
							break;
						}

						case ShaderNodes::BasicType::Sampler2D:
						{
							UInt32 imageTypeId = resultId - 1;

							m_currentState->types.Append(SpirvOp::OpTypeImage, imageTypeId, GetTypeId(ShaderNodes::BasicType::Float1), SpvDim2D, 0, 0, 0, 1, SpvImageFormatUnknown);
							m_currentState->types.Append(SpirvOp::OpTypeSampledImage, resultId, imageTypeId);
							break;
						}

						case ShaderNodes::BasicType::Void:
							m_currentState->types.Append(SpirvOp::OpTypeVoid, resultId);
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

	UInt32 SpirvWriter::EvaluateExpression(const ShaderNodes::ExpressionPtr& expr)
	{
		Visit(expr);
		return PopResultId();
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

	UInt32 SpirvWriter::ReadVariable(ExtVar& var)
	{
		if (!var.valueId.has_value())
		{
			UInt32 resultId = AllocateResultId();
			m_currentState->instructions.Append(SpirvOp::OpLoad, var.typeId, resultId, var.varId);

			var.valueId = resultId;
		}

		return var.valueId.value();
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
						case ShaderNodes::BasicType::Int1:
						case ShaderNodes::BasicType::Void:
							break; //< Nothing to do

						// In SPIR-V, vec3 (for example) depends on float
						case ShaderNodes::BasicType::Float2:
						case ShaderNodes::BasicType::Float3:
						case ShaderNodes::BasicType::Float4:
						case ShaderNodes::BasicType::Int2:
						case ShaderNodes::BasicType::Int3:
						case ShaderNodes::BasicType::Int4:
						case ShaderNodes::BasicType::Mat4x4:
							RegisterType(ShaderNodes::Node::GetComponentType(arg));
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

	void SpirvWriter::Visit(ShaderNodes::AccessMember& node)
	{
		UInt32 pointerId;
		SpvStorageClass storage;

		switch (node.structExpr->GetType())
		{
			case ShaderNodes::NodeType::Identifier:
			{
				auto& identifier = static_cast<ShaderNodes::Identifier&>(*node.structExpr);
				switch (identifier.var->GetType())
				{
					case ShaderNodes::VariableType::BuiltinVariable:
					{
						auto& builtinvar = static_cast<ShaderNodes::BuiltinVariable&>(*identifier.var);
						auto it = m_currentState->builtinIds.find(builtinvar.entry);
						assert(it != m_currentState->builtinIds.end());

						pointerId = it->second.varId;
						break;
					}

					case ShaderNodes::VariableType::InputVariable:
					{
						auto& inputVar = static_cast<ShaderNodes::InputVariable&>(*identifier.var);
						auto it = m_currentState->inputIds.find(inputVar.name);
						assert(it != m_currentState->inputIds.end());

						storage = SpvStorageClassInput;

						pointerId = it->second.varId;
						break;
					}

					case ShaderNodes::VariableType::OutputVariable:
					{
						auto& outputVar = static_cast<ShaderNodes::OutputVariable&>(*identifier.var);
						auto it = m_currentState->outputIds.find(outputVar.name);
						assert(it != m_currentState->outputIds.end());

						storage = SpvStorageClassOutput;

						pointerId = it->second.varId;
						break;
					}

					case ShaderNodes::VariableType::UniformVariable:
					{
						auto& uniformVar = static_cast<ShaderNodes::UniformVariable&>(*identifier.var);
						auto it = m_currentState->uniformIds.find(uniformVar.name);
						assert(it != m_currentState->uniformIds.end());

						storage = SpvStorageClassUniform;

						pointerId = it->second.varId;
						break;
					}

					case ShaderNodes::VariableType::LocalVariable:
					case ShaderNodes::VariableType::ParameterVariable:
					default:
						throw std::runtime_error("not yet implemented");
				}
				break;
			}

			case ShaderNodes::NodeType::SwizzleOp: //< TODO
			default:
				throw std::runtime_error("not yet implemented");
		}

		UInt32 memberPointerId = AllocateResultId();
		UInt32 pointerType = AllocateResultId();
		UInt32 typeId = GetTypeId(node.exprType);
		UInt32 indexId = GetConstantId(Int32(node.memberIndex));

		m_currentState->types.Append(SpirvOp::OpTypePointer, pointerType, storage, typeId);

		m_currentState->instructions.Append(SpirvOp::OpAccessChain, pointerType, memberPointerId, pointerId, indexId);

		UInt32 resultId = AllocateResultId();

		m_currentState->instructions.Append(SpirvOp::OpLoad, typeId, resultId, memberPointerId);

		PushResultId(resultId);
	}

	void SpirvWriter::Visit(ShaderNodes::AssignOp& node)
	{
		UInt32 result = EvaluateExpression(node.right);

		switch (node.left->GetType())
		{
			case ShaderNodes::NodeType::Identifier:
			{
				auto& identifier = static_cast<ShaderNodes::Identifier&>(*node.left);
				switch (identifier.var->GetType())
				{
					case ShaderNodes::VariableType::BuiltinVariable:
					{
						auto& builtinvar = static_cast<ShaderNodes::BuiltinVariable&>(*identifier.var);
						auto it = m_currentState->builtinIds.find(builtinvar.entry);
						assert(it != m_currentState->builtinIds.end());

						m_currentState->instructions.Append(SpirvOp::OpStore, it->second.varId, result);
						PushResultId(result);
						break;
					}

					case ShaderNodes::VariableType::OutputVariable:
					{
						auto& outputVar = static_cast<ShaderNodes::OutputVariable&>(*identifier.var);
						auto it = m_currentState->outputIds.find(outputVar.name);
						assert(it != m_currentState->outputIds.end());

						m_currentState->instructions.Append(SpirvOp::OpStore, it->second.varId, result);
						PushResultId(result);
						break;
					}

					case ShaderNodes::VariableType::InputVariable:
					case ShaderNodes::VariableType::LocalVariable:
					case ShaderNodes::VariableType::ParameterVariable:
					case ShaderNodes::VariableType::UniformVariable:
					default:
						throw std::runtime_error("not yet implemented");
				}
				break;
			}

			case ShaderNodes::NodeType::SwizzleOp: //< TODO
			default:
				throw std::runtime_error("not yet implemented");
		}
	}

	void SpirvWriter::Visit(ShaderNodes::Branch& node)
	{
		throw std::runtime_error("not yet implemented");
	}

	void SpirvWriter::Visit(ShaderNodes::BinaryOp& node)
	{
		ShaderExpressionType resultExprType = node.GetExpressionType();
		assert(std::holds_alternative<ShaderNodes::BasicType>(resultExprType));

		const ShaderExpressionType& leftExprType = node.left->GetExpressionType();
		assert(std::holds_alternative<ShaderNodes::BasicType>(leftExprType));

		const ShaderExpressionType& rightExprType = node.right->GetExpressionType();
		assert(std::holds_alternative<ShaderNodes::BasicType>(rightExprType));

		ShaderNodes::BasicType resultType = std::get<ShaderNodes::BasicType>(resultExprType);
		ShaderNodes::BasicType leftType = std::get<ShaderNodes::BasicType>(leftExprType);
		ShaderNodes::BasicType rightType = std::get<ShaderNodes::BasicType>(rightExprType);


		UInt32 leftOperand = EvaluateExpression(node.left);
		UInt32 rightOperand = EvaluateExpression(node.right);
		UInt32 resultId = AllocateResultId();

		bool swapOperands = false;

		SpirvOp op = [&]
		{
			switch (node.op)
			{
				case ShaderNodes::BinaryType::Add:
				{
					switch (leftType)
					{
						case ShaderNodes::BasicType::Float1:
						case ShaderNodes::BasicType::Float2:
						case ShaderNodes::BasicType::Float3:
						case ShaderNodes::BasicType::Float4:
						case ShaderNodes::BasicType::Mat4x4:
							return SpirvOp::OpFAdd;

						case ShaderNodes::BasicType::Int1:
						case ShaderNodes::BasicType::Int2:
						case ShaderNodes::BasicType::Int3:
						case ShaderNodes::BasicType::Int4:
							return SpirvOp::OpIAdd;

						case ShaderNodes::BasicType::Boolean:
						case ShaderNodes::BasicType::Sampler2D:
						case ShaderNodes::BasicType::Void:
							break;
					}
				}

				case ShaderNodes::BinaryType::Substract:
				{
					switch (leftType)
					{
						case ShaderNodes::BasicType::Float1:
						case ShaderNodes::BasicType::Float2:
						case ShaderNodes::BasicType::Float3:
						case ShaderNodes::BasicType::Float4:
						case ShaderNodes::BasicType::Mat4x4:
							return SpirvOp::OpFSub;

						case ShaderNodes::BasicType::Int1:
						case ShaderNodes::BasicType::Int2:
						case ShaderNodes::BasicType::Int3:
						case ShaderNodes::BasicType::Int4:
							return SpirvOp::OpISub;

						case ShaderNodes::BasicType::Boolean:
						case ShaderNodes::BasicType::Sampler2D:
						case ShaderNodes::BasicType::Void:
							break;
					}
				}

				case ShaderNodes::BinaryType::Divide:
				{
					switch (leftType)
					{
						case ShaderNodes::BasicType::Float1:
						case ShaderNodes::BasicType::Float2:
						case ShaderNodes::BasicType::Float3:
						case ShaderNodes::BasicType::Float4:
						case ShaderNodes::BasicType::Mat4x4:
							return SpirvOp::OpFDiv;

						case ShaderNodes::BasicType::Int1:
						case ShaderNodes::BasicType::Int2:
						case ShaderNodes::BasicType::Int3:
						case ShaderNodes::BasicType::Int4:
							return SpirvOp::OpSDiv;

						case ShaderNodes::BasicType::Boolean:
						case ShaderNodes::BasicType::Sampler2D:
						case ShaderNodes::BasicType::Void:
							break;
					}
				}

				case ShaderNodes::BinaryType::Equality:
				{
					switch (leftType)
					{
						case ShaderNodes::BasicType::Boolean:
							return SpirvOp::OpLogicalEqual;

						case ShaderNodes::BasicType::Float1:
						case ShaderNodes::BasicType::Float2:
						case ShaderNodes::BasicType::Float3:
						case ShaderNodes::BasicType::Float4:
						case ShaderNodes::BasicType::Mat4x4:
							return SpirvOp::OpFOrdEqual;

						case ShaderNodes::BasicType::Int1:
						case ShaderNodes::BasicType::Int2:
						case ShaderNodes::BasicType::Int3:
						case ShaderNodes::BasicType::Int4:
							return SpirvOp::OpIEqual;

						case ShaderNodes::BasicType::Sampler2D:
						case ShaderNodes::BasicType::Void:
							break;
					}
				}

				case ShaderNodes::BinaryType::Multiply:
				{
					switch (leftType)
					{
						case ShaderNodes::BasicType::Float1:
						{
							switch (rightType)
							{
								case ShaderNodes::BasicType::Float1:
									return SpirvOp::OpFMul;

								case ShaderNodes::BasicType::Float2:
								case ShaderNodes::BasicType::Float3:
								case ShaderNodes::BasicType::Float4:
									swapOperands = true;
									return SpirvOp::OpVectorTimesScalar;

								case ShaderNodes::BasicType::Mat4x4:
									swapOperands = true;
									return SpirvOp::OpMatrixTimesScalar;

								default:
									break;
							}

							break;
						}

						case ShaderNodes::BasicType::Float2:
						case ShaderNodes::BasicType::Float3:
						case ShaderNodes::BasicType::Float4:
						{
							switch (rightType)
							{
								case ShaderNodes::BasicType::Float1:
									return SpirvOp::OpVectorTimesScalar;

								case ShaderNodes::BasicType::Float2:
								case ShaderNodes::BasicType::Float3:
								case ShaderNodes::BasicType::Float4:
									return SpirvOp::OpFMul;

								case ShaderNodes::BasicType::Mat4x4:
									return SpirvOp::OpVectorTimesMatrix;

								default:
									break;
							}

							break;
						}

						case ShaderNodes::BasicType::Int1:
						case ShaderNodes::BasicType::Int2:
						case ShaderNodes::BasicType::Int3:
						case ShaderNodes::BasicType::Int4:
							return SpirvOp::OpIMul;

						case ShaderNodes::BasicType::Mat4x4:
						{
							switch (rightType)
							{
								case ShaderNodes::BasicType::Float1: return SpirvOp::OpMatrixTimesScalar;
								case ShaderNodes::BasicType::Float4: return SpirvOp::OpMatrixTimesVector;
								case ShaderNodes::BasicType::Mat4x4: return SpirvOp::OpMatrixTimesMatrix;

								default:
									break;
							}

							break;
						}

						default:
							break;
					}
					break;
				}
			}

			assert(false);
			throw std::runtime_error("unexpected binary operation");
		}();

		if (swapOperands)
			std::swap(leftOperand, rightOperand);

		m_currentState->instructions.Append(op, GetTypeId(resultType), resultId, leftOperand, rightOperand);
		PushResultId(resultId);
	}

	void SpirvWriter::Visit(ShaderNodes::Cast& node)
	{
		const ShaderExpressionType& targetExprType = node.exprType;
		assert(std::holds_alternative<ShaderNodes::BasicType>(targetExprType));

		ShaderNodes::BasicType targetType = std::get<ShaderNodes::BasicType>(targetExprType);

		StackVector<UInt32> exprResults = NazaraStackVector(UInt32, node.expressions.size());

		for (const auto& exprPtr : node.expressions)
		{
			if (!exprPtr)
				break;

			exprResults.push_back(EvaluateExpression(exprPtr));
		}

		UInt32 resultId = AllocateResultId();

		m_currentState->instructions.AppendVariadic(SpirvOp::OpCompositeConstruct, [&](const auto& appender)
		{
			appender(GetTypeId(targetType));
			appender(resultId);

			for (UInt32 exprResultId : exprResults)
				appender(exprResultId);
		});

		PushResultId(resultId);
	}

	void SpirvWriter::Visit(ShaderNodes::Constant& node)
	{
		std::visit([&] (const auto& value)
		{
			PushResultId(GetConstantId(value));
		}, node.value);
	}

	void SpirvWriter::Visit(ShaderNodes::DeclareVariable& node)
	{
		if (node.expression)
		{
			assert(node.variable->GetType() == ShaderNodes::VariableType::LocalVariable);

			const auto& localVar = static_cast<const ShaderNodes::LocalVariable&>(*node.variable);
			m_currentState->varToResult[localVar.name] = EvaluateExpression(node.expression);
		}
	}

	void SpirvWriter::Visit(ShaderNodes::ExpressionStatement& node)
	{
		Visit(node.expression);
		PopResultId();
	}

	void SpirvWriter::Visit(ShaderNodes::Identifier& node)
	{
		Visit(node.var);
	}

	void SpirvWriter::Visit(ShaderNodes::IntrinsicCall& node)
	{
		switch (node.intrinsic)
		{
			case ShaderNodes::IntrinsicType::DotProduct:
			{
				const ShaderExpressionType& vecExprType = node.parameters[0]->GetExpressionType();
				assert(std::holds_alternative<ShaderNodes::BasicType>(vecExprType));

				ShaderNodes::BasicType vecType = std::get<ShaderNodes::BasicType>(vecExprType);

				UInt32 typeId = GetTypeId(node.GetComponentType(vecType));

				UInt32 vec1 = EvaluateExpression(node.parameters[0]);
				UInt32 vec2 = EvaluateExpression(node.parameters[1]);

				UInt32 resultId = AllocateResultId();

				m_currentState->instructions.Append(SpirvOp::OpDot, typeId, resultId, vec1, vec2);
				PushResultId(resultId);
				break;
			}

			case ShaderNodes::IntrinsicType::CrossProduct:
			default:
				throw std::runtime_error("not yet implemented");
		}
	}

	void SpirvWriter::Visit(ShaderNodes::Sample2D& node)
	{
		// OpImageSampleImplicitLod %v4float %31 %35

		UInt32 typeId = GetTypeId(ShaderNodes::BasicType::Float4);

		UInt32 samplerId = EvaluateExpression(node.sampler);
		UInt32 coordinatesId = EvaluateExpression(node.coordinates);
		UInt32 resultId = AllocateResultId();

		m_currentState->instructions.Append(SpirvOp::OpImageSampleImplicitLod, typeId, resultId, samplerId, coordinatesId);
		PushResultId(resultId);
	}

	void SpirvWriter::Visit(ShaderNodes::StatementBlock& node)
	{
		for (auto& statement : node.statements)
			Visit(statement);
	}

	void SpirvWriter::Visit(ShaderNodes::SwizzleOp& node)
	{
		const ShaderExpressionType& targetExprType = node.GetExpressionType();
		assert(std::holds_alternative<ShaderNodes::BasicType>(targetExprType));

		ShaderNodes::BasicType targetType = std::get<ShaderNodes::BasicType>(targetExprType);

		UInt32 exprResultId = EvaluateExpression(node.expression);
		UInt32 resultId = AllocateResultId();

		if (node.componentCount > 1)
		{
			// Swizzling is implemented via SpirvOp::OpVectorShuffle using the same vector twice as operands
			m_currentState->instructions.AppendVariadic(SpirvOp::OpVectorShuffle, [&](const auto& appender)
			{
				appender(GetTypeId(targetType));
				appender(resultId);
				appender(exprResultId);
				appender(exprResultId);

				for (std::size_t i = 0; i < node.componentCount; ++i)
					appender(UInt32(node.components[0]) - UInt32(node.components[i]));
			});
		}
		else
		{
			// Extract a single component from the vector
			assert(node.componentCount == 1);

			m_currentState->instructions.Append(SpirvOp::OpCompositeExtract, GetTypeId(targetType), resultId, exprResultId, UInt32(node.components[0]) - UInt32(ShaderNodes::SwizzleComponent::First) );
		}

		PushResultId(resultId);
	}

	void SpirvWriter::Visit(ShaderNodes::BuiltinVariable& var)
	{
		throw std::runtime_error("not implemented yet");
	}

	void SpirvWriter::Visit(ShaderNodes::InputVariable& var)
	{
		auto it = m_currentState->inputIds.find(var.name);
		assert(it != m_currentState->inputIds.end());

		PushResultId(ReadVariable(it.value()));
	}

	void SpirvWriter::Visit(ShaderNodes::LocalVariable& var)
	{
		auto it = m_currentState->varToResult.find(var.name);
		assert(it != m_currentState->varToResult.end());

		PushResultId(it->second);
	}

	void SpirvWriter::Visit(ShaderNodes::OutputVariable& var)
	{
		auto it = m_currentState->outputIds.find(var.name);
		assert(it != m_currentState->outputIds.end());

		PushResultId(ReadVariable(it.value()));
	}

	void SpirvWriter::Visit(ShaderNodes::ParameterVariable& var)
	{
		throw std::runtime_error("not implemented yet");
	}

	void SpirvWriter::Visit(ShaderNodes::UniformVariable& var)
	{
		auto it = m_currentState->uniformIds.find(var.name);
		assert(it != m_currentState->uniformIds.end());

		PushResultId(ReadVariable(it.value()));
	}

	void SpirvWriter::MergeBlocks(std::vector<UInt32>& output, const SpirvSection& from)
	{
		const std::vector<UInt32>& bytecode = from.GetBytecode();

		std::size_t prevSize = output.size();
		output.resize(prevSize + bytecode.size());
		std::copy(bytecode.begin(), bytecode.end(), output.begin() + prevSize);
	}
}
