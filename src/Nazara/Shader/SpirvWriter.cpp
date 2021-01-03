// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/SpirvWriter.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/StackVector.hpp>
#include <Nazara/Shader/ShaderAstCloner.hpp>
#include <Nazara/Shader/ShaderAstValidator.hpp>
#include <Nazara/Shader/SpirvAstVisitor.hpp>
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

				PreVisitor(const ShaderAst& shader, const SpirvWriter::States& conditions, SpirvConstantCache& constantCache) :
				m_shader(shader),
				m_conditions(conditions),
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

				void Visit(ShaderNodes::ConditionalExpression& node) override
				{
					std::size_t conditionIndex = m_shader.FindConditionByName(node.conditionName);
					assert(conditionIndex != ShaderAst::InvalidCondition);

					if (TestBit<Nz::UInt64>(m_conditions.enabledConditions, conditionIndex))
						Visit(node.truePath);
					else
						Visit(node.falsePath);
				}

				void Visit(ShaderNodes::ConditionalStatement& node) override
				{
					std::size_t conditionIndex = m_shader.FindConditionByName(node.conditionName);
					assert(conditionIndex != ShaderAst::InvalidCondition);

					if (TestBit<Nz::UInt64>(m_conditions.enabledConditions, conditionIndex))
						Visit(node.statement);
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
				const ShaderAst& m_shader;
				const SpirvWriter::States& m_conditions;
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
		SpirvSection instructions;
	};

	SpirvWriter::SpirvWriter() :
	m_currentState(nullptr)
	{
	}

	std::vector<UInt32> SpirvWriter::Generate(const ShaderAst& shader, const States& conditions)
	{
		std::string error;
		if (!ValidateShader(shader, &error))
			throw std::runtime_error("Invalid shader AST: " + error);

		m_context.shader = &shader;
		m_context.states = &conditions;

		State state;
		m_currentState = &state;
		CallOnExit onExit([this]()
		{
			m_currentState = nullptr;
		});

		std::vector<ShaderNodes::StatementPtr> functionStatements;

		ShaderAstCloner cloner;

		PreVisitor preVisitor(shader, conditions, state.constantTypeCache);
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
			RegisterPointerType(uniform.type, (IsSamplerType(uniform.type)) ? SpirvStorageClass::UniformConstant : SpirvStorageClass::Uniform);

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
			assert(IsBasicType(builtinExprType));

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
			variable.storageClass = (IsSamplerType(uniform.type)) ? SpirvStorageClass::UniformConstant : SpirvStorageClass::Uniform;
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

			SpirvAstVisitor visitor(*this);
			visitor.Visit(functionStatements[funcIndex]);

			if (func.returnType == ShaderNodes::BasicType::Void)
				state.instructions.Append(SpirvOp::OpReturn);

			state.instructions.Append(SpirvOp::OpFunctionEnd);
		}

		assert(entryPointIndex != std::numeric_limits<std::size_t>::max());

		m_currentState->constantTypeCache.Write(m_currentState->annotations, m_currentState->constants, m_currentState->debugInfo);

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
		MergeSections(ret, state.header);
		MergeSections(ret, state.debugInfo);
		MergeSections(ret, state.annotations);
		MergeSections(ret, state.constants);
		MergeSections(ret, state.instructions);

		return ret;
	}

	void SpirvWriter::SetEnv(Environment environment)
	{
		m_environment = std::move(environment);
	}

	UInt32 SpirvWriter::AllocateResultId()
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

	auto SpirvWriter::GetBuiltinVariable(ShaderNodes::BuiltinEntry builtin) const -> const ExtVar&
	{
		auto it = m_currentState->builtinIds.find(builtin);
		assert(it != m_currentState->builtinIds.end());

		return it->second;
	}

	auto SpirvWriter::GetInputVariable(const std::string& name) const -> const ExtVar&
	{
		auto it = m_currentState->inputIds.find(name);
		assert(it != m_currentState->inputIds.end());

		return it->second;
	}

	auto SpirvWriter::GetOutputVariable(const std::string& name) const -> const ExtVar&
	{
		auto it = m_currentState->outputIds.find(name);
		assert(it != m_currentState->outputIds.end());

		return it->second;
	}

	auto SpirvWriter::GetUniformVariable(const std::string& name) const -> const ExtVar&
	{
		auto it = m_currentState->uniformIds.find(name);
		assert(it != m_currentState->uniformIds.end());

		return it.value();
	}

	SpirvSection& SpirvWriter::GetInstructions()
	{
		return m_currentState->instructions;
	}

	UInt32 SpirvWriter::GetPointerTypeId(const ShaderExpressionType& type, SpirvStorageClass storageClass) const
	{
		return m_currentState->constantTypeCache.GetId(*SpirvConstantCache::BuildPointerType(*m_context.shader, type, storageClass));
	}

	UInt32 SpirvWriter::GetTypeId(const ShaderExpressionType& type) const
	{
		return m_currentState->constantTypeCache.GetId(*SpirvConstantCache::BuildType(*m_context.shader, type));
	}

	UInt32 SpirvWriter::ReadInputVariable(const std::string& name)
	{
		auto it = m_currentState->inputIds.find(name);
		assert(it != m_currentState->inputIds.end());

		return ReadVariable(it.value());
	}

	std::optional<UInt32> SpirvWriter::ReadInputVariable(const std::string& name, OnlyCache)
	{
		auto it = m_currentState->inputIds.find(name);
		assert(it != m_currentState->inputIds.end());

		return ReadVariable(it.value(), OnlyCache{});
	}

	UInt32 SpirvWriter::ReadLocalVariable(const std::string& name)
	{
		auto it = m_currentState->varToResult.find(name);
		assert(it != m_currentState->varToResult.end());

		return it->second;
	}

	std::optional<UInt32> SpirvWriter::ReadLocalVariable(const std::string& name, OnlyCache)
	{
		auto it = m_currentState->varToResult.find(name);
		if (it == m_currentState->varToResult.end())
			return {};

		return it->second;
	}

	UInt32 SpirvWriter::ReadUniformVariable(const std::string& name)
	{
		auto it = m_currentState->uniformIds.find(name);
		assert(it != m_currentState->uniformIds.end());

		return ReadVariable(it.value());
	}

	std::optional<UInt32> SpirvWriter::ReadUniformVariable(const std::string& name, OnlyCache)
	{
		auto it = m_currentState->uniformIds.find(name);
		assert(it != m_currentState->uniformIds.end());

		return ReadVariable(it.value(), OnlyCache{});
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

	std::optional<UInt32> SpirvWriter::ReadVariable(const ExtVar& var, OnlyCache)
	{
		if (!var.valueId.has_value())
			return {};

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

	void SpirvWriter::WriteLocalVariable(std::string name, UInt32 resultId)
	{
		assert(m_currentState);
		m_currentState->varToResult.insert_or_assign(std::move(name), resultId);
	}

	void SpirvWriter::MergeSections(std::vector<UInt32>& output, const SpirvSection& from)
	{
		const std::vector<UInt32>& bytecode = from.GetBytecode();

		std::size_t prevSize = output.size();
		output.resize(prevSize + bytecode.size());
		std::copy(bytecode.begin(), bytecode.end(), output.begin() + prevSize);
	}
}
