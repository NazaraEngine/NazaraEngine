// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADERVARVISITOR_HPP
#define NAZARA_SHADERVARVISITOR_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/ShaderVariables.hpp>

namespace Nz
{
	class NAZARA_SHADER_API ShaderVarVisitor
	{
		public:
			ShaderVarVisitor() = default;
			ShaderVarVisitor(const ShaderVarVisitor&) = delete;
			ShaderVarVisitor(ShaderVarVisitor&&) = delete;
			virtual ~ShaderVarVisitor();

			void Visit(const ShaderNodes::VariablePtr& node);

			virtual void Visit(ShaderNodes::BuiltinVariable& var) = 0;
			virtual void Visit(ShaderNodes::InputVariable& var) = 0;
			virtual void Visit(ShaderNodes::LocalVariable& var) = 0;
			virtual void Visit(ShaderNodes::OutputVariable& var) = 0;
			virtual void Visit(ShaderNodes::ParameterVariable& var) = 0;
			virtual void Visit(ShaderNodes::UniformVariable& var) = 0;
	};
}

#endif
