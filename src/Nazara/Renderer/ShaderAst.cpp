// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/ShaderAst.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	void ShaderAst::AddFunction(std::string name, ShaderNodes::StatementPtr statement, std::vector<FunctionParameter> parameters, ShaderNodes::ExpressionType returnType)
	{
		auto& functionEntry = m_functions.emplace_back();
		functionEntry.name = std::move(name);
		functionEntry.parameters = std::move(parameters);
		functionEntry.returnType = returnType;
		functionEntry.statement = std::move(statement);
	}

	void ShaderAst::AddInput(std::string name, ShaderNodes::ExpressionType type, std::optional<std::size_t> locationIndex)
	{
		auto& inputEntry = m_inputs.emplace_back();
		inputEntry.name = std::move(name);
		inputEntry.locationIndex = std::move(locationIndex);
		inputEntry.type = type;
	}

	void ShaderAst::AddOutput(std::string name, ShaderNodes::ExpressionType type, std::optional<std::size_t> locationIndex)
	{
		auto& outputEntry = m_outputs.emplace_back();
		outputEntry.name = std::move(name);
		outputEntry.locationIndex = std::move(locationIndex);
		outputEntry.type = type;
	}

	void ShaderAst::AddUniform(std::string name, ShaderNodes::ExpressionType type, std::optional<std::size_t> bindingIndex)
	{
		auto& uniformEntry = m_uniforms.emplace_back();
		uniformEntry.bindingIndex = std::move(bindingIndex);
		uniformEntry.name = std::move(name);
		uniformEntry.type = type;
	}
}
