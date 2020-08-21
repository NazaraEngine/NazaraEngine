// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/SpirvWriter.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/StackVector.hpp>
#include <Nazara/Shader/ShaderAstCloner.hpp>
#include <Nazara/Shader/ShaderAstValidator.hpp>
#include <Nazara/Shader/SpirvConstantCache.hpp>
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
		class PreVisitor : public ShaderAstRecursiveVisitor, public ShaderVarVisitor
		{
			public:
				using BuiltinContainer = std::unordered_set<std::shared_ptr<const ShaderNodes::BuiltinVariable>>;
				using ExtInstList = std::unordered_set<std::string>;
				using LocalContainer = std::unordered_set<std::shared_ptr<const ShaderNodes::LocalVariable>>;
				using ParameterContainer = std::unordered_set< std::shared_ptr<const ShaderNodes::ParameterVariable>>;

				PreVisitor(SpirvConstantCache& constantCache) :
				m_constantCache(constantCache)
				{
				}

				using ShaderAstRecursiveVisitor::Visit;
				using ShaderVarVisitor::Visit;

				void Visit(ShaderNodes::AccessMember& node) override
				{
					for (std::size_t index : node.memberIndices)
						m_constantCache.Register(*SpirvConstantCache::BuildConstant(Int32(index)));

					ShaderAstRecursiveVisitor::Visit(node);
				}

				void Visit(ShaderNodes::Constant& node) override
				{
					std::visit([&](auto&& arg)
					{
						m_constantCache.Register(*SpirvConstantCache::BuildConstant(arg));
					}, node.value);

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

				void Visit(ShaderNodes::InputVariable& /*var*/) override
				{
					/* Handled by ShaderAst */
				}

				void Visit(ShaderNodes::LocalVariable& var) override
				{
					localVars.insert(std::static_pointer_cast<const ShaderNodes::LocalVariable>(var.shared_from_this()));
				}

				void Visit(ShaderNodes::OutputVariable& /*var*/) override
				{
					/* Handled by ShaderAst */
				}

				void Visit(ShaderNodes::ParameterVariable& var) override
				{
					paramVars.insert(std::static_pointer_cast<const ShaderNodes::ParameterVariable>(var.shared_from_this()));
				}

				void Visit(ShaderNodes::UniformVariable& /*var*/) override
				{
					/* Handled by ShaderAst */
				}

				BuiltinContainer builtinVars;
				ExtInstList extInsts;
				LocalContainer localVars;
				ParameterContainer paramVars;

			private:
				SpirvConstantCache& m_constantCache;
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
		State() :
		constantTypeCache(nextVarIndex)
		{
		}

		struct Func
		{
			UInt32 typeId;
			UInt32 id;
			std::vector<UInt32> paramsId;
		};

		tsl::ordered_map<std::string, ExtVar> inputIds;
		tsl::ordered_map<std::string, ExtVar> outputIds;
		tsl::ordered_map<std::string, ExtVar> uniformIds;
		std::unordered_map<std::string, UInt32> extensionInstructions;
		std::unordered_map<ShaderNodes::BuiltinEntry, ExtVar> builtinIds;
		std::unordered_map<std::string, UInt32> varToResult;
		std::vector<Func> funcs;
		std::vector<UInt32> resultIds;
		UInt32 nextVarIndex = 1;
		SpirvConstantCache constantTypeCache; //< init after nextVarIndex

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

		std::vector<ShaderNodes::StatementPtr> functionStatements;

		ShaderAstCloner cloner;

		PreVisitor preVisitor(state.constantTypeCache);
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
			RegisterPointerType(input.type, SpirvStorageClass::Input);

		for (const auto& output : shader.GetOutputs())
			RegisterPointerType(output.type, SpirvStorageClass::Output);

		for (const auto& uniform : shader.GetUniforms())
			RegisterPointerType(uniform.type, SpirvStorageClass::Uniform);

		for (const auto& func : shader.GetFunctions())
			RegisterFunctionType(func.returnType, func.parameters);

		for (const auto& local : preVisitor.localVars)
			RegisterType(local->type);

		for (const auto& builtin : preVisitor.builtinVars)
			RegisterType(builtin->type);

		// Register result id and debug infos for global variables/functions
		for (const auto& builtin : preVisitor.builtinVars)
		{
			SpirvConstantCache::Variable variable;
			SpirvBuiltIn builtinDecoration;
			switch (builtin->entry)
			{
				case ShaderNodes::BuiltinEntry::VertexPosition:
					variable.debugName = "builtin_VertexPosition";
					variable.storageClass = SpirvStorageClass::Output;

					builtinDecoration = SpirvBuiltIn::Position;
					break;

				default:
					throw std::runtime_error("unexpected builtin type");
			}

			const ShaderExpressionType& builtinExprType = builtin->type;
			assert(std::holds_alternative<ShaderNodes::BasicType>(builtinExprType));

			ShaderNodes::BasicType builtinType = std::get<ShaderNodes::BasicType>(builtinExprType);

			variable.type = SpirvConstantCache::BuildPointerType(builtinType, variable.storageClass);

			UInt32 varId = m_currentState->constantTypeCache.Register(variable);

			ExtVar builtinData;
			builtinData.pointerTypeId = GetPointerTypeId(builtinType, variable.storageClass);
			builtinData.typeId = GetTypeId(builtinType);
			builtinData.varId = varId;

			state.annotations.Append(SpirvOp::OpDecorate, builtinData.varId, SpvDecorationBuiltIn, builtinDecoration);

			state.builtinIds.emplace(builtin->entry, builtinData);
		}

		for (const auto& input : shader.GetInputs())
		{
			SpirvConstantCache::Variable variable;
			variable.debugName = input.name;
			variable.storageClass = SpirvStorageClass::Input;
			variable.type = SpirvConstantCache::BuildPointerType(shader, input.type, variable.storageClass);

			UInt32 varId = m_currentState->constantTypeCache.Register(variable);

			ExtVar inputData;
			inputData.pointerTypeId = GetPointerTypeId(input.type, variable.storageClass);
			inputData.typeId = GetTypeId(input.type);
			inputData.varId = varId;

			state.inputIds.emplace(input.name, std::move(inputData));

			if (input.locationIndex)
				state.annotations.Append(SpirvOp::OpDecorate, varId, SpvDecorationLocation, *input.locationIndex);
		}

		for (const auto& output : shader.GetOutputs())
		{
			SpirvConstantCache::Variable variable;
			variable.debugName = output.name;
			variable.storageClass = SpirvStorageClass::Output;
			variable.type = SpirvConstantCache::BuildPointerType(shader, output.type, variable.storageClass);

			UInt32 varId = m_currentState->constantTypeCache.Register(variable);

			ExtVar outputData;
			outputData.pointerTypeId = GetPointerTypeId(output.type, variable.storageClass);
			outputData.typeId = GetTypeId(output.type);
			outputData.varId = varId;

			state.outputIds.emplace(output.name, std::move(outputData));

			if (output.locationIndex)
				state.annotations.Append(SpirvOp::OpDecorate, varId, SpvDecorationLocation, *output.locationIndex);
		}

		for (const auto& uniform : shader.GetUniforms())
		{
			SpirvConstantCache::Variable variable;
			variable.debugName = uniform.name;
			variable.storageClass = SpirvStorageClass::Uniform;
			variable.type = SpirvConstantCache::BuildPointerType(shader, uniform.type, variable.storageClass);

			UInt32 varId = m_currentState->constantTypeCache.Register(variable);

			ExtVar uniformData;
			uniformData.pointerTypeId = GetPointerTypeId(uniform.type, variable.storageClass);
			uniformData.typeId = GetTypeId(uniform.type);
			uniformData.varId = varId;

			state.uniformIds.emplace(uniform.name, std::move(uniformData));

			if (uniform.bindingIndex)
			{
				state.annotations.Append(SpirvOp::OpDecorate, varId, SpvDecorationBinding, *uniform.bindingIndex);
				state.annotations.Append(SpirvOp::OpDecorate, varId, SpvDecorationDescriptorSet, 0);
			}
		}

		for (const auto& func : shader.GetFunctions())
		{
			auto& funcData = state.funcs.emplace_back();
			funcData.id = AllocateResultId();
			funcData.typeId = GetFunctionTypeId(func.returnType, func.parameters);

			state.debugInfo.Append(SpirvOp::OpName, funcData.id, func.name);
		}

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

		m_currentState->constantTypeCache.Write(m_currentState->annotations, m_currentState->constants, m_currentState->debugInfo, m_currentState->types);

		AppendHeader();

		SpvExecutionModel execModel;
		const auto& entryFuncData = shader.GetFunction(entryPointIndex);
		const auto& entryFunc = state.funcs[entryPointIndex];

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

		state.header.AppendVariadic(SpirvOp::OpEntryPoint, [&](const auto& appender)
		{
			appender(execModel);
			appender(entryFunc.id);
			appender(entryFuncData.name);

			for (const auto& [name, varData] : state.builtinIds)
				appender(varData.varId);

			for (const auto& [name, varData] : state.inputIds)
				appender(varData.varId);

			for (const auto& [name, varData] : state.outputIds)
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

	UInt32 SpirvWriter::EvaluateExpression(const ShaderNodes::ExpressionPtr& expr)
	{
		Visit(expr);
		return PopResultId();
	}

	UInt32 SpirvWriter::GetConstantId(const ShaderConstantValue& value) const
	{
		return m_currentState->constantTypeCache.GetId(*SpirvConstantCache::BuildConstant(value));
	}

	UInt32 SpirvWriter::GetFunctionTypeId(ShaderExpressionType retType, const std::vector<ShaderAst::FunctionParameter>& parameters)
	{
		std::vector<SpirvConstantCache::TypePtr> parameterTypes;
		parameterTypes.reserve(parameters.size());

		for (const auto& parameter : parameters)
			parameterTypes.push_back(SpirvConstantCache::BuildType(*m_context.shader, parameter.type));

		return m_currentState->constantTypeCache.GetId({
			SpirvConstantCache::Function {
				SpirvConstantCache::BuildType(*m_context.shader, retType),
				std::move(parameterTypes)
			}
		});
	}

	UInt32 SpirvWriter::GetPointerTypeId(const ShaderExpressionType& type, SpirvStorageClass storageClass) const
	{
		return m_currentState->constantTypeCache.GetId(*SpirvConstantCache::BuildPointerType(*m_context.shader, type, storageClass));
	}

	UInt32 SpirvWriter::GetTypeId(const ShaderExpressionType& type) const
	{
		return m_currentState->constantTypeCache.GetId(*SpirvConstantCache::BuildType(*m_context.shader, type));
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

	UInt32 SpirvWriter::RegisterConstant(const ShaderConstantValue& value)
	{
		return m_currentState->constantTypeCache.Register(*SpirvConstantCache::BuildConstant(value));
	}

	UInt32 SpirvWriter::RegisterFunctionType(ShaderExpressionType retType, const std::vector<ShaderAst::FunctionParameter>& parameters)
	{
		std::vector<SpirvConstantCache::TypePtr> parameterTypes;
		parameterTypes.reserve(parameters.size());

		for (const auto& parameter : parameters)
			parameterTypes.push_back(SpirvConstantCache::BuildType(*m_context.shader, parameter.type));

		return m_currentState->constantTypeCache.Register({
			SpirvConstantCache::Function {
				SpirvConstantCache::BuildType(*m_context.shader, retType),
				std::move(parameterTypes)
			}
		});
	}

	UInt32 SpirvWriter::RegisterPointerType(ShaderExpressionType type, SpirvStorageClass storageClass)
	{
		return m_currentState->constantTypeCache.Register(*SpirvConstantCache::BuildPointerType(*m_context.shader, type, storageClass));
	}

	UInt32 SpirvWriter::RegisterType(ShaderExpressionType type)
	{
		assert(m_currentState);
		return m_currentState->constantTypeCache.Register(*SpirvConstantCache::BuildType(*m_context.shader, type));
	}

	void SpirvWriter::Visit(ShaderNodes::AccessMember& node)
	{
		UInt32 pointerId;
		SpirvStorageClass storage;

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

						storage = SpirvStorageClass::Input;

						pointerId = it->second.varId;
						break;
					}

					case ShaderNodes::VariableType::OutputVariable:
					{
						auto& outputVar = static_cast<ShaderNodes::OutputVariable&>(*identifier.var);
						auto it = m_currentState->outputIds.find(outputVar.name);
						assert(it != m_currentState->outputIds.end());

						storage = SpirvStorageClass::Output;

						pointerId = it->second.varId;
						break;
					}

					case ShaderNodes::VariableType::UniformVariable:
					{
						auto& uniformVar = static_cast<ShaderNodes::UniformVariable&>(*identifier.var);
						auto it = m_currentState->uniformIds.find(uniformVar.name);
						assert(it != m_currentState->uniformIds.end());

						storage = SpirvStorageClass::Uniform;

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
		UInt32 pointerType = RegisterPointerType(node.exprType, storage); //< FIXME
		UInt32 typeId = GetTypeId(node.exprType);

		m_currentState->instructions.AppendVariadic(SpirvOp::OpAccessChain, [&](const auto& appender)
		{
			appender(pointerType);
			appender(memberPointerId);
			appender(pointerId);

			for (std::size_t index : node.memberIndices)
				appender(GetConstantId(Int32(index)));
		});

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
						case ShaderNodes::BasicType::UInt1:
						case ShaderNodes::BasicType::UInt2:
						case ShaderNodes::BasicType::UInt3:
						case ShaderNodes::BasicType::UInt4:
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
						case ShaderNodes::BasicType::UInt1:
						case ShaderNodes::BasicType::UInt2:
						case ShaderNodes::BasicType::UInt3:
						case ShaderNodes::BasicType::UInt4:
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

						case ShaderNodes::BasicType::UInt1:
						case ShaderNodes::BasicType::UInt2:
						case ShaderNodes::BasicType::UInt3:
						case ShaderNodes::BasicType::UInt4:
							return SpirvOp::OpUDiv;

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
						case ShaderNodes::BasicType::UInt1:
						case ShaderNodes::BasicType::UInt2:
						case ShaderNodes::BasicType::UInt3:
						case ShaderNodes::BasicType::UInt4:
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
						case ShaderNodes::BasicType::UInt1:
						case ShaderNodes::BasicType::UInt2:
						case ShaderNodes::BasicType::UInt3:
						case ShaderNodes::BasicType::UInt4:
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
