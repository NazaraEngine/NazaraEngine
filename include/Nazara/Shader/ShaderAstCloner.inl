// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderAstCloner.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	inline ExpressionPtr Clone(ExpressionPtr& node)
	{
		AstCloner cloner;
		return cloner.Clone(node);
	}

	inline StatementPtr Clone(StatementPtr& node)
	{
		AstCloner cloner;
		return cloner.Clone(node);
	}
}

#include <Nazara/Shader/DebugOff.hpp>
