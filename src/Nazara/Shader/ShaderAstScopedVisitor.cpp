// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderAstScopedVisitor.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	void AstScopedVisitor::ScopedVisit(StatementPtr& nodePtr)
	{
		PushScope(); //< Global scope
		{
			nodePtr->Visit(*this);
		}
		PopScope();
	}

	void AstScopedVisitor::Visit(BranchStatement& node)
	{
		for (auto& cond : node.condStatements)
		{
			PushScope();
			{
				cond.condition->Visit(*this);
				cond.statement->Visit(*this);
			}
			PopScope();
		}

		if (node.elseStatement)
		{
			PushScope();
			{
				node.elseStatement->Visit(*this);
			}
			PopScope();
		}
	}

	void AstScopedVisitor::Visit(ConditionalStatement& node)
	{
		PushScope();
		{
			AstRecursiveVisitor::Visit(node);
		}
		PopScope();
	}

	void AstScopedVisitor::Visit(DeclareExternalStatement& node)
	{
		for (auto& extVar : node.externalVars)
		{
			ExpressionType subType = extVar.type;
			if (IsUniformType(subType))
				subType = std::get<IdentifierType>(std::get<UniformType>(subType).containedType);

			RegisterVariable(extVar.name, std::move(subType));
		}

		AstRecursiveVisitor::Visit(node);
	}

	void AstScopedVisitor::Visit(DeclareFunctionStatement& node)
	{
		PushScope();
		{
			for (auto& parameter : node.parameters)
				RegisterVariable(parameter.name, parameter.type);

			AstRecursiveVisitor::Visit(node);
		}
		PopScope();
	}

	void AstScopedVisitor::Visit(DeclareStructStatement& node)
	{
		RegisterStruct(node.description);

		AstRecursiveVisitor::Visit(node);
	}

	void AstScopedVisitor::Visit(DeclareVariableStatement& node)
	{
		RegisterVariable(node.varName, node.varType);

		AstRecursiveVisitor::Visit(node);
	}

	void AstScopedVisitor::Visit(MultiStatement& node)
	{
		PushScope();
		{
			AstRecursiveVisitor::Visit(node);
		}
		PopScope();
	}

	void AstScopedVisitor::PushScope()
	{
		m_scopeSizes.push_back(m_identifiersInScope.size());
	}

	void AstScopedVisitor::PopScope()
	{
		assert(!m_scopeSizes.empty());
		m_identifiersInScope.resize(m_scopeSizes.back());
		m_scopeSizes.pop_back();
	}
}
