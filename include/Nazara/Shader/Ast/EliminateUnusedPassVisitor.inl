// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/Ast/EliminateUnusedPassVisitor.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	inline StatementPtr EliminateUnusedPass(Statement& ast, const DependencyCheckerVisitor::Config& config)
	{
		DependencyCheckerVisitor dependencyVisitor;
		dependencyVisitor.Process(ast, config);
		dependencyVisitor.Resolve();

		return EliminateUnusedPass(ast, dependencyVisitor.GetUsage());
	}

	StatementPtr EliminateUnusedPass(Statement& ast, const DependencyCheckerVisitor::UsageSet& usageSet)
	{
		EliminateUnusedPassVisitor visitor;
		return visitor.Process(ast, usageSet);
	}
}

#include <Nazara/Shader/DebugOff.hpp>
