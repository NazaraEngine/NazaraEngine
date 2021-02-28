// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderAst.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	void ShaderAst::AddCondition(std::string name)
	{
		auto& conditionEntry = m_conditions.emplace_back();
		conditionEntry.name = std::move(name);
	}

	void ShaderAst::AddFunction(std::string name, ShaderNodes::StatementPtr statement, std::vector<FunctionParameter> parameters, ShaderExpressionType returnType)
	{
		auto& functionEntry = m_functions.emplace_back();
		functionEntry.name = std::move(name);
		functionEntry.parameters = std::move(parameters);
		functionEntry.returnType = returnType;
		functionEntry.statement = std::move(statement);
	}

	void ShaderAst::AddInput(std::string name, ShaderExpressionType type, std::optional<std::size_t> locationIndex)
	{
		auto& inputEntry = m_inputs.emplace_back();
		inputEntry.name = std::move(name);
		inputEntry.locationIndex = std::move(locationIndex);
		inputEntry.type = std::move(type);
	}

	void ShaderAst::AddOutput(std::string name, ShaderExpressionType type, std::optional<std::size_t> locationIndex)
	{
		auto& outputEntry = m_outputs.emplace_back();
		outputEntry.name = std::move(name);
		outputEntry.locationIndex = std::move(locationIndex);
		outputEntry.type = std::move(type);
	}

	void ShaderAst::AddStruct(std::string name, std::vector<StructMember> members)
	{
		auto& structEntry = m_structs.emplace_back();
		structEntry.name = std::move(name);
		structEntry.members = std::move(members);
	}

	void ShaderAst::AddUniform(std::string name, ShaderExpressionType type, std::optional<std::size_t> bindingIndex, std::optional<ShaderNodes::MemoryLayout> memoryLayout)
	{
		auto& uniformEntry = m_uniforms.emplace_back();
		uniformEntry.bindingIndex = std::move(bindingIndex);
		uniformEntry.memoryLayout = std::move(memoryLayout);
		uniformEntry.name = std::move(name);
		uniformEntry.type = std::move(type);
	}
}
