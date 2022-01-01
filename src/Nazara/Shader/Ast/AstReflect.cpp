// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
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

	void AstReflect::Visit(DeclareFunctionStatement& node)
	{
		assert(m_callbacks);
		if (m_callbacks->onEntryPointDeclaration)
		{
			if (!node.entryStage.HasValue())
				return;

			m_callbacks->onEntryPointDeclaration(node.entryStage.GetResultingValue(), node.name);
		}
	}

	void AstReflect::Visit(DeclareOptionStatement& node)
	{
		assert(m_callbacks);
		if (m_callbacks->onOptionDeclaration)
			m_callbacks->onOptionDeclaration(node.optName, node.optType);
	}
}
