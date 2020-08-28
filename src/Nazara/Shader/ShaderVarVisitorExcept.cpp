// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderVarVisitorExcept.hpp>
#include <stdexcept>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	void ShaderVarVisitorExcept::Visit(ShaderNodes::BuiltinVariable& /*var*/)
	{
		throw std::runtime_error("unhandled BuiltinVariable");
	}

	void ShaderVarVisitorExcept::Visit(ShaderNodes::InputVariable& /*var*/)
	{
		throw std::runtime_error("unhandled InputVariable");
	}

	void ShaderVarVisitorExcept::Visit(ShaderNodes::LocalVariable& /*var*/)
	{
		throw std::runtime_error("unhandled LocalVariable");
	}

	void ShaderVarVisitorExcept::Visit(ShaderNodes::OutputVariable& /*var*/)
	{
		throw std::runtime_error("unhandled OutputVariable");
	}

	void ShaderVarVisitorExcept::Visit(ShaderNodes::ParameterVariable& /*var*/)
	{
		throw std::runtime_error("unhandled ParameterVariable");
	}

	void ShaderVarVisitorExcept::Visit(ShaderNodes::UniformVariable& /*var*/)
	{
		throw std::runtime_error("unhandled UniformVariable");
	}
}
