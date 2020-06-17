// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/ShaderVariables.hpp>
#include <Nazara/Renderer/ShaderVarVisitor.hpp>
#include <Nazara/Renderer/Debug.hpp>

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
