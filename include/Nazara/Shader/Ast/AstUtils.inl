// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/Ast/AstUtils.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	ExpressionCategory GetExpressionCategory(Expression& expression)
	{
		ShaderAstValueCategory visitor;
		return visitor.GetExpressionCategory(expression);
	}
}

#include <Nazara/Shader/DebugOff.hpp>
