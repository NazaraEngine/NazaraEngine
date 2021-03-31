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
		std::unordered_set<std::string> declaredExternalVar;
		std::unordered_set<long long> usedBindingIndexes;;
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

	void AstValidator::TypeMustMatch(const ExpressionType& left, const ExpressionType& right)
	{
		if (left != right)
			throw AstError{ "Left expression type must match right expression type" };
	}

	ExpressionType AstValidator::CheckField(const std::string& structName, const std::string* memberIdentifier, std::size_t remainingMembers)
	{
		const AstCache::Identifier* identifier = m_context->cache->FindIdentifier(*m_context->activeScopeId, structName);
		if (!identifier)
			throw AstError{ "unknown identifier " + structName };

		if (!std::holds_alternative<StructDescription>(identifier->value))
			throw AstError{ "identifier is not a struct" };

		const StructDescription& s = std::get<StructDescription>(identifier->value);

		auto memberIt = std::find_if(s.members.begin(), s.members.end(), [&](const auto& field) { return field.name == memberIdentifier[0]; });
		if (memberIt == s.members.end())
			throw AstError{ "unknown field " + memberIdentifier[0]};

		const auto& member = *memberIt;

		if (remainingMembers > 1)
			return CheckField(std::get<IdentifierType>(member.type).name, memberIdentifier + 1, remainingMembers - 1);
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

	void AstValidator::RegisterExpressionType(Expression& node, ExpressionType expressionType)
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

		// Register expressions types
		AstRecursiveVisitor::Visit(node);

		ExpressionType exprType = GetExpressionType(MandatoryExpr(node.structExpr), m_context->cache);
		if (!IsIdentifierType(exprType))
			throw AstError{ "expression is not a structure" };

		const std::string& structName = std::get<IdentifierType>(exprType).name;

		RegisterExpressionType(node, CheckField(structName, node.memberIdentifiers.data(), node.memberIdentifiers.size()));
	}

	void AstValidator::Visit(AssignExpression& node)
	{
		RegisterScope(node);

		MandatoryExpr(node.left);
		MandatoryExpr(node.right);

		// Register expressions types
		AstRecursiveVisitor::Visit(node);

		TypeMustMatch(node.left, node.right);

		if (GetExpressionCategory(*node.left) != ExpressionCategory::LValue)
			throw AstError { "Assignation is only possible with a l-value" };
	}

	void AstValidator::Visit(BinaryExpression& node)
	{
		RegisterScope(node);

		// Register expression type
		AstRecursiveVisitor::Visit(node);

		ExpressionType leftExprType = GetExpressionType(MandatoryExpr(node.left), m_context->cache);
		if (!IsPrimitiveType(leftExprType) && !IsMatrixType(leftExprType) && !IsVectorType(leftExprType))
			throw AstError{ "left expression type does not support binary operation" };

		ExpressionType rightExprType = GetExpressionType(MandatoryExpr(node.right), m_context->cache);
		if (!IsPrimitiveType(rightExprType) && !IsMatrixType(rightExprType) && !IsVectorType(rightExprType))
			throw AstError{ "right expression type does not support binary operation" };

		if (IsPrimitiveType(leftExprType))
		{
			PrimitiveType leftType = std::get<PrimitiveType>(leftExprType);
			switch (node.op)
			{
				case BinaryType::CompGe:
				case BinaryType::CompGt:
				case BinaryType::CompLe:
				case BinaryType::CompLt:
					if (leftType == PrimitiveType::Boolean)
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
						case PrimitiveType::Float32:
						case PrimitiveType::Int32:
						case PrimitiveType::UInt32:
						{
							if (IsMatrixType(rightExprType))
								TypeMustMatch(leftType, std::get<MatrixType>(rightExprType).type);
							else if (IsVectorType(rightExprType))
								TypeMustMatch(leftType, std::get<VectorType>(rightExprType).type);
							else
								throw AstError{ "incompatible types" };

							break;
						}

						case PrimitiveType::Boolean:
							throw AstError{ "this operation is not supported for booleans" };

						default:
							throw AstError{ "incompatible types" };
					}
				}
			}
		}
		else if (IsMatrixType(leftExprType))
		{
			const MatrixType& leftType = std::get<MatrixType>(leftExprType);
			switch (node.op)
			{
				case BinaryType::CompGe:
				case BinaryType::CompGt:
				case BinaryType::CompLe:
				case BinaryType::CompLt:
				case BinaryType::CompEq:
				case BinaryType::CompNe:
				case BinaryType::Add:
				case BinaryType::Subtract:
					TypeMustMatch(node.left, node.right);
					break;

				case BinaryType::Multiply:
				case BinaryType::Divide:
				{
					if (IsMatrixType(rightExprType))
						TypeMustMatch(leftExprType, rightExprType);
					else if (IsPrimitiveType(rightExprType))
						TypeMustMatch(leftType.type, rightExprType);
					else if (IsVectorType(rightExprType))
					{
						const VectorType& rightType = std::get<VectorType>(rightExprType);
						TypeMustMatch(leftType.type, rightType.type);

						if (leftType.columnCount != rightType.componentCount)
							throw AstError{ "incompatible types" };
					}
					else
						throw AstError{ "incompatible types" };
				}
			}
		}
		else if (IsVectorType(leftExprType))
		{
			const MatrixType& leftType = std::get<MatrixType>(leftExprType);
			switch (node.op)
			{
				case BinaryType::CompGe:
				case BinaryType::CompGt:
				case BinaryType::CompLe:
				case BinaryType::CompLt:
				case BinaryType::CompEq:
				case BinaryType::CompNe:
				case BinaryType::Add:
				case BinaryType::Subtract:
					TypeMustMatch(node.left, node.right);
					break;

				case BinaryType::Multiply:
				case BinaryType::Divide:
				{
					if (IsPrimitiveType(rightExprType))
						TypeMustMatch(leftType.type, rightExprType);
					else
						throw AstError{ "incompatible types" };
				}
			}
		}
	}

	void AstValidator::Visit(CastExpression& node)
	{
		RegisterScope(node);

		AstRecursiveVisitor::Visit(node);

		auto GetComponentCount = [](const ExpressionType& exprType) -> unsigned int
		{
			if (IsPrimitiveType(exprType))
				return 1;
			else if (IsVectorType(exprType))
				return std::get<VectorType>(exprType).componentCount;
			else
				throw AstError{ "wut" };
		};

		unsigned int componentCount = 0;
		unsigned int requiredComponents = GetComponentCount(node.targetType);

		for (auto& exprPtr : node.expressions)
		{
			if (!exprPtr)
				break;

			ExpressionType exprType = GetExpressionType(*exprPtr, m_context->cache);
			if (!IsPrimitiveType(exprType) && !IsVectorType(exprType))
				throw AstError{ "incompatible type" };

			componentCount += GetComponentCount(exprType);
		}

		if (componentCount != requiredComponents)
			throw AstError{ "component count doesn't match required component count" };
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

		AstRecursiveVisitor::Visit(node);

		switch (node.intrinsic)
		{
			case IntrinsicType::CrossProduct:
			case IntrinsicType::DotProduct:
			{
				if (node.parameters.size() != 2)
					throw AstError { "Expected two parameters" };

				for (auto& param : node.parameters)
					MandatoryExpr(param);

				ExpressionType type = GetExpressionType(*node.parameters.front(), m_context->cache);
				for (std::size_t i = 1; i < node.parameters.size(); ++i)
				{
					if (type != GetExpressionType(MandatoryExpr(node.parameters[i]), m_context->cache))
						throw AstError{ "All type must match" };
				}

				break;
			}

			case IntrinsicType::SampleTexture:
			{
				if (node.parameters.size() != 2)
					throw AstError{ "Expected two parameters" };

				for (auto& param : node.parameters)
					MandatoryExpr(param);

				if (!IsSamplerType(GetExpressionType(*node.parameters[0], m_context->cache)))
					throw AstError{ "First parameter must be a sampler" };

				if (!IsVectorType(GetExpressionType(*node.parameters[1], m_context->cache)))
					throw AstError{ "First parameter must be a vector" };
			}
		}

		switch (node.intrinsic)
		{
			case IntrinsicType::CrossProduct:
			{
				if (GetExpressionType(*node.parameters[0]) != ExpressionType{ VectorType{ 3, PrimitiveType::Float32 } })
					throw AstError{ "CrossProduct only works with vec3<f32> expressions" };

				break;
			}

			case IntrinsicType::DotProduct:
				break;
		}
	}

	void AstValidator::Visit(SwizzleExpression& node)
	{
		RegisterScope(node);

		if (node.componentCount > 4)
			throw AstError{ "Cannot swizzle more than four elements" };

		ExpressionType exprType = GetExpressionType(MandatoryExpr(node.expression), m_context->cache);
		if (!IsPrimitiveType(exprType) && !IsVectorType(exprType))
			throw AstError{ "Cannot swizzle this type" };

		AstRecursiveVisitor::Visit(node);
	}

	void AstValidator::Visit(BranchStatement& node)
	{
		RegisterScope(node);

		for (auto& condStatement : node.condStatements)
		{
			ExpressionType condType = GetExpressionType(MandatoryExpr(condStatement.condition), m_context->cache);
			if (!IsPrimitiveType(condType) || std::get<PrimitiveType>(condType) != PrimitiveType::Boolean)
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

	void AstValidator::Visit(DeclareExternalStatement& node)
	{
		RegisterScope(node);
		auto& scope = m_context->cache->scopes[*m_context->activeScopeId];

		for (const auto& [attributeType, arg] : node.attributes)
		{
			switch (attributeType)
			{
				default:
					throw AstError{ "unhandled attribute for external block" };
			}
		}

		for (const auto& extVar : node.externalVars)
		{
			bool hasBinding = false;
			bool hasLayout = false;
			for (const auto& [attributeType, arg] : extVar.attributes)
			{
				switch (attributeType)
				{
					case AttributeType::Binding:
					{
						if (hasBinding)
							throw AstError{ "attribute binding must be present once" };

						if (!std::holds_alternative<long long>(arg))
							throw AstError{ "attribute binding requires a string parameter" };

						long long bindingIndex = std::get<long long>(arg);
						if (m_context->usedBindingIndexes.find(bindingIndex) != m_context->usedBindingIndexes.end())
							throw AstError{ "Binding #" + std::to_string(bindingIndex) + " is already in use" };

						m_context->usedBindingIndexes.insert(bindingIndex);
						break;
					}

					case AttributeType::Layout:
					{
						if (hasLayout)
							throw AstError{ "attribute layout must be present once" };

						if (!std::holds_alternative<std::string>(arg))
							throw AstError{ "attribute layout requires a string parameter" };

						if (std::get<std::string>(arg) != "std140")
							throw AstError{ "unknow layout type" };

						hasLayout = true;
						break;
					}

					default:
						throw AstError{ "unhandled attribute for external variable" };
				}
			}

			if (m_context->declaredExternalVar.find(extVar.name) != m_context->declaredExternalVar.end())
				throw AstError{ "External variable " + extVar.name + " is already declared" };

			m_context->declaredExternalVar.insert(extVar.name);

			ExpressionType subType = extVar.type;
			if (IsUniformType(subType))
				subType = IdentifierType{ std::get<UniformType>(subType).containedType };

			auto& identifier = scope.identifiers.emplace_back();
			identifier = AstCache::Identifier{ extVar.name, AstCache::Variable { std::move(subType) } };
		}
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

					if (!std::holds_alternative<std::string>(arg))
						throw AstError{ "attribute entry requires a string parameter" };

					const std::string& argStr = std::get<std::string>(arg);

					auto it = entryPoints.find(argStr);
					if (it == entryPoints.end())
						throw AstError{ "invalid parameter " + argStr + " for entry attribute" };

					ShaderStageType stageType = it->second;

					if (m_context->cache->entryFunctions[UnderlyingCast(stageType)])
						throw AstError{ "the same entry type has been defined multiple times" };

					m_context->cache->entryFunctions[UnderlyingCast(it->second)] = &node;

					if (node.parameters.size() > 1)
						throw AstError{ "entry functions can either take one struct parameter or no parameter" };

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

		//TODO: check members attributes

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
