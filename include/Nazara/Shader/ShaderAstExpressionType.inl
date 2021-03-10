// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderAstExpressionType.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	inline ShaderExpressionType GetExpressionType(Expression& expression, AstCache* cache)
	{
		ExpressionTypeVisitor visitor;
		return visitor.GetExpressionType(expression, cache);
	}
}

#include <Nazara/Shader/DebugOff.hpp>
