// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/Ast/SanitizeVisitor.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/StackArray.hpp>
#include <Nazara/Shader/ShaderBuilder.hpp>
#include <Nazara/Shader/Ast/AstRecursiveVisitor.hpp>
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
		struct FunctionData
		{
			std::optional<ShaderStageType> stageType;
			Bitset<> calledFunctions;
			DeclareFunctionStatement* statement;
			FunctionFlags flags;
		};

		Options options;
		std::array<DeclareFunctionStatement*, ShaderStageTypeCount> entryFunctions = {};
		std::unordered_set<std::string> declaredExternalVar;
		std::unordered_set<UInt64> usedBindingIndexes;
		FunctionData* currentFunction = nullptr;
	};

	StatementPtr SanitizeVisitor::Sanitize(Statement& statement, const Options& options, std::string* error)
	{
		StatementPtr clone;

		Context currentContext;
		currentContext.options = options;

		m_context = &currentContext;
		CallOnExit resetContext([&] { m_context = nullptr; });

		PushScope(); //< Global scope
		{
			RegisterIntrinsic("cross", IntrinsicType::CrossProduct);
			RegisterIntrinsic("dot", IntrinsicType::DotProduct);
			RegisterIntrinsic("length", IntrinsicType::Length);
			RegisterIntrinsic("max", IntrinsicType::Max);
			RegisterIntrinsic("min", IntrinsicType::Min);
			RegisterIntrinsic("pow", IntrinsicType::Pow);

			// Collect function name and their types
			if (statement.GetType() == NodeType::MultiStatement)
			{
				const MultiStatement& multiStatement = static_cast<const MultiStatement&>(statement);
				for (auto& statementPtr : multiStatement.statements)
				{
					if (statementPtr->GetType() == NodeType::DeclareFunctionStatement)
						DeclareFunction(static_cast<DeclareFunctionStatement&>(*statementPtr));
				}
			}
			else if (statement.GetType() == NodeType::DeclareFunctionStatement)
				DeclareFunction(static_cast<DeclareFunctionStatement&>(statement));

			try
			{
				clone = AstCloner::Clone(statement);
			}
			catch (const AstError& err)
			{
				if (!error)
					throw std::runtime_error(err.errMsg);

				*error = err.errMsg;
			}

			ResolveFunctions();
		}
		PopScope();

		return clone;
	}

	ExpressionPtr SanitizeVisitor::Clone(AccessIdentifierExpression& node)
	{
		if (node.identifiers.empty())
			throw AstError{ "AccessIdentifierExpression must have at least one identifier" };

		ExpressionPtr indexedExpr = CloneExpression(MandatoryExpr(node.expr));
		for (std::size_t i = 0; i < node.identifiers.size(); ++i)
		{
			const std::string& identifier = node.identifiers[i];

			const ExpressionType& exprType = GetExpressionType(*indexedExpr);
			if (IsStructType(exprType))
			{
				// Transform to AccessIndexExpression
				AccessIndexExpression* accessIndexPtr;
				if (indexedExpr->GetType() != NodeType::AccessIndexExpression)
				{
					std::unique_ptr<AccessIndexExpression> accessIndex = std::make_unique<AccessIndexExpression>();
					accessIndex->expr = std::move(indexedExpr);

					accessIndexPtr = accessIndex.get();
					indexedExpr = std::move(accessIndex);
				}
				else
					accessIndexPtr = static_cast<AccessIndexExpression*>(indexedExpr.get());

				std::size_t structIndex = ResolveStruct(exprType);
				assert(structIndex < m_structs.size());
				const StructDescription& s = m_structs[structIndex];

				auto it = std::find_if(s.members.begin(), s.members.end(), [&](const auto& field) { return field.name == identifier; });
				if (it == s.members.end())
					throw AstError{ "unknown field " + identifier };

				accessIndexPtr->indices.push_back(ShaderBuilder::Constant(Int32(std::distance(s.members.begin(), it))));
				accessIndexPtr->cachedExpressionType = ResolveType(it->type);
			}
			else if (IsVectorType(exprType))
			{
				// Swizzle expression
				const VectorType& swizzledVec = std::get<VectorType>(exprType);

				auto swizzle = std::make_unique<SwizzleExpression>();
				swizzle->expression = std::move(indexedExpr);

				if (node.identifiers.size() - i != 1)
					throw AstError{ "invalid swizzle" };

				const std::string& swizzleStr = node.identifiers[i];
				if (swizzleStr.empty() || swizzleStr.size() > swizzle->components.size())
					throw AstError{ "invalid swizzle" };

				swizzle->componentCount = swizzleStr.size();

				if (swizzle->componentCount > 1)
					swizzle->cachedExpressionType = VectorType{ swizzle->componentCount, swizzledVec.type };
				else
					swizzle->cachedExpressionType = swizzledVec.type;

				for (std::size_t j = 0; j < swizzle->componentCount; ++j)
				{
					switch (swizzleStr[j])
					{
						case 'r':
						case 'x':
						case 's':
							swizzle->components[j] = SwizzleComponent::First;
							break;

						case 'g':
						case 'y':
						case 't':
							swizzle->components[j] = SwizzleComponent::Second;
							break;

						case 'b':
						case 'z':
						case 'p':
							swizzle->components[j] = SwizzleComponent::Third;
							break;

						case 'a':
						case 'w':
						case 'q':
							swizzle->components[j] = SwizzleComponent::Fourth;
							break;
					}
				}

				indexedExpr = std::move(swizzle);
			}
			else
				throw AstError{ "unexpected type (only struct and vectors can be indexed with identifiers)" }; //< TODO: Add support for arrays
		}

		return indexedExpr;
	}

	ExpressionPtr SanitizeVisitor::Clone(AccessIndexExpression& node)
	{
		MandatoryExpr(node.expr);
		for (auto& index : node.indices)
			MandatoryExpr(index);

		auto clone = static_unique_pointer_cast<AccessIndexExpression>(AstCloner::Clone(node));
		Validate(*clone);

		return clone;
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
						clone->cachedExpressionType = leftExprType;
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

	ExpressionPtr SanitizeVisitor::Clone(CallFunctionExpression& node)
	{
		if (!m_context->currentFunction)
			throw AstError{ "function calls must happen inside a function" };

		auto clone = std::make_unique<CallFunctionExpression>();

		clone->parameters.reserve(node.parameters.size());
		for (std::size_t i = 0; i < node.parameters.size(); ++i)
			clone->parameters.push_back(CloneExpression(node.parameters[i]));

		std::size_t targetFuncIndex;
		if (std::holds_alternative<std::string>(node.targetFunction))
		{
			const std::string& functionName = std::get<std::string>(node.targetFunction);

			const Identifier* identifier = FindIdentifier(functionName);
			if (identifier)
			{
				if (identifier->type == Identifier::Type::Intrinsic)
				{
					// Intrinsic function call
					std::vector<ExpressionPtr> parameters;
					parameters.reserve(node.parameters.size());

					for (const auto& param : node.parameters)
						parameters.push_back(CloneExpression(param));

					auto intrinsic = ShaderBuilder::Intrinsic(m_intrinsics[identifier->index], std::move(parameters));
					Validate(*intrinsic);

					return intrinsic;
				}
				else
				{
					// Regular function call
					if (identifier->type != Identifier::Type::Function)
						throw AstError{ "function expected" };

					clone->targetFunction = identifier->index;
					targetFuncIndex = identifier->index;
				}
			}
			else
			{
				// Identifier not found, maybe the function is declared later
				auto it = std::find_if(m_functions.begin(), m_functions.end(), [&](const auto& funcData) { return funcData.node->name == functionName; });
				if (it == m_functions.end())
					throw AstError{ "function " + functionName + " does not exist" };

				targetFuncIndex = std::distance(m_functions.begin(), it);

				clone->targetFunction = targetFuncIndex;
			}
		}
		else
			targetFuncIndex = std::get<std::size_t>(node.targetFunction);

		m_context->currentFunction->calledFunctions.UnboundedSet(targetFuncIndex);

		Validate(*clone, m_functions[targetFuncIndex].node);

		return clone;
	}

	ExpressionPtr SanitizeVisitor::Clone(CastExpression& node)
	{
		auto clone = static_unique_pointer_cast<CastExpression>(AstCloner::Clone(node));

		clone->cachedExpressionType = clone->targetType;
		clone->targetType = ResolveType(clone->targetType);

		//FIXME: Make proper rules
		if (IsMatrixType(clone->targetType) && clone->expressions.front())
		{
			const ExpressionType& exprType = GetExpressionType(*clone->expressions.front());
			if (IsMatrixType(exprType) && !clone->expressions[1])
			{
				return clone;
			}
		}

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
		clone->cachedExpressionType = GetExpressionType(clone->value);

		return clone;
	}

	ExpressionPtr SanitizeVisitor::Clone(IdentifierExpression& node)
	{
		assert(m_context);

		const Identifier* identifier = FindIdentifier(node.identifier);
		if (!identifier)
			throw AstError{ "unknown identifier " + node.identifier };

		if (identifier->type != Identifier::Type::Variable)
			throw AstError{ "expected variable identifier" };

		// Replace IdentifierExpression by VariableExpression
		auto varExpr = std::make_unique<VariableExpression>();
		varExpr->cachedExpressionType = m_variableTypes[identifier->index];
		varExpr->variableId = identifier->index;

		return varExpr;
	}

	ExpressionPtr SanitizeVisitor::Clone(IntrinsicExpression& node)
	{
		auto clone = static_unique_pointer_cast<IntrinsicExpression>(AstCloner::Clone(node));
		Validate(*clone);

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

		if (identifier->type != Identifier::Type::Option)
			throw AstError{ "expected option identifier" };

		condExpr->optionIndex = identifier->index;

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

	ExpressionPtr SanitizeVisitor::Clone(UnaryExpression& node)
	{
		auto clone = static_unique_pointer_cast<UnaryExpression>(AstCloner::Clone(node));

		const ExpressionType& exprType = GetExpressionType(MandatoryExpr(clone->expression));

		switch (node.op)
		{
			case UnaryType::LogicalNot:
			{
				if (exprType != ExpressionType(PrimitiveType::Boolean))
					throw AstError{ "logical not is only supported on booleans" };

				break;
			}

			case UnaryType::Minus:
			case UnaryType::Plus:
			{
				ShaderAst::PrimitiveType basicType;
				if (IsPrimitiveType(exprType))
					basicType = std::get<ShaderAst::PrimitiveType>(exprType);
				else if (IsVectorType(exprType))
					basicType = std::get<ShaderAst::VectorType>(exprType).type;
				else
					throw AstError{ "plus and minus unary expressions are only supported on primitive/vectors types" };

				if (basicType != PrimitiveType::Float32 && basicType != PrimitiveType::Int32 && basicType != PrimitiveType::UInt32)
					throw AstError{ "plus and minus unary expressions are only supported on floating points and integers types" };

				break;
			}
		}

		clone->cachedExpressionType = exprType;

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
			if (!extVar.bindingIndex)
				throw AstError{ "external variable " + extVar.name + " requires a binding index" };

			if (!extVar.bindingSet)
				throw AstError{ "external variable " + extVar.name + " requires a binding set" };

			UInt64 bindingIndex = *extVar.bindingIndex;
			UInt64 bindingSet = *extVar.bindingSet;
			UInt64 bindingKey = bindingSet << 32 | bindingIndex;
			if (m_context->usedBindingIndexes.find(bindingKey) != m_context->usedBindingIndexes.end())
				throw AstError{ "Binding (set=" + std::to_string(bindingSet) + ", binding=" + std::to_string(bindingIndex) + ") is already in use" };

			m_context->usedBindingIndexes.insert(bindingKey);

			if (m_context->declaredExternalVar.find(extVar.name) != m_context->declaredExternalVar.end())
				throw AstError{ "External variable " + extVar.name + " is already declared" };

			m_context->declaredExternalVar.insert(extVar.name);
		}

		auto clone = static_unique_pointer_cast<DeclareExternalStatement>(AstCloner::Clone(node));
		for (auto& extVar : clone->externalVars)
		{
			SanitizeIdentifier(extVar.name);

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

			if (stageType != ShaderStageType::Fragment)
			{
				if (node.depthWrite.has_value())
					throw AstError{ "only fragment entry-points can have the depth_write attribute" };

				if (node.earlyFragmentTests.has_value())
					throw AstError{ "only functions with entry(frag) attribute can have the early_fragments_tests attribute" };
			}
		}

		auto clone = std::make_unique<DeclareFunctionStatement>();
		clone->depthWrite = node.depthWrite;
		clone->earlyFragmentTests = node.earlyFragmentTests;
		clone->entryStage = node.entryStage;
		clone->name = node.name;
		clone->optionName = node.optionName;
		clone->parameters = node.parameters;
		clone->returnType = ResolveType(node.returnType);

		SanitizeIdentifier(clone->name);

		Context::FunctionData tempFuncData;
		tempFuncData.stageType = node.entryStage;

		m_context->currentFunction = &tempFuncData;

		PushScope();
		{
			for (auto& parameter : clone->parameters)
			{
				parameter.type = ResolveType(parameter.type);
				std::size_t varIndex = RegisterVariable(parameter.name, parameter.type);
				if (!clone->varIndex)
					clone->varIndex = varIndex; //< First parameter variable index is node variable index

				SanitizeIdentifier(parameter.name);
			}

			clone->statements.reserve(node.statements.size());
			for (auto& statement : node.statements)
				clone->statements.push_back(CloneStatement(MandatoryStatement(statement)));
		}
		PopScope();

		m_context->currentFunction = nullptr;

		if (clone->earlyFragmentTests.has_value() && *clone->earlyFragmentTests)
		{
			//TODO: warning and disable early fragment tests
			throw AstError{ "discard is not compatible with early fragment tests" };
		}

		if (!clone->optionName.empty())
		{
			const Identifier* identifier = FindIdentifier(node.optionName);
			if (!identifier)
				throw AstError{ "unknown option " + node.optionName };

			if (identifier->type != Identifier::Type::Option)
				throw AstError{ "expected option identifier" };

			std::size_t optionIndex = identifier->index;

			return ShaderBuilder::ConditionalStatement(optionIndex, std::move(clone));
		}

		auto it = std::find_if(m_functions.begin(), m_functions.end(), [&](const auto& funcData) { return funcData.node == &node; });
		assert(it != m_functions.end());
		assert(!it->defined);

		std::size_t funcIndex = std::distance(m_functions.begin(), it);

		clone->funcIndex = funcIndex;

		auto& funcData = RegisterFunction(funcIndex);
		funcData.flags = tempFuncData.flags;

		for (std::size_t i = tempFuncData.calledFunctions.FindFirst(); i != tempFuncData.calledFunctions.npos; i = tempFuncData.calledFunctions.FindNext(i))
		{
			assert(i < m_functions.size());
			auto& targetFunc = m_functions[i];
			targetFunc.calledByFunctions.UnboundedSet(funcIndex);
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

		SanitizeIdentifier(clone->description.name);

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

		SanitizeIdentifier(clone->varName);

		return clone;
	}

	StatementPtr SanitizeVisitor::Clone(DiscardStatement& node)
	{
		if (!m_context->currentFunction)
			throw AstError{ "discard can only be used inside a function" };

		m_context->currentFunction->flags |= FunctionFlag::DoesDiscard;

		return AstCloner::Clone(node);
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

	std::size_t SanitizeVisitor::DeclareFunction(DeclareFunctionStatement& funcDecl)
	{
		std::size_t functionIndex = m_functions.size();
		auto& funcData = m_functions.emplace_back();
		funcData.node = &funcDecl;

		return functionIndex;
	}

	void SanitizeVisitor::PropagateFunctionFlags(std::size_t funcIndex, FunctionFlags flags, Bitset<>& seen)
	{
		assert(funcIndex < m_functions.size());
		auto& funcData = m_functions[funcIndex];
		assert(funcData.defined);

		funcData.flags |= flags;

		for (std::size_t i = funcData.calledByFunctions.FindFirst(); i != funcData.calledByFunctions.npos; i = funcData.calledByFunctions.FindNext(i))
			PropagateFunctionFlags(i, funcData.flags, seen);
	}

	auto SanitizeVisitor::RegisterFunction(std::size_t functionIndex) -> FunctionData&
	{
		assert(m_functions.size() >= functionIndex);
		auto& funcData = m_functions[functionIndex];
		assert(!funcData.defined);
		funcData.defined = true;

		if (auto* identifier = FindIdentifier(funcData.node->name))
		{
			bool duplicate = true;

			// Functions cannot be declared twice, except for entry ones if their stages are different
			if (funcData.node->entryStage && identifier->type == Identifier::Type::Function)
			{
				auto& otherFunction = m_functions[identifier->index];
				if (funcData.node->entryStage != otherFunction.node->entryStage)
					duplicate = false;
			}

			if (duplicate)
				throw AstError{ funcData.node->name + " is already used" };
		}

		m_identifiersInScope.push_back({
			funcData.node->name,
			functionIndex,
			Identifier::Type::Function
		});

		return funcData;
	}

	std::size_t SanitizeVisitor::RegisterIntrinsic(std::string name, IntrinsicType type)
	{
		if (FindIdentifier(name))
			throw AstError{ name + " is already used" };

		std::size_t intrinsicIndex = m_intrinsics.size();
		m_intrinsics.push_back(type);

		m_identifiersInScope.push_back({
			std::move(name),
			intrinsicIndex,
			Identifier::Type::Intrinsic
		});

		return intrinsicIndex;
	}

	std::size_t SanitizeVisitor::RegisterOption(std::string name, ExpressionType type)
	{
		if (FindIdentifier(name))
			throw AstError{ name + " is already used" };

		std::size_t optionIndex = m_options.size();
		m_options.emplace_back(std::move(type));

		m_identifiersInScope.push_back({
			std::move(name),
			optionIndex,
			Identifier::Type::Option
		});

		return optionIndex;
	}

	std::size_t SanitizeVisitor::RegisterStruct(std::string name, StructDescription description)
	{
		if (FindIdentifier(name))
			throw AstError{ name + " is already used" };

		std::size_t structIndex = m_structs.size();
		m_structs.emplace_back(std::move(description));

		m_identifiersInScope.push_back({
			std::move(name),
			structIndex,
			Identifier::Type::Struct
		});

		return structIndex;
	}

	std::size_t SanitizeVisitor::RegisterVariable(std::string name, ExpressionType type)
	{
		// Allow variable shadowing
		if (auto* identifier = FindIdentifier(name); identifier && identifier->type != Identifier::Type::Variable)
			throw AstError{ name + " is already used" };

		std::size_t varIndex = m_variableTypes.size();
		m_variableTypes.emplace_back(std::move(type));

		m_identifiersInScope.push_back({
			std::move(name),
			varIndex,
			Identifier::Type::Variable
		});

		return varIndex;
	}

	void SanitizeVisitor::ResolveFunctions()
	{
		// Once every function is known, we can propagate flags

		Bitset<> seen;
		for (std::size_t funcIndex = 0; funcIndex < m_functions.size(); ++funcIndex)
		{
			auto& funcData = m_functions[funcIndex];

			PropagateFunctionFlags(funcIndex, funcData.flags, seen);
			seen.Clear();
		}

		for (const FunctionData& funcData : m_functions)
		{
			if (funcData.flags.Test(ShaderAst::FunctionFlag::DoesDiscard) && funcData.node->entryStage && *funcData.node->entryStage != ShaderStageType::Fragment)
				throw AstError{ "discard can only be used in the fragment stage" };
		}
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

		if (identifier->type != Identifier::Type::Struct)
			throw AstError{ identifierType.name + " is not a struct" };

		return identifier->index;
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

				if (identifier->type != Identifier::Type::Struct)
					throw AstError{ "expected type identifier" };

				return StructType{ identifier->index };
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

	void SanitizeVisitor::SanitizeIdentifier(std::string& identifier)
	{
		// Append _ until the identifier is no longer found
		while (m_context->options.reservedIdentifiers.find(identifier) != m_context->options.reservedIdentifiers.end())
		{
			do 
			{
				identifier += "_";
			}
			while (FindIdentifier(identifier) != nullptr);
		}
	}

	void SanitizeVisitor::Validate(AccessIndexExpression& node)
	{
		if (node.indices.empty())
			throw AstError{ "AccessIndexExpression must have at least one index" };

		for (auto& index : node.indices)
		{
			const ShaderAst::ExpressionType& indexType = GetExpressionType(*index);
			if (!IsPrimitiveType(indexType) || std::get<PrimitiveType>(indexType) != PrimitiveType::Int32)
				throw AstError{ "AccessIndex expects Int32 indices" };
		}

		ExpressionType exprType = GetExpressionType(*node.expr);
		for (std::size_t i = 0; i < node.indices.size(); ++i)
		{
			if (IsStructType(exprType))
			{
				auto& indexExpr = node.indices[i];

				const ShaderAst::ExpressionType& indexType = GetExpressionType(*indexExpr);
				if (indexExpr->GetType() != NodeType::ConstantExpression)
					throw AstError{ "struct can only be accessed with constant indices" };

				ConstantExpression& constantExpr = static_cast<ConstantExpression&>(*indexExpr);

				Int32 index = std::get<Int32>(constantExpr.value);

				std::size_t structIndex = ResolveStruct(exprType);
				assert(structIndex < m_structs.size());
				const StructDescription& s = m_structs[structIndex];

				exprType = ResolveType(s.members[index].type);
			}
			else if (IsMatrixType(exprType))
			{
				// Matrix index (ex: mat[2])
				const MatrixType& matrixType = std::get<MatrixType>(exprType);

				//TODO: Handle row-major matrices
				exprType = VectorType{ matrixType.rowCount, matrixType.type };
			}
			else if (IsVectorType(exprType))
			{
				// Swizzle expression with one component (ex: vec[2])
				const VectorType& swizzledVec = std::get<VectorType>(exprType);

				exprType = swizzledVec.type;
			}
			else
				throw AstError{ "unexpected type (only struct, vectors and matrices can be indexed)" }; //< TODO: Add support for arrays
		}

		node.cachedExpressionType = std::move(exprType);
	}

	void SanitizeVisitor::Validate(CallFunctionExpression& node, const DeclareFunctionStatement* referenceDeclaration)
	{
		if (referenceDeclaration->entryStage)
			throw AstError{ referenceDeclaration->name + " is an entry function which cannot be called by the program" };

		for (std::size_t i = 0; i < node.parameters.size(); ++i)
		{
			if (GetExpressionType(*node.parameters[i]) != referenceDeclaration->parameters[i].type)
				throw AstError{ "function " + referenceDeclaration->name + " parameter " + std::to_string(i) + " type mismatch" };
		}

		if (node.parameters.size() != referenceDeclaration->parameters.size())
			throw AstError{ "function " + referenceDeclaration->name + " expected " + std::to_string(referenceDeclaration->parameters.size()) + " parameters, got " + std::to_string(node.parameters.size()) };

		node.cachedExpressionType = referenceDeclaration->returnType;
	}

	void SanitizeVisitor::Validate(IntrinsicExpression& node)
	{
		// Parameter validation
		switch (node.intrinsic)
		{
			case IntrinsicType::CrossProduct:
			case IntrinsicType::DotProduct:
			case IntrinsicType::Max:
			case IntrinsicType::Min:
			case IntrinsicType::Pow:
			{
				if (node.parameters.size() != 2)
					throw AstError { "Expected two parameters" };

				for (auto& param : node.parameters)
					MandatoryExpr(param);

				const ExpressionType& type = GetExpressionType(*node.parameters.front());

				for (std::size_t i = 1; i < node.parameters.size(); ++i)
				{
					if (type != GetExpressionType(*node.parameters[i]))
						throw AstError{ "All type must match" };
				}

				break;
			}

			case IntrinsicType::Length:
			{
				if (node.parameters.size() != 1)
					throw AstError{ "Expected only one parameters" };

				for (auto& param : node.parameters)
					MandatoryExpr(param);

				const ExpressionType& type = GetExpressionType(*node.parameters.front());
				if (!IsVectorType(type))
					throw AstError{ "Expected a vector" };

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

				break;
			}
		}

		// Return type attribution
		switch (node.intrinsic)
		{
			case IntrinsicType::CrossProduct:
			{
				const ExpressionType& type = GetExpressionType(*node.parameters.front());
				if (type != ExpressionType{ VectorType{ 3, PrimitiveType::Float32 } })
					throw AstError{ "CrossProduct only works with vec3<f32> expressions" };

				node.cachedExpressionType = type;
				break;
			}

			case IntrinsicType::DotProduct:
			case IntrinsicType::Length:
			{
				ExpressionType type = GetExpressionType(*node.parameters.front());
				if (!IsVectorType(type))
					throw AstError{ "DotProduct expects vector types" };

				node.cachedExpressionType = std::get<VectorType>(type).type;
				break;
			}

			case IntrinsicType::Max:
			case IntrinsicType::Min:
			{
				const ExpressionType& type = GetExpressionType(*node.parameters.front());
				if (!IsPrimitiveType(type) && !IsVectorType(type))
					throw AstError{ "max and min only work with primitive and vector types" };

				if ((IsPrimitiveType(type) && std::get<PrimitiveType>(type) == PrimitiveType::Boolean) ||
				    (IsVectorType(type) && std::get<VectorType>(type).type == PrimitiveType::Boolean))
					throw AstError{ "max and min do not work with booleans" };

				node.cachedExpressionType = type;
				break;
			}

			case IntrinsicType::Pow:
			{
				const ExpressionType& type = GetExpressionType(*node.parameters.front());
				if (!IsPrimitiveType(type) && !IsVectorType(type))
					throw AstError{ "pow only works with primitive and vector types" };

				if ((IsPrimitiveType(type) && std::get<PrimitiveType>(type) != PrimitiveType::Float32) ||
					(IsVectorType(type) && std::get<VectorType>(type).type != PrimitiveType::Float32))
					throw AstError{ "pow only works with floating-point primitive or vectors" };

				node.cachedExpressionType = type;
				break;
			}

			case IntrinsicType::SampleTexture:
			{
				node.cachedExpressionType = VectorType{ 4, std::get<SamplerType>(GetExpressionType(*node.parameters.front())).sampledType };
				break;
			}
		}
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
