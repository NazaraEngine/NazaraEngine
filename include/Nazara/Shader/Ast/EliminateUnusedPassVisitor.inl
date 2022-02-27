// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/Ast/EliminateUnusedPassVisitor.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	inline StatementPtr EliminateUnusedPassVisitor::Process(Statement& statement)
	{
		EliminateUnusedPassVisitor::Config defaultConfig;
		return Process(statement, defaultConfig);
	}

	StatementPtr EliminateUnusedPass(Statement& ast)
	{
		EliminateUnusedPassVisitor visitor;
		return visitor.Process(ast);
	}

	inline StatementPtr EliminateUnusedPass(Statement& ast, const EliminateUnusedPassVisitor::Config& config)
	{
		EliminateUnusedPassVisitor visitor;
		return visitor.Process(ast, config);
	}
}

#include <Nazara/Shader/DebugOff.hpp>
