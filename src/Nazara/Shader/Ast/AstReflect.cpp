// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/Ast/AstReflect.hpp>
#include <stdexcept>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	void AstReflect::Reflect(Statement& statement, const Callbacks& callbacks)
	{
		m_callbacks = &callbacks;
		statement.Visit(*this);
	}

	void AstReflect::Visit(DeclareOptionStatement& node)
	{
		assert(m_callbacks);
		if (m_callbacks->onOptionDeclaration)
			m_callbacks->onOptionDeclaration(node.optName, node.optType);
	}
}
