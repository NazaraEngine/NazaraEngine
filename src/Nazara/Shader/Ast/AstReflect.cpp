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

	void AstReflect::Visit(DeclareAliasStatement& node)
	{
		assert(m_callbacks);
		if (m_callbacks->onAliasDeclaration)
			m_callbacks->onAliasDeclaration(node);

		if (m_callbacks->onAliasIndex && node.aliasIndex)
			m_callbacks->onAliasIndex(node.name, *node.aliasIndex, node.sourceLocation);

		AstRecursiveVisitor::Visit(node);
	}

	void AstReflect::Visit(DeclareConstStatement& node)
	{
		assert(m_callbacks);
		if (m_callbacks->onConstDeclaration)
			m_callbacks->onConstDeclaration(node);

		if (m_callbacks->onConstIndex && node.constIndex)
			m_callbacks->onConstIndex(node.name, *node.constIndex, node.sourceLocation);

		AstRecursiveVisitor::Visit(node);
	}

	void AstReflect::Visit(DeclareExternalStatement& node)
	{
		assert(m_callbacks);
		if (m_callbacks->onExternalDeclaration)
			m_callbacks->onExternalDeclaration(node);

		if (m_callbacks->onVariableIndex)
		{
			for (const auto& extVar : node.externalVars)
			{
				if (extVar.varIndex)
					m_callbacks->onVariableIndex(extVar.name, *extVar.varIndex, extVar.sourceLocation);
			}
		}

		AstRecursiveVisitor::Visit(node);
	}

	void AstReflect::Visit(DeclareFunctionStatement& node)
	{
		assert(m_callbacks);

		if (m_callbacks->onFunctionDeclaration)
			m_callbacks->onFunctionDeclaration(node);

		if (node.funcIndex && m_callbacks->onFunctionIndex)
			m_callbacks->onFunctionIndex(node.name, *node.funcIndex, node.sourceLocation);

		if (m_callbacks->onEntryPointDeclaration)
		{
			if (!node.entryStage.HasValue())
				return;

			m_callbacks->onEntryPointDeclaration(node.entryStage.GetResultingValue(), node.name);
		}

		if (m_callbacks->onVariableIndex)
		{
			for (const auto& parameter : node.parameters)
			{
				if (parameter.varIndex)
					m_callbacks->onVariableIndex(parameter.name, *parameter.varIndex, parameter.sourceLocation);
			}
		}

		AstRecursiveVisitor::Visit(node);
	}

	void AstReflect::Visit(DeclareOptionStatement& node)
	{
		assert(m_callbacks);
		if (m_callbacks->onOptionDeclaration)
			m_callbacks->onOptionDeclaration(node);

		if (m_callbacks->onOptionIndex && node.optIndex)
			m_callbacks->onOptionIndex(node.optName, *node.optIndex, node.sourceLocation);

		AstRecursiveVisitor::Visit(node);
	}

	void AstReflect::Visit(DeclareStructStatement& node)
	{
		assert(m_callbacks);
		if (m_callbacks->onStructDeclaration)
			m_callbacks->onStructDeclaration(node);

		if (m_callbacks->onStructIndex && node.structIndex)
			m_callbacks->onStructIndex(node.description.name, *node.structIndex, node.sourceLocation);

		AstRecursiveVisitor::Visit(node);
	}

	void AstReflect::Visit(DeclareVariableStatement& node)
	{
		assert(m_callbacks);
		if (m_callbacks->onVariableDeclaration)
			m_callbacks->onVariableDeclaration(node);

		if (m_callbacks->onVariableIndex && node.varIndex)
			m_callbacks->onVariableIndex(node.varName, *node.varIndex, node.sourceLocation);

		AstRecursiveVisitor::Visit(node);
	}

	void AstReflect::Visit(ForStatement& node)
	{
		assert(m_callbacks);
		if (m_callbacks->onVariableIndex && node.varIndex)
			m_callbacks->onVariableIndex(node.varName, *node.varIndex, node.sourceLocation);

		AstRecursiveVisitor::Visit(node);
	}

	void AstReflect::Visit(ForEachStatement& node)
	{
		assert(m_callbacks);
		if (m_callbacks->onVariableIndex && node.varIndex)
			m_callbacks->onVariableIndex(node.varName, *node.varIndex, node.sourceLocation);

		AstRecursiveVisitor::Visit(node);
	}
}
