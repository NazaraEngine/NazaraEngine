// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/ShaderAstVisitor.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	ShaderAstVisitor::~ShaderAstVisitor() = default;

	void ShaderAstVisitor::EnableCondition(const std::string& name, bool cond)
	{
		if (cond)
			m_conditions.insert(name);
		else
			m_conditions.erase(name);
	}

	bool ShaderAstVisitor::IsConditionEnabled(const std::string& name) const
	{
		return m_conditions.count(name) != 0;
	}

	void ShaderAstVisitor::Visit(const ShaderNodes::NodePtr& node)
	{
		node->Visit(*this);
	}
}
