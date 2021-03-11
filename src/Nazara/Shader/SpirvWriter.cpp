// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/SpirvWriter.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/StackVector.hpp>
#include <Nazara/Shader/ShaderAstCloner.hpp>
#include <Nazara/Shader/ShaderAstValidator.hpp>
#include <Nazara/Shader/SpirvAstVisitor.hpp>
#include <Nazara/Shader/SpirvBlock.hpp>
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
		class PreVisitor : public ShaderAst::AstRecursiveVisitor
		{
			public:
				using ExtInstList = std::unordered_set<std::string>;
				using LocalContainer = std::unordered_set<ShaderAst::ShaderExpressionType>;
				using FunctionContainer = std::vector<std::reference_wrapper<ShaderAst::DeclareFunctionStatement>>;

				PreVisitor(ShaderAst::AstCache* cache, const SpirvWriter::States& conditions, SpirvConstantCache& constantCache) :
				m_cache(cache),
				m_conditions(conditions),
				m_constantCache(constantCache)
				{
				}

				void Visit(ShaderAst::AccessMemberExpression& node) override
				{
					/*for (std::size_t index : node.memberIdentifiers)
						m_constantCache.Register(*SpirvConstantCache::BuildConstant(Int32(index)));*/

					AstRecursiveVisitor::Visit(node);
				}

				void Visit(ShaderAst::ConditionalExpression& node) override
				{
					/*std::size_t conditionIndex = m_shader.FindConditionByName(node.conditionName);
					assert(conditionIndex != ShaderAst::InvalidCondition);

					if (TestBit<Nz::UInt64>(m_conditions.enabledConditions, conditionIndex))
						Visit(node.truePath);
					else
						Visit(node.falsePath);*/
				}

				void Visit(ShaderAst::ConditionalStatement& node) override
				{
					/*std::size_t conditionIndex = m_shader.FindConditionByName(node.conditionName);
					assert(conditionIndex != ShaderAst::InvalidCondition);

					if (TestBit<Nz::UInt64>(m_conditions.enabledConditions, conditionIndex))
						Visit(node.statement);*/
				}

				void Visit(ShaderAst::ConstantExpression& node) override
				{
					std::visit([&](auto&& arg)
					{
						m_constantCache.Register(*SpirvConstantCache::BuildConstant(arg));
					}, node.value);

					AstRecursiveVisitor::Visit(node);
				}

				void Visit(ShaderAst::DeclareFunctionStatement& node) override
				{
					funcs.emplace_back(node);

					std::vector<ShaderAst::ShaderExpressionType> parameterTypes;
					for (auto& parameter : node.parameters)
						parameterTypes.push_back(parameter.type);

					m_constantCache.Register(*SpirvConstantCache::BuildFunctionType(node.returnType, parameterTypes));

					AstRecursiveVisitor::Visit(node);
				}

				void Visit(ShaderAst::DeclareStructStatement& node) override
				{
					for (auto& field : node.description.members)
						m_constantCache.Register(*SpirvConstantCache::BuildType(field.type));
				}

				void Visit(ShaderAst::DeclareVariableStatement& node) override
				{
					m_constantCache.Register(*SpirvConstantCache::BuildType(node.varType));

					AstRecursiveVisitor::Visit(node);
				}

				void Visit(ShaderAst::IdentifierExpression& node) override
				{
					m_constantCache.Register(*SpirvConstantCache::BuildType(GetExpressionType(node, m_cache)));

					AstRecursiveVisitor::Visit(node);
				}

				void Visit(ShaderAst::IntrinsicExpression& node) override
				{
					AstRecursiveVisitor::Visit(node);

					switch (node.intrinsic)
					{
						// Require GLSL.std.450
						case ShaderAst::IntrinsicType::CrossProduct:
							extInsts.emplace("GLSL.std.450");
							break;

						// Part of SPIR-V core
						case ShaderAst::IntrinsicType::DotProduct:
							break;
					}
				}

				ExtInstList extInsts;
				FunctionContainer funcs;

			private:
				ShaderAst::AstCache* m_cache;
				const SpirvWriter::States& m_conditions;
				SpirvConstantCache& m_constantCache;
		};

		template<typename T>
		constexpr ShaderAst::BasicType GetBasicType()
		{
			if constexpr (std::is_same_v<T, bool>)
				return ShaderAst::BasicType::Boolean;
			else if constexpr (std::is_same_v<T, float>)
				return(ShaderAst::BasicType::Float1);
			else if constexpr (std::is_same_v<T, Int32>)
				return(ShaderAst::BasicType::Int1);
			else if constexpr (std::is_same_v<T, Vector2f>)
				return(ShaderAst::BasicType::Float2);
			else if constexpr (std::is_same_v<T, Vector3f>)
				return(ShaderAst::BasicType::Float3);
			else if constexpr (std::is_same_v<T, Vector4f>)
				return(ShaderAst::BasicType::Float4);
			else if constexpr (std::is_same_v<T, Vector2i32>)
				return(ShaderAst::BasicType::Int2);
			else if constexpr (std::is_same_v<T, Vector3i32>)
				return(ShaderAst::BasicType::Int3);
			else if constexpr (std::is_same_v<T, Vector4i32>)
				return(ShaderAst::BasicType::Int4);
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
		};

		tsl::ordered_map<std::string, ExtVar> inputIds;
		tsl::ordered_map<std::string, ExtVar> outputIds;
		tsl::ordered_map<std::string, ExtVar> parameterIds;
		tsl::ordered_map<std::string, ExtVar> uniformIds;
		std::unordered_map<std::string, UInt32> extensionInstructions;
		std::unordered_map<ShaderAst::BuiltinEntry, ExtVar> builtinIds;
		std::unordered_map<std::string, UInt32> varToResult;
		std::vector<Func> funcs;
		std::vector<SpirvBlock> functionBlocks;
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

	std::vector<UInt32> SpirvWriter::Generate(ShaderAst::StatementPtr& shader, const States& conditions)
	{
		std::string error;
		if (!ShaderAst::ValidateAst(shader, &error, &m_context.cache))
			throw std::runtime_error("Invalid shader AST: " + error);

		m_context.states = &conditions;

		State state;
		m_currentState = &state;
		CallOnExit onExit([this]()
		{
			m_currentState = nullptr;
		});

		ShaderAst::AstCloner cloner;

		// Register all extended instruction sets
		PreVisitor preVisitor(&m_context.cache, conditions, state.constantTypeCache);
		shader->Visit(preVisitor);

		for (const std::string& extInst : preVisitor.extInsts)
			state.extensionInstructions[extInst] = AllocateResultId();

		// Register all types
		/*for (const auto& func : shader.GetFunctions())
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

		for (const auto& type : preVisitor.variableTypes)
			RegisterType(type);

		for (const auto& builtin : preVisitor.builtinVars)
			RegisterType(builtin->type);

		// Register result id and debug infos for global variables/functions
		for (const auto& builtin : preVisitor.builtinVars)
		{
			SpirvConstantCache::Variable variable;
			SpirvBuiltIn builtinDecoration;
			switch (builtin->entry)
			{
				case ShaderAst::BuiltinEntry::VertexPosition:
					variable.debugName = "builtin_VertexPosition";
					variable.storageClass = SpirvStorageClass::Output;

					builtinDecoration = SpirvBuiltIn::Position;
					break;

				default:
					throw std::runtime_error("unexpected builtin type");
			}

			const ShaderAst::ShaderExpressionType& builtinExprType = builtin->type;
			assert(IsBasicType(builtinExprType));

			ShaderAst::BasicType builtinType = std::get<ShaderAst::BasicType>(builtinExprType);

			variable.type = SpirvConstantCache::BuildPointerType(builtinType, variable.storageClass);

			UInt32 varId = m_currentState->constantTypeCache.Register(variable);

			ExtVar builtinData;
			builtinData.pointerTypeId = GetPointerTypeId(builtinType, variable.storageClass);
			builtinData.typeId = GetTypeId(builtinType);
			builtinData.varId = varId;

			state.annotations.Append(SpirvOp::OpDecorate, builtinData.varId, SpirvDecoration::BuiltIn, builtinDecoration);

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
				state.annotations.Append(SpirvOp::OpDecorate, varId, SpirvDecoration::Location, *input.locationIndex);
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
				state.annotations.Append(SpirvOp::OpDecorate, varId, SpirvDecoration::Location, *output.locationIndex);
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
				state.annotations.Append(SpirvOp::OpDecorate, varId, SpirvDecoration::Binding, *uniform.bindingIndex);
				state.annotations.Append(SpirvOp::OpDecorate, varId, SpirvDecoration::DescriptorSet, 0);
			}
		}*/

		for (const ShaderAst::DeclareFunctionStatement& func : preVisitor.funcs)
		{
			auto& funcData = state.funcs.emplace_back();
			funcData.id = AllocateResultId();
			funcData.typeId = GetFunctionTypeId(func);

			state.debugInfo.Append(SpirvOp::OpName, funcData.id, func.name);
		}

		std::size_t funcIndex = 0;

		for (const ShaderAst::DeclareFunctionStatement& func : preVisitor.funcs)
		{
			auto& funcData = state.funcs[funcIndex++];

			state.instructions.Append(SpirvOp::OpFunction, GetTypeId(func.returnType), funcData.id, 0, funcData.typeId);

			state.functionBlocks.clear();
			state.functionBlocks.emplace_back(*this);

			state.parameterIds.clear();

			for (const auto& param : func.parameters)
			{
				UInt32 paramResultId = AllocateResultId();
				state.instructions.Append(SpirvOp::OpFunctionParameter, GetTypeId(param.type), paramResultId);

				ExtVar parameterData;
				parameterData.pointerTypeId = GetPointerTypeId(param.type, SpirvStorageClass::Function);
				parameterData.typeId = GetTypeId(param.type);
				parameterData.varId = paramResultId;

				state.parameterIds.emplace(param.name, std::move(parameterData));
			}

			SpirvAstVisitor visitor(*this, state.functionBlocks, &m_context.cache);
			for (const auto& statement : func.statements)
				statement->Visit(visitor);

			if (!state.functionBlocks.back().IsTerminated())
			{
				assert(func.returnType == ShaderAst::ShaderExpressionType(ShaderAst::BasicType::Void));
				state.functionBlocks.back().Append(SpirvOp::OpReturn);
			}

			for (SpirvBlock& block : state.functionBlocks)
				state.instructions.AppendSection(block);

			state.instructions.Append(SpirvOp::OpFunctionEnd);
		}

		m_currentState->constantTypeCache.Write(m_currentState->annotations, m_currentState->constants, m_currentState->debugInfo);

		AppendHeader();

		/*if (entryPointIndex != std::numeric_limits<std::size_t>::max())
		{
			SpvExecutionModel execModel;
			const auto& entryFuncData = shader.GetFunction(entryPointIndex);
			const auto& entryFunc = state.funcs[entryPointIndex];

			assert(m_context.shader);
			switch (m_context.shader->GetStage())
			{
				case ShaderStageType::Fragment:
					execModel = SpirvExecutionModel::Fragment;
					break;

				case ShaderStageType::Vertex:
					execModel = SpirvExecutionModel::Vertex;
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
				state.header.Append(SpirvOp::OpExecutionMode, entryFunc.id, SpirvExecutionMode::OriginUpperLeft);
		}*/

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
		m_currentState->header.AppendRaw(SpvMagicNumber); //< Spir-V magic number

		UInt32 version = (m_environment.spvMajorVersion << 16) | m_environment.spvMinorVersion << 8;
		m_currentState->header.AppendRaw(version); //< Spir-V version number (1.0 for compatibility)
		m_currentState->header.AppendRaw(0); //< Generator identifier (TODO: Register generator to Khronos)

		m_currentState->header.AppendRaw(m_currentState->nextVarIndex); //< Bound (ID count)
		m_currentState->header.AppendRaw(0); //< Instruction schema (required to be 0 for now)

		m_currentState->header.Append(SpirvOp::OpCapability, SpirvCapability::Shader);

		for (const auto& [extInst, resultId] : m_currentState->extensionInstructions)
			m_currentState->header.Append(SpirvOp::OpExtInstImport, resultId, extInst);

		m_currentState->header.Append(SpirvOp::OpMemoryModel, SpirvAddressingModel::Logical, SpirvMemoryModel::GLSL450);
	}

	UInt32 SpirvWriter::GetConstantId(const ShaderConstantValue& value) const
	{
		return m_currentState->constantTypeCache.GetId(*SpirvConstantCache::BuildConstant(value));
	}

	UInt32 SpirvWriter::GetFunctionTypeId(const ShaderAst::DeclareFunctionStatement& functionNode)
	{
		return m_currentState->constantTypeCache.GetId({ *BuildFunctionType(functionNode) });
	}

	auto SpirvWriter::GetBuiltinVariable(ShaderAst::BuiltinEntry builtin) const -> const ExtVar&
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

	UInt32 SpirvWriter::GetPointerTypeId(const ShaderAst::ShaderExpressionType& type, SpirvStorageClass storageClass) const
	{
		return m_currentState->constantTypeCache.GetId(*SpirvConstantCache::BuildPointerType(type, storageClass));
	}

	UInt32 SpirvWriter::GetTypeId(const ShaderAst::ShaderExpressionType& type) const
	{
		return m_currentState->constantTypeCache.GetId(*SpirvConstantCache::BuildType(type));
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

	UInt32 SpirvWriter::ReadParameterVariable(const std::string& name)
	{
		auto it = m_currentState->parameterIds.find(name);
		assert(it != m_currentState->parameterIds.end());

		return ReadVariable(it.value());
	}

	std::optional<UInt32> SpirvWriter::ReadParameterVariable(const std::string& name, OnlyCache)
	{
		auto it = m_currentState->parameterIds.find(name);
		assert(it != m_currentState->parameterIds.end());

		return ReadVariable(it.value(), OnlyCache{});
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
			m_currentState->functionBlocks.back().Append(SpirvOp::OpLoad, var.typeId, resultId, var.varId);

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

	UInt32 SpirvWriter::RegisterFunctionType(const ShaderAst::DeclareFunctionStatement& functionNode)
	{
		return m_currentState->constantTypeCache.Register({ *BuildFunctionType(functionNode) });
	}

	UInt32 SpirvWriter::RegisterPointerType(ShaderAst::ShaderExpressionType type, SpirvStorageClass storageClass)
	{
		return m_currentState->constantTypeCache.Register(*SpirvConstantCache::BuildPointerType(type, storageClass));
	}

	UInt32 SpirvWriter::RegisterType(ShaderAst::ShaderExpressionType type)
	{
		assert(m_currentState);
		return m_currentState->constantTypeCache.Register(*SpirvConstantCache::BuildType(type));
	}

	void SpirvWriter::WriteLocalVariable(std::string name, UInt32 resultId)
	{
		assert(m_currentState);
		m_currentState->varToResult.insert_or_assign(std::move(name), resultId);
	}

	SpirvConstantCache::TypePtr SpirvWriter::BuildFunctionType(const ShaderAst::DeclareFunctionStatement& functionNode)
	{
		std::vector<ShaderAst::ShaderExpressionType> parameterTypes;
		parameterTypes.reserve(functionNode.parameters.size());

		for (const auto& parameter : functionNode.parameters)
			parameterTypes.push_back(parameter.type);

		return SpirvConstantCache::BuildFunctionType(functionNode.returnType, parameterTypes);
	}

	void SpirvWriter::MergeSections(std::vector<UInt32>& output, const SpirvSection& from)
	{
		const std::vector<UInt32>& bytecode = from.GetBytecode();

		std::size_t prevSize = output.size();
		output.resize(prevSize + bytecode.size());
		std::copy(bytecode.begin(), bytecode.end(), output.begin() + prevSize);
	}
}
