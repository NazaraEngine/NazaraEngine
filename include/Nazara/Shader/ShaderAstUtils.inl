// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderAstUtils.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	ShaderNodes::ExpressionCategory GetExpressionCategory(const ShaderNodes::ExpressionPtr& expression)
	{
		ShaderAstValueCategory visitor;
		return visitor.GetExpressionCategory(expression);
	}
}

#include <Nazara/Shader/DebugOff.hpp>
