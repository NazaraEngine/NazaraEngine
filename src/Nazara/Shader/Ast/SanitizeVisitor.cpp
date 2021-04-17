// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/Ast/SanitizeVisitor.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/StackArray.hpp>
#include <Nazara/Shader/ShaderBuilder.hpp>
#include <Nazara/Shader/Ast/AstUtils.hpp>
#include <stdexcept>
#include <unordered_set>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	namespace
	{
		struct AstError
		{
			std::string errMsg;
		};

		template<typename T, typename U>
		std::unique_ptr<T> static_unique_pointer_cast(std::unique_ptr<U>&& ptr)
		{
			return std::unique_ptr<T>(static_cast<T*>(ptr.release()));
		}
	}

	struct SanitizeVisitor::Context
	{
		Options options;
		std::array<DeclareFunctionStatement*, ShaderStageTypeCount> entryFunctions = {};
		std::unordered_set<std::string> declaredExternalVar;
		std::unordered_set<unsigned int> usedBindingIndexes;
	};

	StatementPtr SanitizeVisitor::Sanitize(StatementPtr& nodePtr, const Options& options, std::string* error)
	{
		StatementPtr clone;

		Context currentContext;
		currentContext.options = options;

		m_context = &currentContext;
		CallOnExit resetContext([&] { m_context = nullptr; });

		PushScope(); //< Global scope
		{
			try
			{
				clone = AstCloner::Clone(nodePtr);
			}
			catch (const AstError& err)
			{
				if (!error)
					throw std::runtime_error(err.errMsg);

				*error = err.errMsg;
			}
		}
		PopScope();

		return clone;
	}

	const ExpressionType& SanitizeVisitor::CheckField(const ExpressionType& structType, const std::string* memberIdentifier, std::size_t remainingMembers, std::size_t* structIndices)
	{
		std::size_t structIndex = ResolveStruct(structType);

		*structIndices++ = structIndex;

		assert(structIndex < m_structs.size());
		const StructDescription& s = m_structs[structIndex];

		auto memberIt = std::find_if(s.members.begin(), s.members.end(), [&](const auto& field) { return field.name == memberIdentifier[0]; });
		if (memberIt == s.members.end())
			throw AstError{ "unknown field " + memberIdentifier[0] };

		const auto& member = *memberIt;

		if (remainingMembers > 1)
			return CheckField(member.type, memberIdentifier + 1, remainingMembers - 1, structIndices);
		else
			return member.type;
	}

	ExpressionPtr SanitizeVisitor::Clone(AccessMemberIdentifierExpression& node)
	{
		auto structExpr = CloneExpression(MandatoryExpr(node.structExpr));

		const ExpressionType& exprType = GetExpressionType(*structExpr);

		// Transform to AccessMemberIndexExpression
		auto accessMemberIndex = std::make_unique<AccessMemberIndexExpression>();
		accessMemberIndex->structExpr = std::move(structExpr);

		StackArray<std::size_t> structIndices = NazaraStackArrayNoInit(std::size_t, node.memberIdentifiers.size());

		accessMemberIndex->cachedExpressionType = ResolveType(CheckField(exprType, node.memberIdentifiers.data(), node.memberIdentifiers.size(), structIndices.data()));

		accessMemberIndex->memberIndices.resize(node.memberIdentifiers.size());
		for (std::size_t i = 0; i < node.memberIdentifiers.size(); ++i)
		{
			std::size_t structIndex = structIndices[i];
			assert(structIndex < m_structs.size());
			const StructDescription& structDesc = m_structs[structIndex];

			auto it = std::find_if(structDesc.members.begin(), structDesc.members.end(), [&](const auto& member) { return member.name == node.memberIdentifiers[i]; });
			assert(it != structDesc.members.end());

			accessMemberIndex->memberIndices[i] = std::distance(structDesc.members.begin(), it);
		}

		return accessMemberIndex;
	}

	ExpressionPtr SanitizeVisitor::Clone(AssignExpression& node)
	{
		MandatoryExpr(node.left);
		MandatoryExpr(node.right);

		if (GetExpressionCategory(*node.left) != ExpressionCategory::LValue)
			throw AstError{ "Assignation is only possible with a l-value" };

		auto clone = static_unique_pointer_cast<AssignExpression>(AstCloner::Clone(node));

		TypeMustMatch(clone->left, clone->right);
		clone->cachedExpressionType = GetExpressionType(*clone->right);

		return clone;
	}

	ExpressionPtr SanitizeVisitor::Clone(BinaryExpression& node)
	{
		auto clone = static_unique_pointer_cast<BinaryExpression>(AstCloner::Clone(node));

		const ExpressionType& leftExprType = GetExpressionType(MandatoryExpr(clone->left));
		if (!IsPrimitiveType(leftExprType) && !IsMatrixType(leftExprType) && !IsVectorType(leftExprType))
			throw AstError{ "left expression type does not support binary operation" };

		const ExpressionType& rightExprType = GetExpressionType(MandatoryExpr(clone->right));
		if (!IsPrimitiveType(rightExprType) && !IsMatrixType(rightExprType) && !IsVectorType(rightExprType))
			throw AstError{ "right expression type does not support binary operation" };

		if (IsPrimitiveType(leftExprType))
		{
			PrimitiveType leftType = std::get<PrimitiveType>(leftExprType);
			switch (clone->op)
			{
				case BinaryType::CompGe:
				case BinaryType::CompGt:
				case BinaryType::CompLe:
				case BinaryType::CompLt:
					if (leftType == PrimitiveType::Boolean)
						throw AstError{ "this operation is not supported for booleans" };

					TypeMustMatch(clone->left, clone->right);

					clone->cachedExpressionType = PrimitiveType::Boolean;
					break;

				case BinaryType::Add:
				case BinaryType::CompEq:
				case BinaryType::CompNe:
				case BinaryType::Subtract:
					TypeMustMatch(clone->left, clone->right);

					clone->cachedExpressionType = leftExprType;
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
								clone->cachedExpressionType = rightExprType;
							}
							else if (IsPrimitiveType(rightExprType))
							{
								TypeMustMatch(leftType, rightExprType);
								clone->cachedExpressionType = leftExprType;
							}
							else if (IsVectorType(rightExprType))
							{
								TypeMustMatch(leftType, std::get<VectorType>(rightExprType).type);
								clone->cachedExpressionType = rightExprType;
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
			switch (clone->op)
			{
				case BinaryType::CompGe:
				case BinaryType::CompGt:
				case BinaryType::CompLe:
				case BinaryType::CompLt:
				case BinaryType::CompEq:
				case BinaryType::CompNe:
					TypeMustMatch(clone->left, clone->right);
					clone->cachedExpressionType = PrimitiveType::Boolean;
					break;

				case BinaryType::Add:
				case BinaryType::Subtract:
					TypeMustMatch(clone->left, clone->right);
					clone->cachedExpressionType = leftExprType;
					break;

				case BinaryType::Multiply:
				case BinaryType::Divide:
				{
					if (IsMatrixType(rightExprType))
					{
						TypeMustMatch(leftExprType, rightExprType);
						clone->cachedExpressionType = leftExprType; //< FIXME
					}
					else if (IsPrimitiveType(rightExprType))
					{
						TypeMustMatch(leftType.type, rightExprType);
						clone->cachedExpressionType = leftExprType;
					}
					else if (IsVectorType(rightExprType))
					{
						const VectorType& rightType = std::get<VectorType>(rightExprType);
						TypeMustMatch(leftType.type, rightType.type);

						if (leftType.columnCount != rightType.componentCount)
							throw AstError{ "incompatible types" };

						clone->cachedExpressionType = rightExprType;
					}
					else
						throw AstError{ "incompatible types" };
				}
			}
		}
		else if (IsVectorType(leftExprType))
		{
			const VectorType& leftType = std::get<VectorType>(leftExprType);
			switch (clone->op)
			{
				case BinaryType::CompGe:
				case BinaryType::CompGt:
				case BinaryType::CompLe:
				case BinaryType::CompLt:
				case BinaryType::CompEq:
				case BinaryType::CompNe:
					TypeMustMatch(clone->left, clone->right);
					clone->cachedExpressionType = PrimitiveType::Boolean;
					break;

				case BinaryType::Add:
				case BinaryType::Subtract:
					TypeMustMatch(clone->left, clone->right);
					clone->cachedExpressionType = leftExprType;
					break;

				case BinaryType::Multiply:
				case BinaryType::Divide:
				{
					if (IsPrimitiveType(rightExprType))
					{
						TypeMustMatch(leftType.type, rightExprType);
						clone->cachedExpressionType = rightExprType;
					}
					else if (IsVectorType(rightExprType))
					{
						TypeMustMatch(leftType, rightExprType);
						clone->cachedExpressionType = rightExprType;
					}
					else
						throw AstError{ "incompatible types" };
				}
			}
		}

		return clone;
	}

	ExpressionPtr SanitizeVisitor::Clone(CastExpression& node)
	{
		auto clone = static_unique_pointer_cast<CastExpression>(AstCloner::Clone(node));

		auto GetComponentCount = [](const ExpressionType& exprType) -> std::size_t
		{
			if (IsVectorType(exprType))
				return std::get<VectorType>(exprType).componentCount;
			else
			{
				assert(IsPrimitiveType(exprType));
				return 1;
			}
		};

		std::size_t componentCount = 0;
		std::size_t requiredComponents = GetComponentCount(clone->targetType);

		for (auto& exprPtr : clone->expressions)
		{
			if (!exprPtr)
				break;

			const ExpressionType& exprType = GetExpressionType(*exprPtr);
			if (!IsPrimitiveType(exprType) && !IsVectorType(exprType))
				throw AstError{ "incompatible type" };

			componentCount += GetComponentCount(exprType);
		}

		if (componentCount != requiredComponents)
			throw AstError{ "component count doesn't match required component count" };

		clone->targetType = ResolveType(clone->targetType);
		clone->cachedExpressionType = clone->targetType;

		return clone;
	}

	ExpressionPtr SanitizeVisitor::Clone(ConditionalExpression& node)
	{
		MandatoryExpr(node.truePath);
		MandatoryExpr(node.falsePath);

		auto clone = static_unique_pointer_cast<ConditionalExpression>(AstCloner::Clone(node));

		const ExpressionType& leftExprType = GetExpressionType(*clone->truePath);
		if (leftExprType != GetExpressionType(*clone->falsePath))
			throw AstError{ "true path type must match false path type" };

		clone->cachedExpressionType = leftExprType;

		return clone;
	}

	ExpressionPtr SanitizeVisitor::Clone(ConstantExpression& node)
	{
		auto clone = static_unique_pointer_cast<ConstantExpression>(AstCloner::Clone(node));
		clone->cachedExpressionType = clone->GetExpressionType();

		return clone;
	}

	ExpressionPtr SanitizeVisitor::Clone(IdentifierExpression& node)
	{
		assert(m_context);

		const Identifier* identifier = FindIdentifier(node.identifier);
		if (!identifier)
			throw AstError{ "unknown identifier " + node.identifier };

		if (!std::holds_alternative<Variable>(identifier->value))
			throw AstError{ "expected variable identifier" };

		const Variable& variable = std::get<Variable>(identifier->value);

		// Replace IdentifierExpression by VariableExpression
		auto varExpr = std::make_unique<VariableExpression>();
		varExpr->cachedExpressionType = m_variables[variable.varIndex];
		varExpr->variableId = variable.varIndex;

		return varExpr;
	}

	ExpressionPtr SanitizeVisitor::Clone(IntrinsicExpression& node)
	{
		auto clone = static_unique_pointer_cast<IntrinsicExpression>(AstCloner::Clone(node));

		// Parameter validation
		switch (clone->intrinsic)
		{
			case IntrinsicType::CrossProduct:
			case IntrinsicType::DotProduct:
			{
				if (clone->parameters.size() != 2)
					throw AstError { "Expected two parameters" };

				for (auto& param : clone->parameters)
					MandatoryExpr(param);

				const ExpressionType& type = GetExpressionType(*clone->parameters.front());

				for (std::size_t i = 1; i < clone->parameters.size(); ++i)
				{
					if (type != GetExpressionType(*clone->parameters[i]))
						throw AstError{ "All type must match" };
				}

				break;
			}

			case IntrinsicType::SampleTexture:
			{
				if (clone->parameters.size() != 2)
					throw AstError{ "Expected two parameters" };

				for (auto& param : clone->parameters)
					MandatoryExpr(param);

				if (!IsSamplerType(GetExpressionType(*clone->parameters[0])))
					throw AstError{ "First parameter must be a sampler" };

				if (!IsVectorType(GetExpressionType(*clone->parameters[1])))
					throw AstError{ "Second parameter must be a vector" };
			}
		}

		// Return type attribution
		switch (clone->intrinsic)
		{
			case IntrinsicType::CrossProduct:
			{
				const ExpressionType& type = GetExpressionType(*clone->parameters.front());
				if (type != ExpressionType{ VectorType{ 3, PrimitiveType::Float32 } })
					throw AstError{ "CrossProduct only works with vec3<f32> expressions" };

				clone->cachedExpressionType = type;
				break;
			}

			case IntrinsicType::DotProduct:
			{
				ExpressionType type = GetExpressionType(*clone->parameters.front());
				if (!IsVectorType(type))
					throw AstError{ "DotProduct expects vector types" };

				clone->cachedExpressionType = std::get<VectorType>(type).type;
				break;
			}

			case IntrinsicType::SampleTexture:
			{
				clone->cachedExpressionType = VectorType{ 4, std::get<SamplerType>(GetExpressionType(*clone->parameters.front())).sampledType };
				break;
			}
		}

		return clone;
	}

	ExpressionPtr SanitizeVisitor::Clone(SelectOptionExpression& node)
	{
		MandatoryExpr(node.truePath);
		MandatoryExpr(node.falsePath);

		auto condExpr = std::make_unique<ConditionalExpression>();
		condExpr->truePath = CloneExpression(node.truePath);
		condExpr->falsePath = CloneExpression(node.falsePath);

		const Identifier* identifier = FindIdentifier(node.optionName);
		if (!identifier)
			throw AstError{ "unknown option " + node.optionName };

		if (!std::holds_alternative<Option>(identifier->value))
			throw AstError{ "expected option identifier" };

		condExpr->optionIndex = std::get<Option>(identifier->value).optionIndex;

		const ExpressionType& leftExprType = GetExpressionType(*condExpr->truePath);
		if (leftExprType != GetExpressionType(*condExpr->falsePath))
			throw AstError{ "true path type must match false path type" };

		condExpr->cachedExpressionType = leftExprType;

		return condExpr;
	}

	ExpressionPtr SanitizeVisitor::Clone(SwizzleExpression& node)
	{
		if (node.componentCount > 4)
			throw AstError{ "Cannot swizzle more than four elements" };

		MandatoryExpr(node.expression);

		auto clone = static_unique_pointer_cast<SwizzleExpression>(AstCloner::Clone(node));

		const ExpressionType& exprType = GetExpressionType(*clone->expression);
		if (!IsPrimitiveType(exprType) && !IsVectorType(exprType))
			throw AstError{ "Cannot swizzle this type" };

		PrimitiveType baseType;
		if (IsPrimitiveType(exprType))
			baseType = std::get<PrimitiveType>(exprType);
		else
			baseType = std::get<VectorType>(exprType).type;

		if (clone->componentCount > 1)
		{
			clone->cachedExpressionType = VectorType{
				clone->componentCount,
				baseType
			};
		}
		else
			clone->cachedExpressionType = baseType;

		return clone;
	}

	StatementPtr SanitizeVisitor::Clone(BranchStatement& node)
	{
		auto clone = std::make_unique<BranchStatement>();
		clone->condStatements.reserve(node.condStatements.size());

		for (auto& cond : node.condStatements)
		{
			PushScope();

			auto& condStatement = clone->condStatements.emplace_back();
			condStatement.condition = CloneExpression(MandatoryExpr(cond.condition));

			const ExpressionType& condType = GetExpressionType(*condStatement.condition);
			if (!IsPrimitiveType(condType) || std::get<PrimitiveType>(condType) != PrimitiveType::Boolean)
				throw AstError{ "branch expressions must resolve to boolean type" };

			condStatement.statement = CloneStatement(MandatoryStatement(cond.statement));

			PopScope();
		}

		if (node.elseStatement)
		{
			PushScope();
			clone->elseStatement = CloneStatement(node.elseStatement);
			PopScope();
		}

		return clone;
	}

	StatementPtr SanitizeVisitor::Clone(ConditionalStatement& node)
	{
		MandatoryStatement(node.statement);

		PushScope();
		
		auto clone = static_unique_pointer_cast<ConditionalStatement>(AstCloner::Clone(node));

		PopScope();

		return clone;
	}

	StatementPtr SanitizeVisitor::Clone(DeclareExternalStatement& node)
	{
		assert(m_context);

		for (const auto& extVar : node.externalVars)
		{
			if (extVar.bindingIndex)
			{
				unsigned int bindingIndex = extVar.bindingIndex.value();
				if (m_context->usedBindingIndexes.find(bindingIndex) != m_context->usedBindingIndexes.end())
					throw AstError{ "Binding #" + std::to_string(bindingIndex) + " is already in use" };

				m_context->usedBindingIndexes.insert(bindingIndex);
			}

			if (m_context->declaredExternalVar.find(extVar.name) != m_context->declaredExternalVar.end())
				throw AstError{ "External variable " + extVar.name + " is already declared" };

			m_context->declaredExternalVar.insert(extVar.name);
		}

		auto clone = static_unique_pointer_cast<DeclareExternalStatement>(AstCloner::Clone(node));
		for (auto& extVar : clone->externalVars)
		{
			extVar.type = ResolveType(extVar.type);

			ExpressionType varType;
			if (IsUniformType(extVar.type))
				varType = std::get<StructType>(std::get<UniformType>(extVar.type).containedType);
			else if (IsSamplerType(extVar.type))
				varType = extVar.type;
			else
				throw AstError{ "External variable " + extVar.name + " is of wrong type: only uniform and sampler are allowed in external blocks" };

			std::size_t varIndex = RegisterVariable(extVar.name, std::move(varType));
			if (!clone->varIndex)
				clone->varIndex = varIndex; //< First external variable index is node variable index
		}

		return clone;
	}

	StatementPtr SanitizeVisitor::Clone(DeclareFunctionStatement& node)
	{
		if (node.entryStage)
		{
			ShaderStageType stageType = *node.entryStage;

			if (m_context->entryFunctions[UnderlyingCast(stageType)])
				throw AstError{ "the same entry type has been defined multiple times" };

			m_context->entryFunctions[UnderlyingCast(stageType)] = &node;

			if (node.parameters.size() > 1)
				throw AstError{ "entry functions can either take one struct parameter or no parameter" };
		}

		auto clone = std::make_unique<DeclareFunctionStatement>();
		clone->entryStage = node.entryStage;
		clone->name = node.name;
		clone->funcIndex = m_nextFuncIndex++;
		clone->optionName = node.optionName;
		clone->parameters = node.parameters;
		clone->returnType = ResolveType(node.returnType);

		PushScope();
		{
			for (auto& parameter : clone->parameters)
			{
				parameter.type = ResolveType(parameter.type);
				std::size_t varIndex = RegisterVariable(parameter.name, parameter.type);
				if (!clone->varIndex)
					clone->varIndex = varIndex; //< First parameter variable index is node variable index
			}

			clone->statements.reserve(node.statements.size());
			for (auto& statement : node.statements)
				clone->statements.push_back(CloneStatement(MandatoryStatement(statement)));
		}
		PopScope();

		if (!clone->optionName.empty())
		{
			const Identifier* identifier = FindIdentifier(node.optionName);
			if (!identifier)
				throw AstError{ "unknown option " + node.optionName };

			if (!std::holds_alternative<Option>(identifier->value))
				throw AstError{ "expected option identifier" };

			std::size_t optionIndex = std::get<Option>(identifier->value).optionIndex;

			return ShaderBuilder::ConditionalStatement(optionIndex, std::move(clone));
		}

		return clone;
	}

	StatementPtr SanitizeVisitor::Clone(DeclareOptionStatement& node)
	{
		auto clone = static_unique_pointer_cast<DeclareOptionStatement>(AstCloner::Clone(node));
		clone->optType = ResolveType(clone->optType);

		if (clone->initialValue && clone->optType != GetExpressionType(*clone->initialValue))
			throw AstError{ "option " + clone->optName + " initial expression must be of the same type than the option" };

		clone->optIndex = RegisterOption(clone->optName, clone->optType);

		if (m_context->options.removeOptionDeclaration)
			return ShaderBuilder::NoOp();

		return clone;
	}

	StatementPtr SanitizeVisitor::Clone(DeclareStructStatement& node)
	{
		std::unordered_set<std::string> declaredMembers;

		for (auto& member : node.description.members)
		{
			if (declaredMembers.find(member.name) != declaredMembers.end())
				throw AstError{ "struct member " + member.name + " found multiple time" };

			declaredMembers.insert(member.name);
		}

		auto clone = static_unique_pointer_cast<DeclareStructStatement>(AstCloner::Clone(node));

		for (auto& member : clone->description.members)
			member.type = ResolveType(member.type);

		clone->structIndex = RegisterStruct(clone->description.name, clone->description);

		return clone;
	}

	StatementPtr SanitizeVisitor::Clone(DeclareVariableStatement& node)
	{
		auto clone = static_unique_pointer_cast<DeclareVariableStatement>(AstCloner::Clone(node));
		if (IsNoType(clone->varType))
		{
			if (!clone->initialExpression)
				throw AstError{ "variable must either have a type or an initial value" };

			clone->varType = ResolveType(GetExpressionType(*clone->initialExpression));
		}
		else
			clone->varType = ResolveType(clone->varType);

		clone->varIndex = RegisterVariable(clone->varName, clone->varType);

		return clone;
	}

	StatementPtr SanitizeVisitor::Clone(ExpressionStatement& node)
	{
		MandatoryExpr(node.expression);

		return AstCloner::Clone(node);
	}

	StatementPtr SanitizeVisitor::Clone(MultiStatement& node)
	{
		for (auto& statement : node.statements)
			MandatoryStatement(statement);

		PushScope();

		auto clone = static_unique_pointer_cast<MultiStatement>(AstCloner::Clone(node));

		PopScope();

		return clone;
	}

	Expression& SanitizeVisitor::MandatoryExpr(ExpressionPtr& node)
	{
		if (!node)
			throw AstError{ "Invalid expression" };

		return *node;
	}

	Statement& SanitizeVisitor::MandatoryStatement(StatementPtr& node)
	{
		if (!node)
			throw AstError{ "Invalid statement" };

		return *node;
	}

	void SanitizeVisitor::PushScope()
	{
		m_scopeSizes.push_back(m_identifiersInScope.size());
	}

	void SanitizeVisitor::PopScope()
	{
		assert(!m_scopeSizes.empty());
		m_identifiersInScope.resize(m_scopeSizes.back());
		m_scopeSizes.pop_back();
	}

	std::size_t SanitizeVisitor::ResolveStruct(const ExpressionType& exprType)
	{
		return std::visit([&](auto&& arg) -> std::size_t
		{
			using T = std::decay_t<decltype(arg)>;

			if constexpr (std::is_same_v<T, IdentifierType> || std::is_same_v<T, StructType> || std::is_same_v<T, UniformType>)
				return ResolveStruct(arg);
			else if constexpr (std::is_same_v<T, NoType> ||
			                   std::is_same_v<T, PrimitiveType> ||
			                   std::is_same_v<T, MatrixType> ||
			                   std::is_same_v<T, SamplerType> ||
			                   std::is_same_v<T, VectorType>)
			{
				throw AstError{ "expression is not a structure" };
			}
			else
				static_assert(AlwaysFalse<T>::value, "non-exhaustive visitor");
		}, exprType);
	}

	std::size_t SanitizeVisitor::ResolveStruct(const IdentifierType& identifierType)
	{
		const Identifier* identifier = FindIdentifier(identifierType.name);
		if (!identifier)
			throw AstError{ "unknown identifier " + identifierType.name };

		if (!std::holds_alternative<Struct>(identifier->value))
			throw AstError{ identifierType.name + " is not a struct" };

		return std::get<Struct>(identifier->value).structIndex;
	}

	std::size_t SanitizeVisitor::ResolveStruct(const StructType& structType)
	{
		return structType.structIndex;
	}

	std::size_t SanitizeVisitor::ResolveStruct(const UniformType& uniformType)
	{
		return std::visit([&](auto&& arg) -> std::size_t
		{
			using T = std::decay_t<decltype(arg)>;

			if constexpr (std::is_same_v<T, IdentifierType> || std::is_same_v<T, StructType>)
				return ResolveStruct(arg);
			else
				static_assert(AlwaysFalse<T>::value, "non-exhaustive visitor");
		}, uniformType.containedType);
	}

	ExpressionType SanitizeVisitor::ResolveType(const ExpressionType& exprType)
	{
		return std::visit([&](auto&& arg) -> ExpressionType
		{
			using T = std::decay_t<decltype(arg)>;

			if constexpr (std::is_same_v<T, NoType> ||
			              std::is_same_v<T, PrimitiveType> ||
			              std::is_same_v<T, MatrixType> ||
			              std::is_same_v<T, SamplerType> ||
			              std::is_same_v<T, StructType> ||
			              std::is_same_v<T, VectorType>)
			{
				return exprType;
			}
			else if constexpr (std::is_same_v<T, IdentifierType>)
			{
				const Identifier* identifier = FindIdentifier(arg.name);
				if (!identifier)
					throw AstError{ "unknown identifier " + arg.name };

				if (!std::holds_alternative<Struct>(identifier->value))
					throw AstError{ "expected type identifier" };

				return StructType{ std::get<Struct>(identifier->value).structIndex };
			}
			else if constexpr (std::is_same_v<T, UniformType>)
			{
				return std::visit([&](auto&& containedArg)
				{
					ExpressionType resolvedType = ResolveType(containedArg);
					assert(std::holds_alternative<StructType>(resolvedType));

					return UniformType{ std::get<StructType>(resolvedType) };
				}, arg.containedType);
			}
			else
				static_assert(AlwaysFalse<T>::value, "non-exhaustive visitor");
		}, exprType);
	}

	void SanitizeVisitor::TypeMustMatch(ExpressionPtr& left, ExpressionPtr& right)
	{
		return TypeMustMatch(GetExpressionType(*left), GetExpressionType(*right));
	}

	void SanitizeVisitor::TypeMustMatch(const ExpressionType& left, const ExpressionType& right)
	{
		if (left != right)
			throw AstError{ "Left expression type must match right expression type" };
	}
}
