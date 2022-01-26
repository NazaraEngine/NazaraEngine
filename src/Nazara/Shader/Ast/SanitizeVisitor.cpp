// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/Ast/SanitizeVisitor.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/StackArray.hpp>
#include <Nazara/Shader/ShaderBuilder.hpp>
#include <Nazara/Shader/Ast/AstOptimizer.hpp>
#include <Nazara/Shader/Ast/AstRecursiveVisitor.hpp>
#include <Nazara/Shader/Ast/AstUtils.hpp>
#include <numeric>
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
			return std::unique_ptr<T>(SafeCast<T*>(ptr.release()));
		}
	}

	struct SanitizeVisitor::Context
	{
		struct CurrentFunctionData
		{
			std::optional<ShaderStageType> stageType;
			Bitset<> calledFunctions;
			DeclareFunctionStatement* statement;
			FunctionFlags flags;
		};

		std::size_t nextOptionIndex = 0;
		Options options;
		std::array<DeclareFunctionStatement*, ShaderStageTypeCount> entryFunctions = {};
		std::unordered_set<std::string> declaredExternalVar;
		std::unordered_set<UInt64> usedBindingIndexes;
		std::vector<Identifier> identifiersInScope;
		std::vector<ConstantValue> constantValues;
		std::vector<FunctionData> functions;
		std::vector<IntrinsicType> intrinsics;
		std::vector<StructDescription*> structs;
		std::vector<ExpressionType> variableTypes;
		std::vector<std::size_t> scopeSizes;
		CurrentFunctionData* currentFunction = nullptr;
		std::vector<StatementPtr>* currentStatementList = nullptr;
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
			RegisterIntrinsic("exp", IntrinsicType::Exp);
			RegisterIntrinsic("length", IntrinsicType::Length);
			RegisterIntrinsic("max", IntrinsicType::Max);
			RegisterIntrinsic("min", IntrinsicType::Min);
			RegisterIntrinsic("normalize", IntrinsicType::Normalize);
			RegisterIntrinsic("pow", IntrinsicType::Pow);
			RegisterIntrinsic("reflect", IntrinsicType::Reflect);

			// Collect function name and their types
			if (statement.GetType() == NodeType::MultiStatement)
			{
				const MultiStatement& multiStatement = static_cast<const MultiStatement&>(statement);
				for (auto& statementPtr : multiStatement.statements)
				{
					if (statementPtr->GetType() == NodeType::DeclareFunctionStatement)
						DeclareFunction(static_cast<DeclareFunctionStatement&>(*statementPtr));
					else if (statementPtr->GetType() == NodeType::ConditionalStatement)
					{
						const ConditionalStatement& condStatement = static_cast<const ConditionalStatement&>(*statementPtr);
						if (condStatement.statement->GetType() == NodeType::DeclareFunctionStatement)
							DeclareFunction(static_cast<DeclareFunctionStatement&>(*condStatement.statement));
					}
				}
			}
			else if (statement.GetType() == NodeType::DeclareFunctionStatement)
				DeclareFunction(static_cast<DeclareFunctionStatement&>(statement));
			else if (statement.GetType() == NodeType::ConditionalStatement)
			{
				const ConditionalStatement& condStatement = static_cast<const ConditionalStatement&>(statement);
				if (condStatement.statement->GetType() == NodeType::DeclareFunctionStatement)
					DeclareFunction(static_cast<DeclareFunctionStatement&>(*condStatement.statement));
			}

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
	
	UInt32 SanitizeVisitor::ToSwizzleIndex(char c)
	{
		switch (c)
		{
			case 'r':
			case 'x':
			case 's':
				return 0u;

			case 'g':
			case 'y':
			case 't':
				return 1u;

			case 'b':
			case 'z':
			case 'p':
				return 2u;

			case 'a':
			case 'w':
			case 'q':
				return 3u;

			default:
				throw AstError{ "unexpected character '" + std::string(&c, 1) + "' on swizzle " };
		}
	}

	ExpressionPtr SanitizeVisitor::Clone(AccessIdentifierExpression& node)
	{
		if (node.identifiers.empty())
			throw AstError{ "AccessIdentifierExpression must have at least one identifier" };

		ExpressionPtr indexedExpr = CloneExpression(MandatoryExpr(node.expr));
		for (const std::string& identifier : node.identifiers)
		{
			if (identifier.empty())
				throw AstError{ "empty identifier" };

			const ExpressionType& exprType = GetExpressionType(*indexedExpr);
			if (IsStructType(exprType))
			{
				std::size_t structIndex = ResolveStruct(exprType);
				assert(structIndex < m_context->structs.size());
				const StructDescription* s = m_context->structs[structIndex];

				// Retrieve member index (not counting disabled fields)
				Int32 fieldIndex = 0;
				const StructDescription::StructMember* fieldPtr = nullptr;
				for (const auto& field : s->members)
				{
					if (field.cond.HasValue() && !field.cond.GetResultingValue())
						continue;

					if (field.name == identifier)
					{
						fieldPtr = &field;
						break;
					}

					fieldIndex++;
				}

				if (!fieldPtr)
					throw AstError{ "unknown field " + identifier };

				if (m_context->options.useIdentifierAccessesForStructs)
				{
					// Use a AccessIdentifierExpression
					AccessIdentifierExpression* accessIdentifierPtr;
					if (indexedExpr->GetType() != NodeType::AccessIdentifierExpression)
					{
						std::unique_ptr<AccessIdentifierExpression> accessIndex = std::make_unique<AccessIdentifierExpression>();
						accessIndex->expr = std::move(indexedExpr);

						accessIdentifierPtr = accessIndex.get();
						indexedExpr = std::move(accessIndex);
					}
					else
						accessIdentifierPtr = static_cast<AccessIdentifierExpression*>(indexedExpr.get());

					accessIdentifierPtr->identifiers.push_back(fieldPtr->name);
					accessIdentifierPtr->cachedExpressionType = ResolveType(fieldPtr->type);
				}
				else
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

					accessIndexPtr->indices.push_back(ShaderBuilder::Constant(fieldIndex));
					accessIndexPtr->cachedExpressionType = ResolveType(fieldPtr->type);
				}
			}
			else if (IsPrimitiveType(exprType) || IsVectorType(exprType))
			{
				// Swizzle expression
				std::size_t swizzleComponentCount = identifier.size();
				if (swizzleComponentCount > 4)
					throw AstError{ "cannot swizzle more than four elements" };

				if (m_context->options.removeScalarSwizzling && IsPrimitiveType(exprType))
				{
					for (std::size_t j = 0; j < swizzleComponentCount; ++j)
					{
						if (ToSwizzleIndex(identifier[j]) != 0)
							throw AstError{ "invalid swizzle" };
					}

					if (swizzleComponentCount == 1)
						continue; //< ignore this swizzle (a.x == a)

					// Use a Cast expression to replace swizzle
					indexedExpr = CacheResult(std::move(indexedExpr)); //< Since we are going to use a value multiple times, cache it if required

					PrimitiveType baseType;
					if (IsVectorType(exprType))
						baseType = std::get<VectorType>(exprType).type;
					else
						baseType = std::get<PrimitiveType>(exprType);

					auto cast = std::make_unique<CastExpression>();
					cast->targetType = VectorType{ swizzleComponentCount, baseType };
					for (std::size_t j = 0; j < swizzleComponentCount; ++j)
						cast->expressions[j] = CloneExpression(indexedExpr);

					Validate(*cast);

					indexedExpr = std::move(cast);
				}
				else
				{
					auto swizzle = std::make_unique<SwizzleExpression>();
					swizzle->expression = std::move(indexedExpr);

					swizzle->componentCount = swizzleComponentCount;
					for (std::size_t j = 0; j < swizzleComponentCount; ++j)
						swizzle->components[j] = ToSwizzleIndex(identifier[j]);

					Validate(*swizzle);

					indexedExpr = std::move(swizzle);
				}
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

		// TODO: Handle AccessIndex on structs with m_context->options.useIdentifierAccessesForStructs

		return clone;
	}

	ExpressionPtr SanitizeVisitor::Clone(AssignExpression& node)
	{
		MandatoryExpr(node.left);
		MandatoryExpr(node.right);

		auto clone = static_unique_pointer_cast<AssignExpression>(AstCloner::Clone(node));
		Validate(*clone);

		return clone;
	}

	ExpressionPtr SanitizeVisitor::Clone(BinaryExpression& node)
	{
		auto clone = static_unique_pointer_cast<BinaryExpression>(AstCloner::Clone(node));
		Validate(*clone);

		return clone;
	}

	ExpressionPtr SanitizeVisitor::Clone(CallFunctionExpression& node)
	{
		if (!m_context->currentFunction)
			throw AstError{ "function calls must happen inside a function" };

		auto clone = std::make_unique<CallFunctionExpression>();

		clone->parameters.reserve(node.parameters.size());
		for (const auto& parameter : node.parameters)
			clone->parameters.push_back(CloneExpression(parameter));

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

					auto intrinsic = ShaderBuilder::Intrinsic(m_context->intrinsics[identifier->index], std::move(parameters));
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
				auto it = std::find_if(m_context->functions.begin(), m_context->functions.end(), [&](const auto& funcData) { return funcData.node->name == functionName; });
				if (it == m_context->functions.end())
					throw AstError{ "function " + functionName + " does not exist" };

				targetFuncIndex = std::distance(m_context->functions.begin(), it);

				clone->targetFunction = targetFuncIndex;
			}
		}
		else
			targetFuncIndex = std::get<std::size_t>(node.targetFunction);

		m_context->currentFunction->calledFunctions.UnboundedSet(targetFuncIndex);

		Validate(*clone, m_context->functions[targetFuncIndex].node);

		return clone;
	}

	ExpressionPtr SanitizeVisitor::Clone(CastExpression& node)
	{
		auto clone = static_unique_pointer_cast<CastExpression>(AstCloner::Clone(node));
		Validate(*clone);

		if (m_context->options.removeMatrixCast && IsMatrixType(clone->targetType))
		{
			const MatrixType& targetMatrixType = std::get<MatrixType>(clone->targetType);

			const ShaderAst::ExpressionType& frontExprType = GetExpressionType(*clone->expressions.front());
			bool isMatrixCast = IsMatrixType(frontExprType);
			if (isMatrixCast && std::get<MatrixType>(frontExprType) == targetMatrixType)
			{
				// Nothing to do
				return std::move(clone->expressions.front());
			}

			auto variableDeclaration = ShaderBuilder::DeclareVariable("temp", clone->targetType); //< Validation will prevent name-clash if required
			Validate(*variableDeclaration);

			std::size_t variableIndex = *variableDeclaration->varIndex;

			m_context->currentStatementList->emplace_back(std::move(variableDeclaration));

			for (std::size_t i = 0; i < targetMatrixType.columnCount; ++i)
			{
				// temp[i]
				auto columnExpr = ShaderBuilder::AccessIndex(ShaderBuilder::Variable(variableIndex, clone->targetType), ShaderBuilder::Constant(UInt32(i)));
				Validate(*columnExpr);

				// vector expression
				ExpressionPtr vectorExpr;
				std::size_t vectorComponentCount;
				if (isMatrixCast)
				{
					// fromMatrix[i]
					auto matrixColumnExpr = ShaderBuilder::AccessIndex(CloneExpression(clone->expressions.front()), ShaderBuilder::Constant(UInt32(i)));
					Validate(*matrixColumnExpr);

					vectorExpr = std::move(matrixColumnExpr);
					vectorComponentCount = std::get<MatrixType>(frontExprType).rowCount;
				}
				else
				{
					// parameter #i
					vectorExpr = std::move(clone->expressions[i]);
					vectorComponentCount = std::get<VectorType>(GetExpressionType(*vectorExpr)).componentCount;
				}

				// cast expression (turn fromMatrix[i] to vec3[f32](fromMatrix[i]))
				ExpressionPtr castExpr;
				if (vectorComponentCount != targetMatrixType.rowCount)
				{
					CastExpressionPtr vecCast;
					if (vectorComponentCount < targetMatrixType.rowCount)
					{
						std::array<ExpressionPtr, 4> expressions;
						expressions[0] = std::move(vectorExpr);
						for (std::size_t j = 0; j < targetMatrixType.rowCount - vectorComponentCount; ++j)
							expressions[j + 1] = ShaderBuilder::Constant(targetMatrixType.type, (i == j + vectorComponentCount) ? 1 : 0); //< set 1 to diagonal

						vecCast = ShaderBuilder::Cast(VectorType{ targetMatrixType.rowCount, targetMatrixType.type }, std::move(expressions));
						Validate(*vecCast);

						castExpr = std::move(vecCast);
					}
					else
					{
						std::array<UInt32, 4> swizzleComponents;
						std::iota(swizzleComponents.begin(), swizzleComponents.begin() + targetMatrixType.rowCount, 0);

						auto swizzleExpr = ShaderBuilder::Swizzle(std::move(vectorExpr), swizzleComponents, targetMatrixType.rowCount);
						Validate(*swizzleExpr);

						castExpr = std::move(swizzleExpr);
					}
				}
				else
					castExpr = std::move(vectorExpr);

				// temp[i] = castExpr
				m_context->currentStatementList->emplace_back(ShaderBuilder::ExpressionStatement(ShaderBuilder::Assign(AssignType::Simple, std::move(columnExpr), std::move(castExpr))));
			}

			return ShaderBuilder::Variable(variableIndex, clone->targetType);
		}

		return clone;
	}

	ExpressionPtr SanitizeVisitor::Clone(ConditionalExpression& node)
	{
		MandatoryExpr(node.condition);
		MandatoryExpr(node.truePath);
		MandatoryExpr(node.falsePath);

		ConstantValue conditionValue = ComputeConstantValue(*AstCloner::Clone(*node.condition));
		if (GetExpressionType(conditionValue) != ExpressionType{ PrimitiveType::Boolean })
			throw AstError{ "expected a boolean value" };

		if (std::get<bool>(conditionValue))
			return AstCloner::Clone(*node.truePath);
		else
			return AstCloner::Clone(*node.falsePath);
	}

	ExpressionPtr SanitizeVisitor::Clone(ConstantValueExpression& node)
	{
		if (std::holds_alternative<NoValue>(node.value))
			throw std::runtime_error("expected a value");

		auto clone = static_unique_pointer_cast<ConstantValueExpression>(AstCloner::Clone(node));
		clone->cachedExpressionType = GetExpressionType(clone->value);

		return clone;
	}

	ExpressionPtr SanitizeVisitor::Clone(ConstantExpression& node)
	{
		if (node.constantId >= m_context->constantValues.size())
			throw AstError{ "invalid constant index " + std::to_string(node.constantId) };

		// Replace by constant value
		auto constant = ShaderBuilder::Constant(m_context->constantValues[node.constantId]);
		constant->cachedExpressionType = GetExpressionType(constant->value);

		return constant;
	}

	ExpressionPtr SanitizeVisitor::Clone(IdentifierExpression& node)
	{
		assert(m_context);

		const Identifier* identifier = FindIdentifier(node.identifier);
		if (!identifier)
			throw AstError{ "unknown identifier " + node.identifier };

		switch (identifier->type)
		{
			case Identifier::Type::Constant:
			{
				// Replace IdentifierExpression by Constant(Value)Expression
				ConstantExpression constantExpr;
				constantExpr.constantId = identifier->index;

				return Clone(constantExpr); //< Turn ConstantExpression into ConstantValueExpression
			}

			case Identifier::Type::Variable:
			{
				// Replace IdentifierExpression by VariableExpression
				auto varExpr = std::make_unique<VariableExpression>();
				varExpr->cachedExpressionType = m_context->variableTypes[identifier->index];
				varExpr->variableId = identifier->index;

				return varExpr;
			}

			default:
				throw AstError{ "expected constant or variable identifier" };
		}
	}

	ExpressionPtr SanitizeVisitor::Clone(IntrinsicExpression& node)
	{
		auto clone = static_unique_pointer_cast<IntrinsicExpression>(AstCloner::Clone(node));
		Validate(*clone);

		return clone;
	}

	ExpressionPtr SanitizeVisitor::Clone(SwizzleExpression& node)
	{
		auto clone = static_unique_pointer_cast<SwizzleExpression>(AstCloner::Clone(node));
		Validate(*clone);

		return clone;
	}

	ExpressionPtr SanitizeVisitor::Clone(UnaryExpression& node)
	{
		auto clone = static_unique_pointer_cast<UnaryExpression>(AstCloner::Clone(node));
		Validate(*clone);

		return clone;
	}

	ExpressionPtr SanitizeVisitor::Clone(VariableExpression& node)
	{
		auto clone = static_unique_pointer_cast<VariableExpression>(AstCloner::Clone(node));
		Validate(*clone);

		return clone;
	}

	StatementPtr SanitizeVisitor::Clone(BranchStatement& node)
	{
		if (node.isConst)
		{
			// Evaluate every condition at compilation and select the right statement
			for (auto& cond : node.condStatements)
			{
				MandatoryExpr(cond.condition);

				ConstantValue conditionValue = ComputeConstantValue(*AstCloner::Clone(*cond.condition));
				if (GetExpressionType(conditionValue) != ExpressionType{ PrimitiveType::Boolean })
					throw AstError{ "expected a boolean value" };

				if (std::get<bool>(conditionValue))
					return AstCloner::Clone(*cond.statement);
			}

			// Every condition failed, fallback to else if any
			if (node.elseStatement)
				return AstCloner::Clone(*node.elseStatement);
			else
				return ShaderBuilder::NoOp();
		}

		auto clone = std::make_unique<BranchStatement>();
		clone->condStatements.reserve(node.condStatements.size());

		if (!m_context->currentFunction)
			throw AstError{ "non-const branching statements can only exist inside a function" };

		BranchStatement* root = clone.get();
		for (std::size_t condIndex = 0; condIndex < node.condStatements.size(); ++condIndex)
		{
			auto& cond = node.condStatements[condIndex];

			PushScope();

			auto BuildCondStatement = [&](BranchStatement::ConditionalStatement& condStatement)
			{
				condStatement.condition = CloneExpression(MandatoryExpr(cond.condition));

				const ExpressionType& condType = GetExpressionType(*condStatement.condition);
				if (!IsPrimitiveType(condType) || std::get<PrimitiveType>(condType) != PrimitiveType::Boolean)
					throw AstError{ "branch expressions must resolve to boolean type" };

				condStatement.statement = CloneStatement(MandatoryStatement(cond.statement));
			};

			if (m_context->options.splitMultipleBranches && condIndex > 0)
			{
				auto currentBranch = std::make_unique<BranchStatement>();

				BuildCondStatement(currentBranch->condStatements.emplace_back());

				root->elseStatement = std::move(currentBranch);
				root = static_cast<BranchStatement*>(root->elseStatement.get());
			}
			else
				BuildCondStatement(clone->condStatements.emplace_back());

			PopScope();
		}

		if (node.elseStatement)
		{
			PushScope();
			root->elseStatement = CloneStatement(node.elseStatement);
			PopScope();
		}

		return clone;
	}

	StatementPtr SanitizeVisitor::Clone(ConditionalStatement& node)
	{
		MandatoryExpr(node.condition);
		MandatoryStatement(node.statement);

		ConstantValue conditionValue = ComputeConstantValue(*AstCloner::Clone(*node.condition));
		if (GetExpressionType(conditionValue) != ExpressionType{ PrimitiveType::Boolean })
			throw AstError{ "expected a boolean value" };

		if (std::get<bool>(conditionValue))
			return AstCloner::Clone(*node.statement);
		else
			return ShaderBuilder::NoOp();
	}

	StatementPtr SanitizeVisitor::Clone(DeclareConstStatement& node)
	{
		auto clone = static_unique_pointer_cast<DeclareConstStatement>(AstCloner::Clone(node));

		if (!clone->expression)
			throw AstError{ "const variables must have an expression" };

		clone->expression = Optimize(*clone->expression);
		if (clone->expression->GetType() != NodeType::ConstantValueExpression)
			throw AstError{ "const variable must have constant expressions " };

		const ConstantValue& value = static_cast<ConstantValueExpression&>(*clone->expression).value;

		ExpressionType expressionType = ResolveType(GetExpressionType(value));

		if (!IsNoType(clone->type) && ResolveType(clone->type) != expressionType)
			throw AstError{ "constant expression doesn't match type" };

		clone->type = expressionType;

		clone->constIndex = RegisterConstant(clone->name, value);

		if (m_context->options.removeConstDeclaration)
			return ShaderBuilder::NoOp();

		return clone;
	}

	StatementPtr SanitizeVisitor::Clone(DeclareExternalStatement& node)
	{
		assert(m_context);

		auto clone = static_unique_pointer_cast<DeclareExternalStatement>(AstCloner::Clone(node));

		UInt32 defaultBlockSet = 0;
		if (clone->bindingSet.HasValue())
			defaultBlockSet = ComputeAttributeValue(clone->bindingSet);

		for (auto& extVar : clone->externalVars)
		{
			if (!extVar.bindingIndex.HasValue())
				throw AstError{ "external variable " + extVar.name + " requires a binding index" };

			if (extVar.bindingSet.HasValue())
				ComputeAttributeValue(extVar.bindingSet);
			else
				extVar.bindingSet = defaultBlockSet;

			UInt64 bindingSet = extVar.bindingSet.GetResultingValue();

			UInt64 bindingIndex = ComputeAttributeValue(extVar.bindingIndex);

			UInt64 bindingKey = bindingSet << 32 | bindingIndex;
			if (m_context->usedBindingIndexes.find(bindingKey) != m_context->usedBindingIndexes.end())
				throw AstError{ "binding (set=" + std::to_string(bindingSet) + ", binding=" + std::to_string(bindingIndex) + ") is already in use" };

			m_context->usedBindingIndexes.insert(bindingKey);

			if (m_context->declaredExternalVar.find(extVar.name) != m_context->declaredExternalVar.end())
				throw AstError{ "external variable " + extVar.name + " is already declared" };

			m_context->declaredExternalVar.insert(extVar.name);

			extVar.type = ResolveType(extVar.type);

			ExpressionType varType;
			if (IsUniformType(extVar.type))
				varType = std::get<StructType>(std::get<UniformType>(extVar.type).containedType);
			else if (IsSamplerType(extVar.type))
				varType = extVar.type;
			else
				throw AstError{ "external variable " + extVar.name + " is of wrong type: only uniform and sampler are allowed in external blocks" };

			std::size_t varIndex = RegisterVariable(extVar.name, std::move(varType));
			if (!clone->varIndex)
				clone->varIndex = varIndex; //< First external variable index is node variable index

			SanitizeIdentifier(extVar.name);
		}

		return clone;
	}

	StatementPtr SanitizeVisitor::Clone(DeclareFunctionStatement& node)
	{
		if (m_context->currentFunction)
			throw AstError{ "a function cannot be defined inside another function" };

		auto clone = std::make_unique<DeclareFunctionStatement>();
		clone->name = node.name;
		clone->parameters = node.parameters;
		clone->returnType = ResolveType(node.returnType);

		if (node.depthWrite.HasValue())
			clone->depthWrite = ComputeAttributeValue(node.depthWrite);

		if (node.earlyFragmentTests.HasValue())
			clone->earlyFragmentTests = ComputeAttributeValue(node.earlyFragmentTests);

		if (node.entryStage.HasValue())
			clone->entryStage = ComputeAttributeValue(node.entryStage);

		if (clone->entryStage.HasValue())
		{
			ShaderStageType stageType = clone->entryStage.GetResultingValue();

			if (m_context->entryFunctions[UnderlyingCast(stageType)])
				throw AstError{ "the same entry type has been defined multiple times" };

			m_context->entryFunctions[UnderlyingCast(stageType)] = &node;

			if (node.parameters.size() > 1)
				throw AstError{ "entry functions can either take one struct parameter or no parameter" };

			if (stageType != ShaderStageType::Fragment)
			{
				if (node.depthWrite.HasValue())
					throw AstError{ "only fragment entry-points can have the depth_write attribute" };

				if (node.earlyFragmentTests.HasValue())
					throw AstError{ "only functions with entry(frag) attribute can have the early_fragments_tests attribute" };
			}
		}

		Context::CurrentFunctionData tempFuncData;
		if (node.entryStage.HasValue())
			tempFuncData.stageType = node.entryStage.GetResultingValue();

		m_context->currentFunction = &tempFuncData;

		std::vector<StatementPtr>* previousList = m_context->currentStatementList;
		m_context->currentStatementList = &clone->statements;

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

		m_context->currentStatementList = previousList;
		m_context->currentFunction = nullptr;

		if (clone->earlyFragmentTests.HasValue() && clone->earlyFragmentTests.GetResultingValue())
		{
			//TODO: warning and disable early fragment tests
			throw AstError{ "discard is not compatible with early fragment tests" };
		}

		auto it = std::find_if(m_context->functions.begin(), m_context->functions.end(), [&](const auto& funcData) { return funcData.node == &node; });
		assert(it != m_context->functions.end());
		assert(!it->defined);

		std::size_t funcIndex = std::distance(m_context->functions.begin(), it);

		clone->funcIndex = funcIndex;

		auto& funcData = RegisterFunction(funcIndex);
		funcData.flags = tempFuncData.flags;

		for (std::size_t i = tempFuncData.calledFunctions.FindFirst(); i != tempFuncData.calledFunctions.npos; i = tempFuncData.calledFunctions.FindNext(i))
		{
			assert(i < m_context->functions.size());
			auto& targetFunc = m_context->functions[i];
			targetFunc.calledByFunctions.UnboundedSet(funcIndex);
		}

		SanitizeIdentifier(clone->name);

		return clone;
	}

	StatementPtr SanitizeVisitor::Clone(DeclareOptionStatement& node)
	{
		if (m_context->currentFunction)
			throw AstError{ "options must be declared outside of functions" };

		auto clone = static_unique_pointer_cast<DeclareOptionStatement>(AstCloner::Clone(node));
		clone->optType = ResolveType(clone->optType);

		if (clone->defaultValue && clone->optType != GetExpressionType(*clone->defaultValue))
			throw AstError{ "option " + clone->optName + " default expression must be of the same type than the option" };

		std::size_t optionIndex = m_context->nextOptionIndex++;

		if (auto optionValueIt = m_context->options.optionValues.find(optionIndex); optionValueIt != m_context->options.optionValues.end())
			clone->optIndex = RegisterConstant(clone->optName, optionValueIt->second);
		else if (clone->defaultValue)
			clone->optIndex = RegisterConstant(clone->optName, ComputeConstantValue(*clone->defaultValue));
		else
			throw AstError{ "missing option " + clone->optName + " value (has no default value)" };

		if (m_context->options.removeOptionDeclaration)
			return ShaderBuilder::NoOp();

		return clone;
	}

	StatementPtr SanitizeVisitor::Clone(DeclareStructStatement& node)
	{
		if (m_context->currentFunction)
			throw AstError{ "structs must be declared outside of functions" };

		auto clone = static_unique_pointer_cast<DeclareStructStatement>(AstCloner::Clone(node));

		std::unordered_set<std::string> declaredMembers;
		for (auto& member : clone->description.members)
		{
			if (member.cond.HasValue())
			{
				member.cond = ComputeAttributeValue(member.cond);
				if (!member.cond.GetResultingValue())
					continue;
			}

			if (member.builtin.HasValue())
				member.builtin = ComputeAttributeValue(member.builtin);

			if (member.locationIndex.HasValue())
				member.locationIndex = ComputeAttributeValue(member.locationIndex);

			if (declaredMembers.find(member.name) != declaredMembers.end())
				throw AstError{ "struct member " + member.name + " found multiple time" };

			declaredMembers.insert(member.name);

			member.type = ResolveType(member.type);
			if (clone->description.layout.HasValue() && clone->description.layout.GetResultingValue() == StructLayout::Std140)
			{
				if (IsPrimitiveType(member.type) && std::get<PrimitiveType>(member.type) == PrimitiveType::Boolean)
					throw AstError{ "boolean type is not allowed in std140 layout" };
				else if (IsStructType(member.type))
				{
					std::size_t structIndex = std::get<StructType>(member.type).structIndex;
					const StructDescription* desc = m_context->structs[structIndex];
					if (!desc->layout.HasValue() || desc->layout.GetResultingValue() != clone->description.layout.GetResultingValue())
						throw AstError{ "inner struct layout mismatch" };
				}
			}
		}

		clone->structIndex = RegisterStruct(clone->description.name, &clone->description);

		SanitizeIdentifier(clone->description.name);

		return clone;
	}

	StatementPtr SanitizeVisitor::Clone(DeclareVariableStatement& node)
	{
		if (!m_context->currentFunction)
			throw AstError{ "global variables outside of external blocks are forbidden" };

		auto clone = static_unique_pointer_cast<DeclareVariableStatement>(AstCloner::Clone(node));
		Validate(*clone);

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

	StatementPtr SanitizeVisitor::Clone(ForStatement& node)
	{
		if (node.varName.empty())
			throw AstError{ "numerical for variable name cannot be empty" };

		auto fromExpr = CloneExpression(MandatoryExpr(node.fromExpr));
		auto stepExpr = CloneExpression(node.stepExpr);
		auto toExpr = CloneExpression(MandatoryExpr(node.toExpr));

		MandatoryStatement(node.statement);

		const ExpressionType& fromExprType = GetExpressionType(*fromExpr);
		if (!IsPrimitiveType(fromExprType))
			throw AstError{ "numerical for from expression must be an integer or unsigned integer" };

		PrimitiveType fromType = std::get<PrimitiveType>(fromExprType);
		if (fromType != PrimitiveType::Int32 && fromType != PrimitiveType::UInt32)
			throw AstError{ "numerical for from expression must be an integer or unsigned integer" };

		const ExpressionType& toExprType = GetExpressionType(*fromExpr);
		if (toExprType != fromExprType)
			throw AstError{ "numerical for to expression type must match from expression type" };

		if (stepExpr)
		{
			const ExpressionType& stepExprType = GetExpressionType(*fromExpr);
			if (stepExprType != fromExprType)
				throw AstError{ "numerical for step expression type must match from expression type" };
		}


		AttributeValue<LoopUnroll> unrollValue;
		if (node.unroll.HasValue())
		{
			unrollValue = ComputeAttributeValue(node.unroll);
			if (unrollValue.GetResultingValue() == LoopUnroll::Always)
			{
				PushScope();

				auto multi = std::make_unique<MultiStatement>();

				auto Unroll = [&](auto dummy)
				{
					using T = std::decay_t<decltype(dummy)>;

					T counter = std::get<T>(ComputeConstantValue(*fromExpr));
					T to = std::get<T>(ComputeConstantValue(*toExpr));
					T step = (stepExpr) ? std::get<T>(ComputeConstantValue(*stepExpr)) : T(1);

					for (; counter < to; counter += step)
					{
						auto var = ShaderBuilder::DeclareVariable(node.varName, ShaderBuilder::Constant(counter));
						Validate(*var);
						multi->statements.emplace_back(std::move(var));

						multi->statements.emplace_back(CloneStatement(node.statement));
					}
				};

				switch (fromType)
				{
					case PrimitiveType::Int32:
						Unroll(Int32{});
						break;

					case PrimitiveType::UInt32:
						Unroll(UInt32{});
						break;

					default:
						throw AstError{ "internal error" };
				}

				PopScope();

				return multi;
			}
		}

		if (m_context->options.reduceLoopsToWhile)
		{
			PushScope();

			auto multi = std::make_unique<MultiStatement>();

			// Counter variable
			auto counterVariable = ShaderBuilder::DeclareVariable(node.varName, std::move(fromExpr));
			Validate(*counterVariable);

			std::size_t counterVarIndex = counterVariable->varIndex.value();
			multi->statements.emplace_back(std::move(counterVariable));

			// Target variable
			auto targetVariable = ShaderBuilder::DeclareVariable("to", std::move(toExpr));
			Validate(*targetVariable);

			std::size_t targetVarIndex = targetVariable->varIndex.value();
			multi->statements.emplace_back(std::move(targetVariable));

			// Step variable
			std::optional<std::size_t> stepVarIndex;

			if (stepExpr)
			{
				auto stepVariable = ShaderBuilder::DeclareVariable("step", std::move(stepExpr));
				Validate(*stepVariable);

				stepVarIndex = stepVariable->varIndex;
				multi->statements.emplace_back(std::move(stepVariable));
			}

			// While
			auto whileStatement = std::make_unique<WhileStatement>();
			whileStatement->unroll = std::move(unrollValue);

			// While condition
			auto condition = ShaderBuilder::Binary(BinaryType::CompLt, ShaderBuilder::Variable(counterVarIndex, fromType), ShaderBuilder::Variable(targetVarIndex, fromType));
			Validate(*condition);

			whileStatement->condition = std::move(condition);

			// While body
			auto body = std::make_unique<MultiStatement>();
			body->statements.reserve(2);

			body->statements.emplace_back(CloneStatement(node.statement));

			ExpressionPtr incrExpr;
			if (stepVarIndex)
				incrExpr = ShaderBuilder::Variable(*stepVarIndex, fromType);
			else
				incrExpr = (fromType == PrimitiveType::Int32) ? ShaderBuilder::Constant(1) : ShaderBuilder::Constant(1u);

			auto incrCounter = ShaderBuilder::Assign(AssignType::CompoundAdd, ShaderBuilder::Variable(counterVarIndex, fromType), std::move(incrExpr));
			Validate(*incrCounter);

			body->statements.emplace_back(ShaderBuilder::ExpressionStatement(std::move(incrCounter)));

			whileStatement->body = std::move(body);

			multi->statements.emplace_back(std::move(whileStatement));

			PopScope();

			return multi;
		}
		else
		{
			auto clone = std::make_unique<ForStatement>();
			clone->fromExpr = std::move(fromExpr);
			clone->stepExpr = std::move(stepExpr);
			clone->toExpr = std::move(toExpr);
			clone->varName = node.varName;
			clone->unroll = std::move(unrollValue);

			PushScope();
			{
				clone->varIndex = RegisterVariable(node.varName, fromExprType);
				clone->statement = CloneStatement(node.statement);
			}
			PopScope();

			SanitizeIdentifier(clone->varName);

			return clone;
		}
	}

	StatementPtr SanitizeVisitor::Clone(ForEachStatement& node)
	{
		auto expr = CloneExpression(MandatoryExpr(node.expression));

		if (node.varName.empty())
			throw AstError{ "for-each variable name cannot be empty"};

		const ExpressionType& exprType = GetExpressionType(*expr);
		ExpressionType innerType;
		if (IsArrayType(exprType))
		{
			const ArrayType& arrayType = std::get<ArrayType>(exprType);
			innerType = arrayType.containedType->type;
		}
		else
			throw AstError{ "for-each is only supported on arrays and range expressions" };

		AttributeValue<LoopUnroll> unrollValue;
		if (node.unroll.HasValue())
		{
			unrollValue = ComputeAttributeValue(node.unroll);
			if (unrollValue.GetResultingValue() == LoopUnroll::Always)
			{
				PushScope();

				// Repeat code
				auto multi = std::make_unique<MultiStatement>();
				if (IsArrayType(exprType))
				{
					const ArrayType& arrayType = std::get<ArrayType>(exprType);
					UInt32 length = arrayType.length.GetResultingValue();

					for (UInt32 i = 0; i < length; ++i)
					{
						auto accessIndex = ShaderBuilder::AccessIndex(CloneExpression(expr), ShaderBuilder::Constant(i));
						Validate(*accessIndex);

						auto elementVariable = ShaderBuilder::DeclareVariable(node.varName, std::move(accessIndex));
						Validate(*elementVariable);

						multi->statements.emplace_back(std::move(elementVariable));
						multi->statements.emplace_back(CloneStatement(node.statement));
					}
				}

				PopScope();

				return multi;
			}
		}

		if (m_context->options.reduceLoopsToWhile)
		{
			PushScope();

			auto multi = std::make_unique<MultiStatement>();

			if (IsArrayType(exprType))
			{
				const ArrayType& arrayType = std::get<ArrayType>(exprType);
				UInt32 length = arrayType.length.GetResultingValue();

				multi->statements.reserve(2);

				// Counter variable
				auto counterVariable = ShaderBuilder::DeclareVariable("i", ShaderBuilder::Constant(0u));
				Validate(*counterVariable);

				std::size_t counterVarIndex = counterVariable->varIndex.value();

				multi->statements.emplace_back(std::move(counterVariable));

				auto whileStatement = std::make_unique<WhileStatement>();
				whileStatement->unroll = std::move(unrollValue);

				// While condition
				auto condition = ShaderBuilder::Binary(BinaryType::CompLt, ShaderBuilder::Variable(counterVarIndex, PrimitiveType::UInt32), ShaderBuilder::Constant(length));
				Validate(*condition);
				whileStatement->condition = std::move(condition);

				// While body
				auto body = std::make_unique<MultiStatement>();
				body->statements.reserve(3);

				auto accessIndex = ShaderBuilder::AccessIndex(std::move(expr), ShaderBuilder::Variable(counterVarIndex, PrimitiveType::UInt32));
				Validate(*accessIndex);

				auto elementVariable = ShaderBuilder::DeclareVariable(node.varName, std::move(accessIndex));
				Validate(*elementVariable);
				body->statements.emplace_back(std::move(elementVariable));

				body->statements.emplace_back(CloneStatement(node.statement));

				auto incrCounter = ShaderBuilder::Assign(AssignType::CompoundAdd, ShaderBuilder::Variable(counterVarIndex, PrimitiveType::UInt32), ShaderBuilder::Constant(1u));
				Validate(*incrCounter);

				body->statements.emplace_back(ShaderBuilder::ExpressionStatement(std::move(incrCounter)));

				whileStatement->body = std::move(body);

				multi->statements.emplace_back(std::move(whileStatement));
			}

			PopScope();

			return multi;
		}
		else
		{
			auto clone = std::make_unique<ForEachStatement>();
			clone->expression = std::move(expr);
			clone->varName = node.varName;
			clone->unroll = std::move(unrollValue);

			PushScope();
			{
				clone->varIndex = RegisterVariable(node.varName, innerType);
				clone->statement = CloneStatement(node.statement);
			}
			PopScope();

			SanitizeIdentifier(clone->varName);

			return clone;
		}
	}

	StatementPtr SanitizeVisitor::Clone(MultiStatement& node)
	{
		PushScope();

		auto clone = std::make_unique<MultiStatement>();
		clone->statements.reserve(node.statements.size());

		std::vector<StatementPtr>* previousList = m_context->currentStatementList;
		m_context->currentStatementList = &clone->statements;

		for (auto& statement : node.statements)
			clone->statements.push_back(AstCloner::Clone(MandatoryStatement(statement)));

		m_context->currentStatementList = previousList;

		PopScope();

		return clone;
	}

	StatementPtr SanitizeVisitor::Clone(WhileStatement& node)
	{
		MandatoryExpr(node.condition);
		MandatoryStatement(node.body);

		auto clone = static_unique_pointer_cast<WhileStatement>(AstCloner::Clone(node));
		Validate(*clone);

		AttributeValue<LoopUnroll> unrollValue;
		if (node.unroll.HasValue())
		{
			clone->unroll = ComputeAttributeValue(node.unroll);
			if (clone->unroll.GetResultingValue() == LoopUnroll::Always)
				throw AstError{ "unroll(always) is not yet supported on while" };
		}

		return clone;
	}

	auto SanitizeVisitor::FindIdentifier(const std::string_view& identifierName) const -> const Identifier*
	{
		auto it = std::find_if(m_context->identifiersInScope.rbegin(), m_context->identifiersInScope.rend(), [&](const Identifier& identifier) { return identifier.name == identifierName; });
		if (it == m_context->identifiersInScope.rend())
			return nullptr;

		return &*it;
	}

	Expression& SanitizeVisitor::MandatoryExpr(const ExpressionPtr& node)
	{
		if (!node)
			throw AstError{ "Invalid expression" };

		return *node;
	}

	Statement& SanitizeVisitor::MandatoryStatement(const StatementPtr& node)
	{
		if (!node)
			throw AstError{ "Invalid statement" };

		return *node;
	}

	void SanitizeVisitor::PushScope()
	{
		m_context->scopeSizes.push_back(m_context->identifiersInScope.size());
	}

	void SanitizeVisitor::PopScope()
	{
		assert(!m_context->scopeSizes.empty());
		m_context->identifiersInScope.resize(m_context->scopeSizes.back());
		m_context->scopeSizes.pop_back();
	}

	ExpressionPtr SanitizeVisitor::CacheResult(ExpressionPtr expression)
	{
		// No need to cache LValues (variables/constants) (TODO: Improve this, as constants doens't need to be cached as well)
		if (GetExpressionCategory(*expression) == ExpressionCategory::LValue)
			return expression;

		assert(m_context->currentStatementList);

		auto variableDeclaration = ShaderBuilder::DeclareVariable("cachedResult", std::move(expression)); //< Validation will prevent name-clash if required
		Validate(*variableDeclaration);

		auto varExpr = std::make_unique<VariableExpression>();
		varExpr->variableId = *variableDeclaration->varIndex;

		m_context->currentStatementList->push_back(std::move(variableDeclaration));

		return varExpr;
	}

	template<typename T>
	const T& SanitizeVisitor::ComputeAttributeValue(AttributeValue<T>& attribute)
	{
		if (!attribute.HasValue())
			throw AstError{"attribute expected a value"};

		if (attribute.IsExpression())
		{
			ConstantValue value = ComputeConstantValue(*attribute.GetExpression());
			if constexpr (TypeListFind<ConstantTypes, T>)
			{
				if (!std::holds_alternative<T>(value))
				{
					// HAAAAAX
					if (std::holds_alternative<Int32>(value) && std::is_same_v<T, UInt32>)
						attribute = static_cast<UInt32>(std::get<Int32>(value));
					else
						throw AstError{ "unexpected attribute type" };
				}
				else
					attribute = std::get<T>(value);
			}
			else
				throw AstError{ "unexpected expression for this type" };
		}

		assert(attribute.IsResultingValue());
		return attribute.GetResultingValue();
	}

	ConstantValue SanitizeVisitor::ComputeConstantValue(Expression& expr)
	{
		// Run optimizer on constant value to hopefully retrieve a single constant value
		ExpressionPtr optimizedExpr = Optimize(expr);
		if (optimizedExpr->GetType() != NodeType::ConstantValueExpression)
			throw AstError{"expected a constant expression"};

		return static_cast<ConstantValueExpression&>(*optimizedExpr).value;
	}

	template<typename T>
	std::unique_ptr<T> SanitizeVisitor::Optimize(T& node)
	{
		AstOptimizer::Options optimizerOptions;
		optimizerOptions.constantQueryCallback = [this](std::size_t constantId) -> const ConstantValue&
		{
			assert(constantId < m_context->constantValues.size());
			return m_context->constantValues[constantId];
		};

		// Run optimizer on constant value to hopefully retrieve a single constant value
		return static_unique_pointer_cast<T>(ShaderAst::Optimize(node, optimizerOptions));
	}

	std::size_t SanitizeVisitor::DeclareFunction(DeclareFunctionStatement& funcDecl)
	{
		std::size_t functionIndex = m_context->functions.size();
		auto& funcData = m_context->functions.emplace_back();
		funcData.node = &funcDecl;

		return functionIndex;
	}

	void SanitizeVisitor::PropagateFunctionFlags(std::size_t funcIndex, FunctionFlags flags, Bitset<>& seen)
	{
		assert(funcIndex < m_context->functions.size());
		auto& funcData = m_context->functions[funcIndex];
		if (!funcData.defined)
			return;

		funcData.flags |= flags;

		for (std::size_t i = funcData.calledByFunctions.FindFirst(); i != funcData.calledByFunctions.npos; i = funcData.calledByFunctions.FindNext(i))
			PropagateFunctionFlags(i, funcData.flags, seen);
	}
	
	std::size_t SanitizeVisitor::RegisterConstant(std::string name, ConstantValue value)
	{
		if (FindIdentifier(name))
			throw AstError{ name + " is already used" };

		std::size_t constantIndex = m_context->constantValues.size();
		m_context->constantValues.emplace_back(std::move(value));

		m_context->identifiersInScope.push_back({
			std::move(name),
			constantIndex,
			Identifier::Type::Constant
		});

		return constantIndex;
	}

	auto SanitizeVisitor::RegisterFunction(std::size_t functionIndex) -> FunctionData&
	{
		assert(m_context->functions.size() >= functionIndex);
		auto& funcData = m_context->functions[functionIndex];
		assert(!funcData.defined);
		funcData.defined = true;

		if (auto* identifier = FindIdentifier(funcData.node->name))
		{
			bool duplicate = true;

			// Functions cannot be declared twice, except for entry ones if their stages are different
			if (funcData.node->entryStage.HasValue() && identifier->type == Identifier::Type::Function)
			{
				auto& otherFunction = m_context->functions[identifier->index];
				if (funcData.node->entryStage.GetResultingValue() != otherFunction.node->entryStage.GetResultingValue())
					duplicate = false;
			}

			if (duplicate)
				throw AstError{ funcData.node->name + " is already used" };
		}

		m_context->identifiersInScope.push_back({
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

		std::size_t intrinsicIndex = m_context->intrinsics.size();
		m_context->intrinsics.push_back(type);

		m_context->identifiersInScope.push_back({
			std::move(name),
			intrinsicIndex,
			Identifier::Type::Intrinsic
		});

		return intrinsicIndex;
	}

	std::size_t SanitizeVisitor::RegisterStruct(std::string name, StructDescription* description)
	{
		if (FindIdentifier(name))
			throw AstError{ name + " is already used" };

		std::size_t structIndex = m_context->structs.size();
		m_context->structs.emplace_back(description);

		m_context->identifiersInScope.push_back({
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

		std::size_t varIndex = m_context->variableTypes.size();
		m_context->variableTypes.emplace_back(std::move(type));

		m_context->identifiersInScope.push_back({
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
		for (std::size_t funcIndex = 0; funcIndex < m_context->functions.size(); ++funcIndex)
		{
			auto& funcData = m_context->functions[funcIndex];

			PropagateFunctionFlags(funcIndex, funcData.flags, seen);
			seen.Clear();
		}

		for (const FunctionData& funcData : m_context->functions)
		{
			if (funcData.flags.Test(ShaderAst::FunctionFlag::DoesDiscard) && funcData.node->entryStage.HasValue() && funcData.node->entryStage.GetResultingValue() != ShaderStageType::Fragment)
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
			                   std::is_same_v<T, ArrayType> ||
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
			else if constexpr (std::is_same_v<T, ArrayType>)
			{
				ArrayType resolvedArrayType;
				if (arg.length.IsExpression())
				{
					resolvedArrayType.length = CloneExpression(arg.length.GetExpression());
					ComputeAttributeValue(resolvedArrayType.length);
				}
				else if (arg.length.IsResultingValue())
					resolvedArrayType.length = arg.length.GetResultingValue();

				resolvedArrayType.containedType = std::make_unique<ContainedType>();
				resolvedArrayType.containedType->type = ResolveType(arg.containedType->type);

				return resolvedArrayType;
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

	void SanitizeVisitor::Validate(WhileStatement& node)
	{
		if (GetExpressionType(*node.condition) != ExpressionType{ PrimitiveType::Boolean })
			throw AstError{ "expected a boolean value" };
	}

	void SanitizeVisitor::Validate(AccessIndexExpression& node)
	{
		if (node.indices.empty())
			throw AstError{ "AccessIndexExpression must have at least one index" };

		for (auto& index : node.indices)
		{
			const ShaderAst::ExpressionType& indexType = GetExpressionType(*index);
			if (!IsPrimitiveType(indexType))
				throw AstError{ "AccessIndex expects integer indices" };

			PrimitiveType primitiveIndexType = std::get<PrimitiveType>(indexType);
			if (primitiveIndexType != PrimitiveType::Int32 && primitiveIndexType != PrimitiveType::UInt32)
				throw AstError{ "AccessIndex expects integer indices" };
		}

		ExpressionType exprType = GetExpressionType(*node.expr);
		for (const auto& indexExpr : node.indices)
		{
			if (IsArrayType(exprType))
			{
				const ArrayType& arrayType = std::get<ArrayType>(exprType);
				ExpressionType containedType = arrayType.containedType->type; //< Don't overwrite exprType directly since it contains arrayType
				exprType = std::move(containedType);
			}
			else if (IsStructType(exprType))
			{
				const ShaderAst::ExpressionType& indexType = GetExpressionType(*indexExpr);
				if (indexExpr->GetType() != NodeType::ConstantValueExpression || indexType != ExpressionType{ PrimitiveType::Int32 })
					throw AstError{ "struct can only be accessed with constant i32 indices" };

				ConstantValueExpression& constantExpr = static_cast<ConstantValueExpression&>(*indexExpr);

				Int32 index = std::get<Int32>(constantExpr.value);

				std::size_t structIndex = ResolveStruct(exprType);
				assert(structIndex < m_context->structs.size());
				const StructDescription* s = m_context->structs[structIndex];

				exprType = ResolveType(s->members[index].type);
			}
			else if (IsMatrixType(exprType))
			{
				// Matrix index (ex: mat[2])
				MatrixType matrixType = std::get<MatrixType>(exprType);

				//TODO: Handle row-major matrices
				exprType = VectorType{ matrixType.rowCount, matrixType.type };
			}
			else if (IsVectorType(exprType))
			{
				// Swizzle expression with one component (ex: vec[2])
				VectorType swizzledVec = std::get<VectorType>(exprType);

				exprType = swizzledVec.type;
			}
			else
				throw AstError{ "unexpected type (only struct, vectors and matrices can be indexed)" }; //< TODO: Add support for arrays
		}

		node.cachedExpressionType = std::move(exprType);
	}

	void SanitizeVisitor::Validate(AssignExpression& node)
	{
		if (GetExpressionCategory(*node.left) != ExpressionCategory::LValue)
			throw AstError{ "Assignation is only possible with a l-value" };

		std::optional<BinaryType> binaryType;
		switch (node.op)
		{
			case AssignType::Simple:
				TypeMustMatch(node.left, node.right);
				break;

			case AssignType::CompoundAdd: binaryType = BinaryType::Add; break;
			case AssignType::CompoundDivide: binaryType = BinaryType::Divide; break;
			case AssignType::CompoundMultiply: binaryType = BinaryType::Multiply; break;
			case AssignType::CompoundLogicalAnd: binaryType = BinaryType::LogicalAnd; break;
			case AssignType::CompoundLogicalOr: binaryType = BinaryType::LogicalOr; break;
			case AssignType::CompoundSubtract: binaryType = BinaryType::Subtract; break;
		}

		if (binaryType)
		{
			ExpressionType expressionType = ValidateBinaryOp(*binaryType, node.left, node.right);
			TypeMustMatch(GetExpressionType(*node.left), expressionType);

			if (m_context->options.removeCompoundAssignments)
			{
				node.op = AssignType::Simple;
				node.right = ShaderBuilder::Binary(*binaryType, AstCloner::Clone(*node.left), std::move(node.right));
				node.right->cachedExpressionType = std::move(expressionType);
			}
		}

		node.cachedExpressionType = GetExpressionType(*node.left);
	}

	void SanitizeVisitor::Validate(BinaryExpression& node)
	{
		node.cachedExpressionType = ValidateBinaryOp(node.op, node.left, node.right);
	}

	void SanitizeVisitor::Validate(CallFunctionExpression& node, const DeclareFunctionStatement* referenceDeclaration)
	{
		if (referenceDeclaration->entryStage.HasValue())
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

	void SanitizeVisitor::Validate(CastExpression& node)
	{
		node.targetType = ResolveType(node.targetType);
		node.cachedExpressionType = node.targetType;

		const auto& firstExprPtr = node.expressions.front();
		if (!firstExprPtr)
			throw AstError{ "expected at least one expression" };

		if (IsMatrixType(node.targetType))
		{
			const MatrixType& targetMatrixType = std::get<MatrixType>(node.targetType);

			const ExpressionType& firstExprType = GetExpressionType(*firstExprPtr);
			if (IsMatrixType(firstExprType))
			{
				if (node.expressions[1])
					throw AstError{ "too many expressions" };

				// Matrix to matrix cast: always valid
				return;
			}
			else
			{
				assert(targetMatrixType.columnCount <= 4);
				for (std::size_t i = 0; i < targetMatrixType.columnCount; ++i)
				{
					const auto& exprPtr = node.expressions[i];
					if (!exprPtr)
						throw AstError{ "component count doesn't match required component count" };

					const ExpressionType& exprType = GetExpressionType(*exprPtr);
					if (!IsVectorType(exprType))
						throw AstError{ "expected vector type" };

					const VectorType& vecType = std::get<VectorType>(exprType);
					if (vecType.componentCount != targetMatrixType.rowCount)
						throw AstError{ "vector component count must match target matrix row count" };
				}
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
		std::size_t requiredComponents = GetComponentCount(node.targetType);

		for (auto& exprPtr : node.expressions)
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
	}

	void SanitizeVisitor::Validate(DeclareVariableStatement& node)
	{
		if (IsNoType(node.varType))
		{
			if (!node.initialExpression)
				throw AstError{ "variable must either have a type or an initial value" };

			node.varType = ResolveType(GetExpressionType(*node.initialExpression));
		}
		else
			node.varType = ResolveType(node.varType);

		if (m_context->options.makeVariableNameUnique && FindIdentifier(node.varName) != nullptr)
		{
			// Try to make variable name unique by appending _X to its name (incrementing X until it's unique) to the variable name until by incrementing X
			unsigned int cloneIndex = 2;
			std::string candidateName;
			do
			{
				candidateName = node.varName + "_" + std::to_string(cloneIndex++);
			}
			while (FindIdentifier(candidateName) != nullptr);

			node.varName = std::move(candidateName);
		}

		node.varIndex = RegisterVariable(node.varName, node.varType);

		SanitizeIdentifier(node.varName);
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
			case IntrinsicType::Reflect:
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

			case IntrinsicType::Exp:
			{
				if (node.parameters.size() != 1)
					throw AstError{ "Expected only one parameters" };

				MandatoryExpr(node.parameters.front());
				break;
			}

			case IntrinsicType::Length:
			case IntrinsicType::Normalize:
			{
				if (node.parameters.size() != 1)
					throw AstError{ "Expected only one parameters" };

				const ExpressionType& type = GetExpressionType(MandatoryExpr(node.parameters.front()));
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
					throw AstError{ "CrossProduct only works with vec3[f32] expressions" };

				node.cachedExpressionType = type;
				break;
			}

			case IntrinsicType::DotProduct:
			case IntrinsicType::Length:
			{
				const ExpressionType& type = GetExpressionType(*node.parameters.front());
				if (!IsVectorType(type))
					throw AstError{ "DotProduct expects vector types" }; //< FIXME

				node.cachedExpressionType = std::get<VectorType>(type).type;
				break;
			}

			case IntrinsicType::Normalize:
			case IntrinsicType::Reflect:
			{
				const ExpressionType& type = GetExpressionType(*node.parameters.front());
				if (!IsVectorType(type))
					throw AstError{ "DotProduct expects vector types" }; //< FIXME

				node.cachedExpressionType = type;
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

			case IntrinsicType::Exp:
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

	void SanitizeVisitor::Validate(SwizzleExpression& node)
	{
		MandatoryExpr(node.expression);
		const ExpressionType& exprType = GetExpressionType(*node.expression);
		if (!IsPrimitiveType(exprType) && !IsVectorType(exprType))
			throw AstError{ "Cannot swizzle this type" };

		PrimitiveType baseType;
		std::size_t componentCount;
		if (IsPrimitiveType(exprType))
		{
			baseType = std::get<PrimitiveType>(exprType);
			componentCount = 1;
		}
		else
		{
			const VectorType& vecType = std::get<VectorType>(exprType);
			baseType = vecType.type;
			componentCount = vecType.componentCount;
		}

		if (node.componentCount > 4)
			throw AstError{ "cannot swizzle more than four elements" };

		for (std::size_t i = 0; i < node.componentCount; ++i)
		{
			if (node.components[i] >= componentCount)
				throw AstError{ "invalid swizzle" };
		}

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
	
	void SanitizeVisitor::Validate(UnaryExpression& node)
	{
		const ExpressionType& exprType = GetExpressionType(MandatoryExpr(node.expression));

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

		node.cachedExpressionType = exprType;
	}

	void SanitizeVisitor::Validate(VariableExpression& node)
	{
		if (node.variableId >= m_context->variableTypes.size())
			throw AstError{ "invalid constant index " + std::to_string(node.variableId) };

		node.cachedExpressionType = m_context->variableTypes[node.variableId];
	}

	ExpressionType SanitizeVisitor::ValidateBinaryOp(BinaryType op, const ExpressionPtr& leftExpr, const ExpressionPtr& rightExpr)
	{
		const ExpressionType& leftExprType = GetExpressionType(MandatoryExpr(leftExpr));
		const ExpressionType& rightExprType = GetExpressionType(MandatoryExpr(rightExpr));

		if (!IsPrimitiveType(leftExprType) && !IsMatrixType(leftExprType) && !IsVectorType(leftExprType))
			throw AstError{ "left expression type does not support binary operation" };

		if (!IsPrimitiveType(rightExprType) && !IsMatrixType(rightExprType) && !IsVectorType(rightExprType))
			throw AstError{ "right expression type does not support binary operation" };

		if (IsPrimitiveType(leftExprType))
		{
			PrimitiveType leftType = std::get<PrimitiveType>(leftExprType);
			switch (op)
			{
				case BinaryType::CompGe:
				case BinaryType::CompGt:
				case BinaryType::CompLe:
				case BinaryType::CompLt:
					if (leftType == PrimitiveType::Boolean)
						throw AstError{ "this operation is not supported for booleans" };

					[[fallthrough]];
				case BinaryType::CompEq:
				case BinaryType::CompNe:
				{
					TypeMustMatch(leftExpr, rightExpr);
					return PrimitiveType::Boolean;
				}

				case BinaryType::Add:
				case BinaryType::Subtract:
					TypeMustMatch(leftExpr, rightExpr);
					return leftExprType;

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
								return rightExprType;
							}
							else if (IsPrimitiveType(rightExprType))
							{
								TypeMustMatch(leftType, rightExprType);
								return leftExprType;
							}
							else if (IsVectorType(rightExprType))
							{
								TypeMustMatch(leftType, std::get<VectorType>(rightExprType).type);
								return rightExprType;
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

				case BinaryType::LogicalAnd:
				case BinaryType::LogicalOr:
				{
					if (leftType != PrimitiveType::Boolean)
						throw AstError{ "logical and/or are only supported on booleans" };

					TypeMustMatch(leftExpr, rightExpr);
					return PrimitiveType::Boolean;
				}
			}
		}
		else if (IsMatrixType(leftExprType))
		{
			const MatrixType& leftType = std::get<MatrixType>(leftExprType);
			switch (op)
			{
				case BinaryType::CompGe:
				case BinaryType::CompGt:
				case BinaryType::CompLe:
				case BinaryType::CompLt:
				case BinaryType::CompEq:
				case BinaryType::CompNe:
					TypeMustMatch(leftExpr, rightExpr);
					return PrimitiveType::Boolean;

				case BinaryType::Add:
				case BinaryType::Subtract:
					TypeMustMatch(leftExpr, rightExpr);
					return leftExprType;

				case BinaryType::Multiply:
				case BinaryType::Divide:
				{
					if (IsMatrixType(rightExprType))
					{
						TypeMustMatch(leftExprType, rightExprType);
						return leftExprType; //< FIXME
					}
					else if (IsPrimitiveType(rightExprType))
					{
						TypeMustMatch(leftType.type, rightExprType);
						return leftExprType;
					}
					else if (IsVectorType(rightExprType))
					{
						const VectorType& rightType = std::get<VectorType>(rightExprType);
						TypeMustMatch(leftType.type, rightType.type);

						if (leftType.columnCount != rightType.componentCount)
							throw AstError{ "incompatible types" };

						return rightExprType;
					}
					else
						throw AstError{ "incompatible types" };
				}

				case BinaryType::LogicalAnd:
				case BinaryType::LogicalOr:
					throw AstError{ "logical and/or are only supported on booleans" };
			}
		}
		else if (IsVectorType(leftExprType))
		{
			const VectorType& leftType = std::get<VectorType>(leftExprType);
			switch (op)
			{
				case BinaryType::CompGe:
				case BinaryType::CompGt:
				case BinaryType::CompLe:
				case BinaryType::CompLt:
				case BinaryType::CompEq:
				case BinaryType::CompNe:
					TypeMustMatch(leftExpr, rightExpr);
					return PrimitiveType::Boolean;

				case BinaryType::Add:
				case BinaryType::Subtract:
					TypeMustMatch(leftExpr, rightExpr);
					return leftExprType;

				case BinaryType::Multiply:
				case BinaryType::Divide:
				{
					if (IsPrimitiveType(rightExprType))
					{
						TypeMustMatch(leftType.type, rightExprType);
						return leftExprType;
					}
					else if (IsVectorType(rightExprType))
					{
						TypeMustMatch(leftType, rightExprType);
						return rightExprType;
					}
					else
						throw AstError{ "incompatible types" };

					break;
				}

				case BinaryType::LogicalAnd:
				case BinaryType::LogicalOr:
					throw AstError{ "logical and/or are only supported on booleans" };
			}
		}

		throw AstError{ "internal error: unchecked operation" };
	}

	void SanitizeVisitor::TypeMustMatch(const ExpressionPtr& left, const ExpressionPtr& right)
	{
		return TypeMustMatch(GetExpressionType(*left), GetExpressionType(*right));
	}

	void SanitizeVisitor::TypeMustMatch(const ExpressionType& left, const ExpressionType& right)
	{
		if (left != right)
			throw AstError{ "Left expression type must match right expression type" };
	}
}
