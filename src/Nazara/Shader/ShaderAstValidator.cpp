// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderAstValidator.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Shader/ShaderAstUtils.hpp>
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
		std::array<DeclareFunctionStatement*, ShaderStageTypeCount> entryFunctions = {};
		std::unordered_set<std::string> declaredExternalVar;
		std::unordered_set<long long> usedBindingIndexes;
	};

	bool AstValidator::Validate(StatementPtr& node, std::string* error)
	{
		try
		{
			Context currentContext;

			m_context = &currentContext;
			CallOnExit resetContext([&] { m_context = nullptr; });

			ScopedVisit(node);
			return true;
		}
		catch (const AstError& e)
		{
			if (error)
				*error = e.errMsg;

			return false;
		}
	}

	const ExpressionType& AstValidator::GetExpressionType(Expression& expression)
	{
		assert(expression.cachedExpressionType);
		return ResolveAlias(expression.cachedExpressionType.value());
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
		return TypeMustMatch(GetExpressionType(*left), GetExpressionType(*right));
	}

	void AstValidator::TypeMustMatch(const ExpressionType& left, const ExpressionType& right)
	{
		if (left != right)
			throw AstError{ "Left expression type must match right expression type" };
	}

	ExpressionType AstValidator::CheckField(const std::string& structName, const std::string* memberIdentifier, std::size_t remainingMembers)
	{
		const Identifier* identifier = FindIdentifier(structName);
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

	const ExpressionType& AstValidator::ResolveAlias(const ExpressionType& expressionType)
	{
		if (!IsIdentifierType(expressionType))
			return expressionType;

		const Identifier* identifier = FindIdentifier(std::get<IdentifierType>(expressionType).name);
		if (identifier && std::holds_alternative<Alias>(identifier->value))
		{
			const Alias& alias = std::get<Alias>(identifier->value);
			return std::visit([&](auto&& arg) -> const ShaderAst::ExpressionType&
			{
				using T = std::decay_t<decltype(arg)>;

				if constexpr (std::is_same_v<T, ExpressionType>)
					return arg;
				else
					static_assert(AlwaysFalse<T>::value, "non-exhaustive visitor");
			}, alias.value);
		}

		return expressionType;
	}

	void AstValidator::Visit(AccessMemberExpression& node)
	{
		// Register expressions types
		AstScopedVisitor::Visit(node);

		ExpressionType exprType = GetExpressionType(MandatoryExpr(node.structExpr));
		if (!IsIdentifierType(exprType))
			throw AstError{ "expression is not a structure" };

		const std::string& structName = std::get<IdentifierType>(exprType).name;

		node.cachedExpressionType = CheckField(structName, node.memberIdentifiers.data(), node.memberIdentifiers.size());
	}

	void AstValidator::Visit(AssignExpression& node)
	{
		MandatoryExpr(node.left);
		MandatoryExpr(node.right);

		// Register expressions types
		AstScopedVisitor::Visit(node);

		TypeMustMatch(node.left, node.right);

		if (GetExpressionCategory(*node.left) != ExpressionCategory::LValue)
			throw AstError { "Assignation is only possible with a l-value" };

		node.cachedExpressionType = GetExpressionType(*node.right);
	}

	void AstValidator::Visit(BinaryExpression& node)
	{
		// Register expression type
		AstScopedVisitor::Visit(node);

		ExpressionType leftExprType = GetExpressionType(MandatoryExpr(node.left));
		if (!IsPrimitiveType(leftExprType) && !IsMatrixType(leftExprType) && !IsVectorType(leftExprType))
			throw AstError{ "left expression type does not support binary operation" };

		ExpressionType rightExprType = GetExpressionType(MandatoryExpr(node.right));
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

					TypeMustMatch(node.left, node.right);

					node.cachedExpressionType = PrimitiveType::Boolean;
					break;

				case BinaryType::Add:
				case BinaryType::CompEq:
				case BinaryType::CompNe:
				case BinaryType::Subtract:
					TypeMustMatch(node.left, node.right);

					node.cachedExpressionType = leftExprType;
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
							{
								TypeMustMatch(leftType, std::get<MatrixType>(rightExprType).type);
								node.cachedExpressionType = rightExprType;
							}
							else if (IsPrimitiveType(rightExprType))
							{
								TypeMustMatch(leftType, rightExprType);
								node.cachedExpressionType = leftExprType;
							}
							else if (IsVectorType(rightExprType))
							{
								TypeMustMatch(leftType, std::get<VectorType>(rightExprType).type);
								node.cachedExpressionType = rightExprType;
							}
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
					TypeMustMatch(node.left, node.right);
					node.cachedExpressionType = PrimitiveType::Boolean;
					break;

				case BinaryType::Add:
				case BinaryType::Subtract:
					TypeMustMatch(node.left, node.right);
					node.cachedExpressionType = leftExprType;
					break;

				case BinaryType::Multiply:
				case BinaryType::Divide:
				{
					if (IsMatrixType(rightExprType))
					{
						TypeMustMatch(leftExprType, rightExprType);
						node.cachedExpressionType = leftExprType; //< FIXME
					}
					else if (IsPrimitiveType(rightExprType))
					{
						TypeMustMatch(leftType.type, rightExprType);
						node.cachedExpressionType = leftExprType;
					}
					else if (IsVectorType(rightExprType))
					{
						const VectorType& rightType = std::get<VectorType>(rightExprType);
						TypeMustMatch(leftType.type, rightType.type);

						if (leftType.columnCount != rightType.componentCount)
							throw AstError{ "incompatible types" };

						node.cachedExpressionType = rightExprType;
					}
					else
						throw AstError{ "incompatible types" };
				}
			}
		}
		else if (IsVectorType(leftExprType))
		{
			const VectorType& leftType = std::get<VectorType>(leftExprType);
			switch (node.op)
			{
				case BinaryType::CompGe:
				case BinaryType::CompGt:
				case BinaryType::CompLe:
				case BinaryType::CompLt:
				case BinaryType::CompEq:
				case BinaryType::CompNe:
					TypeMustMatch(node.left, node.right);
					node.cachedExpressionType = PrimitiveType::Boolean;
					break;

				case BinaryType::Add:
				case BinaryType::Subtract:
					TypeMustMatch(node.left, node.right);
					node.cachedExpressionType = leftExprType;
					break;

				case BinaryType::Multiply:
				case BinaryType::Divide:
				{
					if (IsPrimitiveType(rightExprType))
					{
						TypeMustMatch(leftType.type, rightExprType);
						node.cachedExpressionType = rightExprType;
					}
					else if (IsVectorType(rightExprType))
					{
						TypeMustMatch(leftType, rightExprType);
						node.cachedExpressionType = rightExprType;
					}
					else
						throw AstError{ "incompatible types" };
				}
			}
		}
	}

	void AstValidator::Visit(CastExpression& node)
	{
		AstScopedVisitor::Visit(node);

		auto GetComponentCount = [](const ExpressionType& exprType) -> std::size_t
		{
			if (IsPrimitiveType(exprType))
				return 1;
			else if (IsVectorType(exprType))
				return std::get<VectorType>(exprType).componentCount;
			else
				throw AstError{ "wut" };
		};

		std::size_t componentCount = 0;
		std::size_t requiredComponents = GetComponentCount(node.targetType);

		for (auto& exprPtr : node.expressions)
		{
			if (!exprPtr)
				break;

			ExpressionType exprType = GetExpressionType(*exprPtr);
			if (!IsPrimitiveType(exprType) && !IsVectorType(exprType))
				throw AstError{ "incompatible type" };

			componentCount += GetComponentCount(exprType);
		}

		if (componentCount != requiredComponents)
			throw AstError{ "component count doesn't match required component count" };

		node.cachedExpressionType = node.targetType;
	}

	void AstValidator::Visit(ConstantExpression& node)
	{
		node.cachedExpressionType = std::visit([&](auto&& arg) -> ShaderAst::ExpressionType
		{
			using T = std::decay_t<decltype(arg)>;

			if constexpr (std::is_same_v<T, bool>)
				return PrimitiveType::Boolean;
			else if constexpr (std::is_same_v<T, float>)
				return PrimitiveType::Float32;
			else if constexpr (std::is_same_v<T, Int32>)
				return PrimitiveType::Int32;
			else if constexpr (std::is_same_v<T, UInt32>)
				return PrimitiveType::UInt32;
			else if constexpr (std::is_same_v<T, Vector2f>)
				return VectorType{ 2, PrimitiveType::Float32 };
			else if constexpr (std::is_same_v<T, Vector3f>)
				return VectorType{ 3, PrimitiveType::Float32 };
			else if constexpr (std::is_same_v<T, Vector4f>)
				return VectorType{ 4, PrimitiveType::Float32 };
			else if constexpr (std::is_same_v<T, Vector2i32>)
				return VectorType{ 2, PrimitiveType::Int32 };
			else if constexpr (std::is_same_v<T, Vector3i32>)
				return VectorType{ 3, PrimitiveType::Int32 };
			else if constexpr (std::is_same_v<T, Vector4i32>)
				return VectorType{ 4, PrimitiveType::Int32 };
			else
				static_assert(AlwaysFalse<T>::value, "non-exhaustive visitor");
		}, node.value);

	}

	void AstValidator::Visit(ConditionalExpression& node)
	{
		MandatoryExpr(node.truePath);
		MandatoryExpr(node.falsePath);

		AstScopedVisitor::Visit(node);

		ExpressionType leftExprType = GetExpressionType(*node.truePath);
		if (leftExprType != GetExpressionType(*node.falsePath))
			throw AstError{ "true path type must match false path type" };

		node.cachedExpressionType = leftExprType;
		//if (m_shader.FindConditionByName(node.conditionName) == ShaderAst::InvalidCondition)
		//	throw AstError{ "condition not found" };
	}

	void AstValidator::Visit(IdentifierExpression& node)
	{
		assert(m_context);

		const Identifier* identifier = FindIdentifier(node.identifier);
		if (!identifier)
			throw AstError{ "Unknown identifier " + node.identifier };

		node.cachedExpressionType = ResolveAlias(std::get<Variable>(identifier->value).type);
	}
	
	void AstValidator::Visit(IntrinsicExpression& node)
	{
		AstScopedVisitor::Visit(node);

		switch (node.intrinsic)
		{
			case IntrinsicType::CrossProduct:
			case IntrinsicType::DotProduct:
			{
				if (node.parameters.size() != 2)
					throw AstError { "Expected two parameters" };

				for (auto& param : node.parameters)
					MandatoryExpr(param);

				ExpressionType type = GetExpressionType(*node.parameters.front());

				for (std::size_t i = 1; i < node.parameters.size(); ++i)
				{
					if (type != GetExpressionType(MandatoryExpr(node.parameters[i])))
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

				if (!IsSamplerType(GetExpressionType(*node.parameters[0])))
					throw AstError{ "First parameter must be a sampler" };

				if (!IsVectorType(GetExpressionType(*node.parameters[1])))
					throw AstError{ "Second parameter must be a vector" };
			}
		}

		switch (node.intrinsic)
		{
			case IntrinsicType::CrossProduct:
			{
				ExpressionType type = GetExpressionType(*node.parameters.front());
				if (type != ExpressionType{ VectorType{ 3, PrimitiveType::Float32 } })
					throw AstError{ "CrossProduct only works with vec3<f32> expressions" };

				node.cachedExpressionType = std::move(type);
				break;
			}

			case IntrinsicType::DotProduct:
			{
				ExpressionType type = GetExpressionType(*node.parameters.front());
				if (!IsVectorType(type))
					throw AstError{ "DotProduct expects vector types" };

				node.cachedExpressionType = std::get<VectorType>(type).type;
				break;
			}

			case IntrinsicType::SampleTexture:
			{
				node.cachedExpressionType = VectorType{ 4, std::get<SamplerType>(GetExpressionType(*node.parameters.front())).sampledType };
				break;
			}
		}
	}

	void AstValidator::Visit(SwizzleExpression& node)
	{
		if (node.componentCount > 4)
			throw AstError{ "Cannot swizzle more than four elements" };

		MandatoryExpr(node.expression);

		AstScopedVisitor::Visit(node);

		ExpressionType exprType = GetExpressionType(*node.expression);
		if (IsPrimitiveType(exprType) || IsVectorType(exprType))
		{
			PrimitiveType baseType;
			if (IsPrimitiveType(exprType))
				baseType = std::get<PrimitiveType>(exprType);
			else
				baseType = std::get<VectorType>(exprType).type;

			if (node.componentCount > 1)
			{
				node.cachedExpressionType = VectorType{
					node.componentCount,
					baseType
				};
			}
			else
				node.cachedExpressionType = baseType;
		}
		else
			throw AstError{ "Cannot swizzle this type" };
	}

	void AstValidator::Visit(BranchStatement& node)
	{
		for (auto& condStatement : node.condStatements)
		{
			ExpressionType condType = GetExpressionType(MandatoryExpr(condStatement.condition));
			if (!IsPrimitiveType(condType) || std::get<PrimitiveType>(condType) != PrimitiveType::Boolean)
				throw AstError{ "if expression must resolve to boolean type" };

			MandatoryStatement(condStatement.statement);
		}

		AstScopedVisitor::Visit(node);
	}

	void AstValidator::Visit(ConditionalStatement& node)
	{
		MandatoryStatement(node.statement);

		AstScopedVisitor::Visit(node);
		//if (m_shader.FindConditionByName(node.conditionName) == ShaderAst::InvalidCondition)
		//	throw AstError{ "condition not found" };
	}

	void AstValidator::Visit(DeclareExternalStatement& node)
	{
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
							throw AstError{ "unknown layout type" };

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
		}

		AstScopedVisitor::Visit(node);
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

					if (m_context->entryFunctions[UnderlyingCast(stageType)])
						throw AstError{ "the same entry type has been defined multiple times" };

					m_context->entryFunctions[UnderlyingCast(it->second)] = &node;

					if (node.parameters.size() > 1)
						throw AstError{ "entry functions can either take one struct parameter or no parameter" };

					hasEntry = true;
					break;
				}

				default:
					throw AstError{ "unhandled attribute for function" };
			}
		}

		for (auto& statement : node.statements)
			MandatoryStatement(statement);

		AstScopedVisitor::Visit(node);
	}

	void AstValidator::Visit(DeclareStructStatement& node)
	{
		assert(m_context);

		//TODO: check members attributes

		AstScopedVisitor::Visit(node);
	}

	void AstValidator::Visit(ExpressionStatement& node)
	{
		MandatoryExpr(node.expression);

		AstScopedVisitor::Visit(node);
	}

	void AstValidator::Visit(MultiStatement& node)
	{
		assert(m_context);

		for (auto& statement : node.statements)
			MandatoryStatement(statement);

		AstScopedVisitor::Visit(node);
	}

	bool ValidateAst(StatementPtr& node, std::string* error)
	{
		AstValidator validator;
		return validator.Validate(node, error);
	}
}
