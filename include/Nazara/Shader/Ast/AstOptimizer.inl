// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/Ast/AstOptimizer.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	inline StatementPtr Optimize(const StatementPtr& ast)
	{
		AstOptimizer optimize;
		return optimize.Optimise(ast);
	}

	inline StatementPtr Optimize(const StatementPtr& ast, UInt64 enabledConditions)
	{
		AstOptimizer optimize;
		return optimize.Optimise(ast, enabledConditions);
	}
}

#include <Nazara/Shader/DebugOff.hpp>
