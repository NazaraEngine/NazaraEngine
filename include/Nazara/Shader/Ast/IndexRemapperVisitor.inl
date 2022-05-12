// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/Ast/IndexRemapperVisitor.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	StatementPtr RemapIndices(Statement& statement, const IndexRemapperVisitor::Options& options)
	{
		IndexRemapperVisitor visitor;
		return visitor.Clone(statement, options);
	}
}

#include <Nazara/Shader/DebugOff.hpp>
