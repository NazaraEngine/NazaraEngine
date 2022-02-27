// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/Ast/DependencyCheckerVisitor.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	inline auto DependencyCheckerVisitor::GetUsage() const -> const UsageSet&
	{
		return m_resolvedUsage;
	}

	inline void DependencyCheckerVisitor::Process(Statement& statement)
	{
		Config defaultConfig;
		return Process(statement, defaultConfig);
	}

	void DependencyCheckerVisitor::Resolve()
	{
		Resolve(m_globalUsage);
	}
}

#include <Nazara/Shader/DebugOff.hpp>
