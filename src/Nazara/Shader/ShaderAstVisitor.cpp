// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderAstVisitor.hpp>
#include <Nazara/Shader/Debug.hpp>

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
