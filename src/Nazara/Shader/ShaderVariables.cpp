// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderVariables.hpp>
#include <Nazara/Shader/ShaderVarVisitor.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderNodes
{
	ShaderNodes::Variable::~Variable() = default;

	VariableType BuiltinVariable::GetType() const
	{
		return VariableType::BuiltinVariable;
	}

	void BuiltinVariable::Visit(ShaderVarVisitor& visitor)
	{
		visitor.Visit(*this);
	}


	VariableType InputVariable::GetType() const
	{
		return VariableType::InputVariable;
	}

	void InputVariable::Visit(ShaderVarVisitor& visitor)
	{
		visitor.Visit(*this);
	}


	VariableType LocalVariable::GetType() const
	{
		return VariableType::LocalVariable;
	}

	void LocalVariable::Visit(ShaderVarVisitor& visitor)
	{
		visitor.Visit(*this);
	}


	VariableType OutputVariable::GetType() const
	{
		return VariableType::OutputVariable;
	}

	void OutputVariable::Visit(ShaderVarVisitor& visitor)
	{
		visitor.Visit(*this);
	}


	VariableType ParameterVariable::GetType() const
	{
		return VariableType::ParameterVariable;
	}

	void ParameterVariable::Visit(ShaderVarVisitor& visitor)
	{
		visitor.Visit(*this);
	}


	VariableType UniformVariable::GetType() const
	{
		return VariableType::UniformVariable;
	}

	void UniformVariable::Visit(ShaderVarVisitor& visitor)
	{
		visitor.Visit(*this);
	}
}
