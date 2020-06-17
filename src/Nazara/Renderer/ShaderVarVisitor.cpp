// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/ShaderVarVisitor.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	ShaderVarVisitor::~ShaderVarVisitor() = default;

	void ShaderVarVisitor::Visit(const ShaderNodes::VariablePtr& node)
	{
		node->Visit(*this);
	}
}
