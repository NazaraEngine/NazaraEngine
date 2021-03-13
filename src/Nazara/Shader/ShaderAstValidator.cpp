// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderAstValidator.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Shader/ShaderAstUtils.hpp>
#include <Nazara/Shader/ShaderAstExpressionType.hpp>
#include <unordered_set>
#include <vector>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	namespace
	{
		std::unordered_map<std::string, ShaderStageType> entryPoints = {
			{ "frag", ShaderStageType::Fragment },
			{ "vert", ShaderStageType::Vertex },
		};

	}

	struct AstError
	{
		std::string errMsg;
	};

	struct AstValidator::Context
	{
		//const ShaderAst::Function* currentFunction;
		std::optional<std::size_t> activeScopeId;
		AstCache* cache;
	};

	bool AstValidator::Validate(StatementPtr& node, std::string* error, AstCache* cache)
	{
		try
		{
			AstCache dummy;

			Context currentContext;
			currentContext.cache = (cache) ? cache : &dummy;

			m_context = &currentContext;
			CallOnExit resetContext([&] { m_context = nullptr; });

			EnterScope();
			node->Visit(*this);
			ExitScope();

			return true;
		}
		catch (const AstError& e)
		{
			if (error)
				*error = e.errMsg;

			return false;
		}
	}

	Expression& AstValidator::MandatoryExpr(ExpressionPtr& node)
	{
		if (!node)
			throw AstError{ "Invalid expression" };

		return *node;
	}

	Statement& AstValidator::MandatoryStatement(StatementPtr& node)
	{
		if (!node)
			throw AstError{ "Invalid statement" };

		return *node;
	}

	void AstValidator::TypeMustMatch(ExpressionPtr& left, ExpressionPtr& right)
	{
		return TypeMustMatch(GetExpressionType(*left, m_context->cache), GetExpressionType(*right, m_context->cache));
	}

	void AstValidator::TypeMustMatch(const ShaderExpressionType& left, const ShaderExpressionType& right)
	{
		if (left != right)
			throw AstError{ "Left expression type must match right expression type" };
	}

	ShaderExpressionType AstValidator::CheckField(const std::string& structName, const std::string* memberIdentifier, std::size_t remainingMembers)
	{
		const AstCache::Identifier* identifier = m_context->cache->FindIdentifier(*m_context->activeScopeId, structName);
		if (!identifier)
			throw AstError{ "unknown identifier " + structName };

		if (std::holds_alternative<StructDescription>(identifier->value))
			throw AstError{ "identifier is not a struct" };

		const StructDescription& s = std::get<StructDescription>(identifier->value);

		auto memberIt = std::find_if(s.members.begin(), s.members.begin(), [&](const auto& field) { return field.name == memberIdentifier[0]; });
		if (memberIt == s.members.end())
			throw AstError{ "unknown field " + memberIdentifier[0]};

		const auto& member = *memberIt;

		if (remainingMembers > 1)
			return CheckField(std::get<std::string>(member.type), memberIdentifier + 1, remainingMembers - 1);
		else
			return member.type;
	}

	AstCache::Scope& AstValidator::EnterScope()
	{
		std::size_t newScopeId = m_context->cache->scopes.size();

		std::optional<std::size_t> previousScope = m_context->activeScopeId;

		auto& newScope = m_context->cache->scopes.emplace_back();
		newScope.parentScopeIndex = previousScope;

		m_context->activeScopeId = newScopeId;
		return m_context->cache->scopes[newScopeId];
	}

	void AstValidator::ExitScope()
	{
		assert(m_context->activeScopeId);
		auto& previousScope = m_context->cache->scopes[*m_context->activeScopeId];
		m_context->activeScopeId = previousScope.parentScopeIndex;
	}

	void AstValidator::RegisterExpressionType(Expression& node, ShaderExpressionType expressionType)
	{
		m_context->cache->nodeExpressionType[&node] = std::move(expressionType);
	}

	void AstValidator::RegisterScope(Node& node)
	{
		if (m_context->activeScopeId)
			m_context->cache->scopeIdByNode[&node] = *m_context->activeScopeId;
	}

	void AstValidator::Visit(AccessMemberExpression& node)
	{
		RegisterScope(node);

		ShaderExpressionType exprType = GetExpressionType(MandatoryExpr(node.structExpr), m_context->cache);
		if (!IsStructType(exprType))
			throw AstError{ "expression is not a structure" };

		const std::string& structName = std::get<std::string>(exprType);

		RegisterExpressionType(node, CheckField(structName, node.memberIdentifiers.data(), node.memberIdentifiers.size()));
	}

	void AstValidator::Visit(AssignExpression& node)
	{
		RegisterScope(node);

		MandatoryExpr(node.left);
		MandatoryExpr(node.right);
		TypeMustMatch(node.left, node.right);

		if (GetExpressionCategory(*node.left) != ExpressionCategory::LValue)
			throw AstError { "Assignation is only possible with a l-value" };

		AstRecursiveVisitor::Visit(node);
	}

	void AstValidator::Visit(BinaryExpression& node)
	{
		RegisterScope(node);

		// Register expression type
		AstRecursiveVisitor::Visit(node);

		ShaderExpressionType leftExprType = GetExpressionType(MandatoryExpr(node.left), m_context->cache);
		if (!IsBasicType(leftExprType))
			throw AstError{ "left expression type does not support binary operation" };

		ShaderExpressionType rightExprType = GetExpressionType(MandatoryExpr(node.right), m_context->cache);
		if (!IsBasicType(rightExprType))
			throw AstError{ "right expression type does not support binary operation" };

		BasicType leftType = std::get<BasicType>(leftExprType);
		BasicType rightType = std::get<BasicType>(rightExprType);

		switch (node.op)
		{
			case BinaryType::CompGe:
			case BinaryType::CompGt:
			case BinaryType::CompLe:
			case BinaryType::CompLt:
				if (leftType == BasicType::Boolean)
					throw AstError{ "this operation is not supported for booleans" };

				[[fallthrough]];
			case BinaryType::Add:
			case BinaryType::CompEq:
			case BinaryType::CompNe:
			case BinaryType::Subtract:
				TypeMustMatch(node.left, node.right);
				break;

			case BinaryType::Multiply:
			case BinaryType::Divide:
			{
				switch (leftType)
				{
					case BasicType::Float1:
					case BasicType::Int1:
					{
						if (GetComponentType(rightType) != leftType)
							throw AstError{ "Left expression type is not compatible with right expression type" };

						break;
					}

					case BasicType::Float2:
					case BasicType::Float3:
					case BasicType::Float4:
					case BasicType::Int2:
					case BasicType::Int3:
					case BasicType::Int4:
					{
						if (leftType != rightType && rightType != GetComponentType(leftType))
							throw AstError{ "Left expression type is not compatible with right expression type" };

						break;
					}

					case BasicType::Mat4x4:
					{
						switch (rightType)
						{
							case BasicType::Float1:
							case BasicType::Float4:
							case BasicType::Mat4x4:
								break;

							default:
								TypeMustMatch(node.left, node.right);
						}

						break;
					}

					default:
						TypeMustMatch(node.left, node.right);
						break;
				}
			}
		}
	}

	void AstValidator::Visit(CastExpression& node)
	{
		RegisterScope(node);

		unsigned int componentCount = 0;
		unsigned int requiredComponents = GetComponentCount(node.targetType);
		for (auto& exprPtr : node.expressions)
		{
			if (!exprPtr)
				break;

			ShaderExpressionType exprType = GetExpressionType(*exprPtr, m_context->cache);
			if (!IsBasicType(exprType))
				throw AstError{ "incompatible type" };

			componentCount += GetComponentCount(std::get<BasicType>(exprType));
		}

		if (componentCount != requiredComponents)
			throw AstError{ "component count doesn't match required component count" };

		AstRecursiveVisitor::Visit(node);
	}

	void AstValidator::Visit(ConditionalExpression& node)
	{
		MandatoryExpr(node.truePath);
		MandatoryExpr(node.falsePath);

		RegisterScope(node);

		AstRecursiveVisitor::Visit(node);
		//if (m_shader.FindConditionByName(node.conditionName) == ShaderAst::InvalidCondition)
		//	throw AstError{ "condition not found" };
	}

	void AstValidator::Visit(ConstantExpression& node)
	{
		RegisterScope(node);
	}

	void AstValidator::Visit(IdentifierExpression& node)
	{
		assert(m_context);

		if (!m_context->activeScopeId)
			throw AstError{ "no scope" };

		RegisterScope(node);

		const AstCache::Identifier* identifier = m_context->cache->FindIdentifier(*m_context->activeScopeId, node.identifier);
		if (!identifier)
			throw AstError{ "Unknown variable " + node.identifier };
	}
	
	void AstValidator::Visit(IntrinsicExpression& node)
	{
		RegisterScope(node);

		switch (node.intrinsic)
		{
			case IntrinsicType::CrossProduct:
			case IntrinsicType::DotProduct:
			{
				if (node.parameters.size() != 2)
					throw AstError { "Expected 2 parameters" };

				for (auto& param : node.parameters)
					MandatoryExpr(param);

				ShaderExpressionType type = GetExpressionType(*node.parameters.front(), m_context->cache);
				for (std::size_t i = 1; i < node.parameters.size(); ++i)
				{
					if (type != GetExpressionType(MandatoryExpr(node.parameters[i])), m_context->cache)
						throw AstError{ "All type must match" };
				}

				break;
			}
		}

		switch (node.intrinsic)
		{
			case IntrinsicType::CrossProduct:
			{
				if (GetExpressionType(*node.parameters[0]) != ShaderExpressionType{ BasicType::Float3 }, m_context->cache)
					throw AstError{ "CrossProduct only works with Float3 expressions" };

				break;
			}

			case IntrinsicType::DotProduct:
				break;
		}

		AstRecursiveVisitor::Visit(node);
	}

	void AstValidator::Visit(SwizzleExpression& node)
	{
		RegisterScope(node);

		if (node.componentCount > 4)
			throw AstError{ "Cannot swizzle more than four elements" };

		ShaderExpressionType exprType = GetExpressionType(MandatoryExpr(node.expression), m_context->cache);
		if (!IsBasicType(exprType))
			throw AstError{ "Cannot swizzle this type" };

		switch (std::get<BasicType>(exprType))
		{
			case BasicType::Float1:
			case BasicType::Float2:
			case BasicType::Float3:
			case BasicType::Float4:
			case BasicType::Int1:
			case BasicType::Int2:
			case BasicType::Int3:
			case BasicType::Int4:
				break;

			default:
				throw AstError{ "Cannot swizzle this type" };
		}

		AstRecursiveVisitor::Visit(node);
	}

	void AstValidator::Visit(BranchStatement& node)
	{
		RegisterScope(node);

		for (auto& condStatement : node.condStatements)
		{
			ShaderExpressionType condType = GetExpressionType(MandatoryExpr(condStatement.condition), m_context->cache);
			if (!IsBasicType(condType) || std::get<BasicType>(condType) != BasicType::Boolean)
				throw AstError{ "if expression must resolve to boolean type" };

			MandatoryStatement(condStatement.statement);
		}

		AstRecursiveVisitor::Visit(node);
	}

	void AstValidator::Visit(ConditionalStatement& node)
	{
		MandatoryStatement(node.statement);

		RegisterScope(node);

		AstRecursiveVisitor::Visit(node);
		//if (m_shader.FindConditionByName(node.conditionName) == ShaderAst::InvalidCondition)
		//	throw AstError{ "condition not found" };
	}

	void AstValidator::Visit(DeclareFunctionStatement& node)
	{
		bool hasEntry = false;
		for (const auto& [attributeType, arg] : node.attributes)
		{
			switch (attributeType)
			{
				case AttributeType::Entry:
				{
					if (hasEntry)
						throw AstError{ "attribute entry must be present once" };

					if (arg.empty())
						throw AstError{ "attribute entry requires a parameter" };

					auto it = entryPoints.find(arg);
					if (it == entryPoints.end())
						throw AstError{ "invalid parameter " + arg + " for entry attribute" };

					ShaderStageType stageType = it->second;

					if (m_context->cache->entryFunctions[UnderlyingCast(stageType)])
						throw AstError{ "the same entry type has been defined multiple times" };

					m_context->cache->entryFunctions[UnderlyingCast(it->second)] = &node;

					hasEntry = true;
					break;
				}

				default:
					throw AstError{ "unhandled attribute for function" };
			}
		}

		auto& scope = EnterScope();
		RegisterScope(node);

		for (auto& parameter : node.parameters)
		{
			auto& identifier = scope.identifiers.emplace_back();
			identifier = AstCache::Identifier{ parameter.name, AstCache::Variable { parameter.type } };
		}

		for (auto& statement : node.statements)
			MandatoryStatement(statement).Visit(*this);

		ExitScope();
	}

	void AstValidator::Visit(DeclareStructStatement& node)
	{
		assert(m_context);

		if (!m_context->activeScopeId)
			throw AstError{ "cannot declare variable without scope" };

		RegisterScope(node);

		auto& scope = m_context->cache->scopes[*m_context->activeScopeId];

		auto& identifier = scope.identifiers.emplace_back();
		identifier = AstCache::Identifier{ node.description.name, node.description };

		AstRecursiveVisitor::Visit(node);
	}

	void AstValidator::Visit(DeclareVariableStatement& node)
	{
		assert(m_context);

		if (!m_context->activeScopeId)
			throw AstError{ "cannot declare variable without scope" };

		RegisterScope(node);

		auto& scope = m_context->cache->scopes[*m_context->activeScopeId];

		auto& identifier = scope.identifiers.emplace_back();
		identifier = AstCache::Identifier{ node.varName, AstCache::Variable { node.varType } };

		AstRecursiveVisitor::Visit(node);
	}

	void AstValidator::Visit(ExpressionStatement& node)
	{
		RegisterScope(node);

		MandatoryExpr(node.expression);

		AstRecursiveVisitor::Visit(node);
	}

	void AstValidator::Visit(MultiStatement& node)
	{
		assert(m_context);

		EnterScope();

		RegisterScope(node);

		for (auto& statement : node.statements)
			MandatoryStatement(statement);

		ExitScope();

		AstRecursiveVisitor::Visit(node);
	}

	void AstValidator::Visit(ReturnStatement& node)
	{
		RegisterScope(node);

		/*if (m_context->currentFunction->returnType != ShaderExpressionType(BasicType::Void))
		{
			if (GetExpressionType(MandatoryExpr(node.returnExpr)) != m_context->currentFunction->returnType)
				throw AstError{ "Return type doesn't match function return type" };
		}
		else
		{
			if (node.returnExpr)
				throw AstError{ "Unexpected expression for return (function doesn't return)" };
		}*/

		AstRecursiveVisitor::Visit(node);
	}

	bool ValidateAst(StatementPtr& node, std::string* error, AstCache* cache)
	{
		AstValidator validator;
		return validator.Validate(node, error, cache);
	}
}
