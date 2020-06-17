// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADERVARVISITOR_HPP
#define NAZARA_SHADERVARVISITOR_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/ShaderVariables.hpp>

namespace Nz
{
	class NAZARA_RENDERER_API ShaderVarVisitor
	{
		public:
			ShaderVarVisitor() = default;
			ShaderVarVisitor(const ShaderVarVisitor&) = delete;
			ShaderVarVisitor(ShaderVarVisitor&&) = delete;
			virtual ~ShaderVarVisitor();

			virtual void Visit(const ShaderNodes::BuiltinVariable& var) = 0;
			virtual void Visit(const ShaderNodes::InputVariable& var) = 0;
			virtual void Visit(const ShaderNodes::LocalVariable& var) = 0;
			virtual void Visit(const ShaderNodes::OutputVariable& var) = 0;
			virtual void Visit(const ShaderNodes::ParameterVariable& var) = 0;
			virtual void Visit(const ShaderNodes::UniformVariable& var) = 0;
			void Visit(const ShaderNodes::VariablePtr& node);
	};
}

#endif
