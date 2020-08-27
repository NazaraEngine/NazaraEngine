// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderVariables.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderNodes
{
	inline std::shared_ptr<BuiltinVariable> BuiltinVariable::Build(BuiltinEntry variable, ShaderExpressionType varType)
	{
		auto node = std::make_shared<BuiltinVariable>();
		node->entry = variable;
		node->type = varType;

		return node;
	}

	inline std::shared_ptr<InputVariable> InputVariable::Build(std::string varName, ShaderExpressionType varType)
	{
		auto node = std::make_shared<InputVariable>();
		node->name = std::move(varName);
		node->type = varType;

		return node;
	}

	inline std::shared_ptr<LocalVariable> LocalVariable::Build(std::string varName, ShaderExpressionType varType)
	{
		auto node = std::make_shared<LocalVariable>();
		node->name = std::move(varName);
		node->type = varType;

		return node;
	}

	inline std::shared_ptr<OutputVariable> OutputVariable::Build(std::string varName, ShaderExpressionType varType)
	{
		auto node = std::make_shared<OutputVariable>();
		node->name = std::move(varName);
		node->type = varType;

		return node;
	}

	inline std::shared_ptr<ParameterVariable> ParameterVariable::Build(std::string varName, ShaderExpressionType varType)
	{
		auto node = std::make_shared<ParameterVariable>();
		node->name = std::move(varName);
		node->type = varType;

		return node;
	}

	inline std::shared_ptr<UniformVariable> UniformVariable::Build(std::string varName, ShaderExpressionType varType)
	{
		auto node = std::make_shared<UniformVariable>();
		node->name = std::move(varName);
		node->type = varType;

		return node;
	}
}

#include <Nazara/Shader/DebugOff.hpp>
