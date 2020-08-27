// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderVarVisitor.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	ShaderVarVisitor::~ShaderVarVisitor() = default;

	void ShaderVarVisitor::Visit(const ShaderNodes::VariablePtr& node)
	{
		node->Visit(*this);
	}
}
