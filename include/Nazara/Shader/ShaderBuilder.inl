// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderBuilder.hpp>
#include <stdexcept>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderBuilder
{
	inline std::unique_ptr<ShaderAst::AccessIdentifierExpression> Impl::AccessMember::operator()(ShaderAst::ExpressionPtr expr, std::vector<std::string> memberIdentifiers) const
	{
		auto accessMemberNode = std::make_unique<ShaderAst::AccessIdentifierExpression>();
		accessMemberNode->expr = std::move(expr);
		accessMemberNode->identifiers = std::move(memberIdentifiers);

		return accessMemberNode;
	}

	inline std::unique_ptr<ShaderAst::AccessIndexExpression> Impl::AccessIndex::operator()(ShaderAst::ExpressionPtr expr, Int32 index) const
	{
		auto accessMemberNode = std::make_unique<ShaderAst::AccessIndexExpression>();
		accessMemberNode->expr = std::move(expr);
		accessMemberNode->indices.push_back(ShaderBuilder::Constant(index));

		return accessMemberNode;
	}

	inline std::unique_ptr<ShaderAst::AccessIndexExpression> Impl::AccessIndex::operator()(ShaderAst::ExpressionPtr expr, const std::vector<Int32>& indexConstants) const
	{
		auto accessMemberNode = std::make_unique<ShaderAst::AccessIndexExpression>();
		accessMemberNode->expr = std::move(expr);

		accessMemberNode->indices.reserve(indexConstants.size());
		for (Int32 index : indexConstants)
			accessMemberNode->indices.push_back(ShaderBuilder::Constant(index));

		return accessMemberNode;
	}

	inline std::unique_ptr<ShaderAst::AccessIndexExpression> Impl::AccessIndex::operator()(ShaderAst::ExpressionPtr expr, ShaderAst::ExpressionPtr indexExpression) const
	{
		auto accessMemberNode = std::make_unique<ShaderAst::AccessIndexExpression>();
		accessMemberNode->expr = std::move(expr);
		accessMemberNode->indices.push_back(std::move(indexExpression));

		return accessMemberNode;
	}

	inline std::unique_ptr<ShaderAst::AccessIndexExpression> Impl::AccessIndex::operator()(ShaderAst::ExpressionPtr expr, std::vector<ShaderAst::ExpressionPtr> indexExpressions) const
	{
		auto accessMemberNode = std::make_unique<ShaderAst::AccessIndexExpression>();
		accessMemberNode->expr = std::move(expr);
		accessMemberNode->indices = std::move(indexExpressions);

		return accessMemberNode;
	}

	inline std::unique_ptr<ShaderAst::AssignExpression> Impl::Assign::operator()(ShaderAst::AssignType op, ShaderAst::ExpressionPtr left, ShaderAst::ExpressionPtr right) const
	{
		auto assignNode = std::make_unique<ShaderAst::AssignExpression>();
		assignNode->op = op;
		assignNode->left = std::move(left);
		assignNode->right = std::move(right);

		return assignNode;
	}

	inline std::unique_ptr<ShaderAst::BinaryExpression> Impl::Binary::operator()(ShaderAst::BinaryType op, ShaderAst::ExpressionPtr left, ShaderAst::ExpressionPtr right) const
	{
		auto binaryNode = std::make_unique<ShaderAst::BinaryExpression>();
		binaryNode->op = op;
		binaryNode->left = std::move(left);
		binaryNode->right = std::move(right);

		return binaryNode;
	}

	template<bool Const>
	std::unique_ptr<ShaderAst::BranchStatement> Impl::Branch<Const>::operator()(ShaderAst::ExpressionPtr condition, ShaderAst::StatementPtr truePath, ShaderAst::StatementPtr falsePath) const
	{
		auto branchNode = std::make_unique<ShaderAst::BranchStatement>();

		auto& condStatement = branchNode->condStatements.emplace_back();
		condStatement.condition = std::move(condition);
		condStatement.statement = std::move(truePath);

		branchNode->elseStatement = std::move(falsePath);
		branchNode->isConst = Const;

		return branchNode;
	}

	template<bool Const>
	std::unique_ptr<ShaderAst::BranchStatement> Impl::Branch<Const>::operator()(std::vector<ShaderAst::BranchStatement::ConditionalStatement> condStatements, ShaderAst::StatementPtr elseStatement) const
	{
		auto branchNode = std::make_unique<ShaderAst::BranchStatement>();
		branchNode->condStatements = std::move(condStatements);
		branchNode->elseStatement = std::move(elseStatement);
		branchNode->isConst = Const;

		return branchNode;
	}

	inline std::unique_ptr<ShaderAst::CallFunctionExpression> Impl::CallFunction::operator()(std::string functionName, std::vector<ShaderAst::ExpressionPtr> parameters) const
	{
		auto callFunctionExpression = std::make_unique<ShaderAst::CallFunctionExpression>();
		callFunctionExpression->targetFunction = ShaderBuilder::Identifier(std::move(functionName));
		callFunctionExpression->parameters = std::move(parameters);

		return callFunctionExpression;
	}

	inline std::unique_ptr<ShaderAst::CallFunctionExpression> Impl::CallFunction::operator()(ShaderAst::ExpressionPtr functionExpr, std::vector<ShaderAst::ExpressionPtr> parameters) const
	{
		auto callFunctionExpression = std::make_unique<ShaderAst::CallFunctionExpression>();
		callFunctionExpression->targetFunction = std::move(functionExpr);
		callFunctionExpression->parameters = std::move(parameters);

		return callFunctionExpression;
	}

	inline std::unique_ptr<ShaderAst::CastExpression> Impl::Cast::operator()(ShaderAst::ExpressionValue<ShaderAst::ExpressionType> targetType, ShaderAst::ExpressionPtr expression) const
	{
		auto castNode = std::make_unique<ShaderAst::CastExpression>();
		castNode->targetType = std::move(targetType);
		castNode->expressions[0] = std::move(expression);

		return castNode;
	}

	inline std::unique_ptr<ShaderAst::CastExpression> Impl::Cast::operator()(ShaderAst::ExpressionValue<ShaderAst::ExpressionType> targetType, std::array<ShaderAst::ExpressionPtr, 4> expressions) const
	{
		auto castNode = std::make_unique<ShaderAst::CastExpression>();
		castNode->expressions = std::move(expressions);
		castNode->targetType = std::move(targetType);

		return castNode;
	}

	inline std::unique_ptr<ShaderAst::CastExpression> Impl::Cast::operator()(ShaderAst::ExpressionValue<ShaderAst::ExpressionType> targetType, std::vector<ShaderAst::ExpressionPtr> expressions) const
	{
		auto castNode = std::make_unique<ShaderAst::CastExpression>();
		castNode->targetType = std::move(targetType);

		assert(expressions.size() <= castNode->expressions.size());
		for (std::size_t i = 0; i < expressions.size(); ++i)
			castNode->expressions[i] = std::move(expressions[i]);

		return castNode;
	}

	inline std::unique_ptr<ShaderAst::ConditionalExpression> Impl::ConditionalExpression::operator()(ShaderAst::ExpressionPtr condition, ShaderAst::ExpressionPtr truePath, ShaderAst::ExpressionPtr falsePath) const
	{
		auto condExprNode = std::make_unique<ShaderAst::ConditionalExpression>();
		condExprNode->condition = std::move(condition);
		condExprNode->falsePath = std::move(falsePath);
		condExprNode->truePath = std::move(truePath);

		return condExprNode;
	}

	inline std::unique_ptr<ShaderAst::ConditionalStatement> Impl::ConditionalStatement::operator()(ShaderAst::ExpressionPtr condition, ShaderAst::StatementPtr statement) const
	{
		auto condStatementNode = std::make_unique<ShaderAst::ConditionalStatement>();
		condStatementNode->condition = std::move(condition);
		condStatementNode->statement = std::move(statement);

		return condStatementNode;
	}

	inline std::unique_ptr<ShaderAst::ConstantValueExpression> Impl::Constant::operator()(ShaderAst::ConstantValue value) const
	{
		auto constantNode = std::make_unique<ShaderAst::ConstantValueExpression>();
		constantNode->value = std::move(value);
		constantNode->cachedExpressionType = ShaderAst::GetExpressionType(constantNode->value);

		return constantNode;
	}

	template<typename T>
	std::unique_ptr<ShaderAst::ConstantValueExpression> Impl::Constant::operator()(ShaderAst::ExpressionType type, T value) const
	{
		assert(IsPrimitiveType(type));

		switch (std::get<ShaderAst::PrimitiveType>(type))
		{
			case ShaderAst::PrimitiveType::Boolean: return ShaderBuilder::Constant(value != T(0));
			case ShaderAst::PrimitiveType::Float32: return ShaderBuilder::Constant(SafeCast<float>(value));
			case ShaderAst::PrimitiveType::Int32:   return ShaderBuilder::Constant(SafeCast<Int32>(value));
			case ShaderAst::PrimitiveType::UInt32:  return ShaderBuilder::Constant(SafeCast<UInt32>(value));
			case ShaderAst::PrimitiveType::String:  return ShaderBuilder::Constant(value);
		}

		throw std::runtime_error("unexpected primitive type");
	}

	inline std::unique_ptr<ShaderAst::DeclareAliasStatement> Impl::DeclareAlias::operator()(std::string name, ShaderAst::ExpressionPtr expression) const
	{
		auto declareAliasNode = std::make_unique<ShaderAst::DeclareAliasStatement>();
		declareAliasNode->name = std::move(name);
		declareAliasNode->expression = std::move(expression);

		return declareAliasNode;
	}

	inline std::unique_ptr<ShaderAst::DeclareConstStatement> Impl::DeclareConst::operator()(std::string name, ShaderAst::ExpressionPtr initialValue) const
	{
		auto declareConstNode = std::make_unique<ShaderAst::DeclareConstStatement>();
		declareConstNode->name = std::move(name);
		declareConstNode->expression = std::move(initialValue);

		return declareConstNode;
	}

	inline std::unique_ptr<ShaderAst::DeclareConstStatement> Impl::DeclareConst::operator()(std::string name, ShaderAst::ExpressionValue<ShaderAst::ExpressionType> type, ShaderAst::ExpressionPtr initialValue) const
	{
		auto declareConstNode = std::make_unique<ShaderAst::DeclareConstStatement>();
		declareConstNode->name = std::move(name);
		declareConstNode->type = std::move(type);
		declareConstNode->expression = std::move(initialValue);

		return declareConstNode;
	}

	inline std::unique_ptr<ShaderAst::DeclareFunctionStatement> Impl::DeclareFunction::operator()(std::string name, ShaderAst::StatementPtr statement) const
	{
		auto declareFunctionNode = std::make_unique<ShaderAst::DeclareFunctionStatement>();
		declareFunctionNode->name = std::move(name);
		declareFunctionNode->statements.push_back(std::move(statement));

		return declareFunctionNode;
	}

	inline std::unique_ptr<ShaderAst::DeclareFunctionStatement> Impl::DeclareFunction::operator()(std::string name, std::vector<ShaderAst::DeclareFunctionStatement::Parameter> parameters, std::vector<ShaderAst::StatementPtr> statements, ShaderAst::ExpressionValue<ShaderAst::ExpressionType> returnType) const
	{
		auto declareFunctionNode = std::make_unique<ShaderAst::DeclareFunctionStatement>();
		declareFunctionNode->name = std::move(name);
		declareFunctionNode->parameters = std::move(parameters);
		declareFunctionNode->returnType = std::move(returnType);
		declareFunctionNode->statements = std::move(statements);

		return declareFunctionNode;
	}

	inline std::unique_ptr<ShaderAst::DeclareFunctionStatement> Impl::DeclareFunction::operator()(std::optional<ShaderStageType> entryStage, std::string name, ShaderAst::StatementPtr statement) const
	{
		auto declareFunctionNode = std::make_unique<ShaderAst::DeclareFunctionStatement>();
		declareFunctionNode->name = std::move(name);
		declareFunctionNode->statements.push_back(std::move(statement));

		if (entryStage)
			declareFunctionNode->entryStage = *entryStage;

		return declareFunctionNode;
	}

	inline std::unique_ptr<ShaderAst::DeclareFunctionStatement> Impl::DeclareFunction::operator()(std::optional<ShaderStageType> entryStage, std::string name, std::vector<ShaderAst::DeclareFunctionStatement::Parameter> parameters, std::vector<ShaderAst::StatementPtr> statements, ShaderAst::ExpressionValue<ShaderAst::ExpressionType> returnType) const
	{
		auto declareFunctionNode = std::make_unique<ShaderAst::DeclareFunctionStatement>();
		declareFunctionNode->name = std::move(name);
		declareFunctionNode->parameters = std::move(parameters);
		declareFunctionNode->returnType = std::move(returnType);
		declareFunctionNode->statements = std::move(statements);

		if (entryStage)
			declareFunctionNode->entryStage = *entryStage;

		return declareFunctionNode;
	}

	inline std::unique_ptr<ShaderAst::DeclareOptionStatement> Impl::DeclareOption::operator()(std::string name, ShaderAst::ExpressionValue<ShaderAst::ExpressionType> type, ShaderAst::ExpressionPtr initialValue) const
	{
		auto declareOptionNode = std::make_unique<ShaderAst::DeclareOptionStatement>();
		declareOptionNode->optName = std::move(name);
		declareOptionNode->optType = std::move(type);
		declareOptionNode->defaultValue = std::move(initialValue);

		return declareOptionNode;
	}

	inline std::unique_ptr<ShaderAst::DeclareStructStatement> Impl::DeclareStruct::operator()(ShaderAst::StructDescription description, ShaderAst::ExpressionValue<bool> isExported) const
	{
		auto declareStructNode = std::make_unique<ShaderAst::DeclareStructStatement>();
		declareStructNode->description = std::move(description);
		declareStructNode->isExported = std::move(isExported);

		return declareStructNode;
	}

	inline std::unique_ptr<ShaderAst::DeclareVariableStatement> Impl::DeclareVariable::operator()(std::string name, ShaderAst::ExpressionPtr initialValue) const
	{
		auto declareVariableNode = std::make_unique<ShaderAst::DeclareVariableStatement>();
		declareVariableNode->varName = std::move(name);
		declareVariableNode->initialExpression = std::move(initialValue);

		return declareVariableNode;
	}

	inline std::unique_ptr<ShaderAst::DeclareVariableStatement> Impl::DeclareVariable::operator()(std::string name, ShaderAst::ExpressionValue<ShaderAst::ExpressionType> type, ShaderAst::ExpressionPtr initialValue) const
	{
		auto declareVariableNode = std::make_unique<ShaderAst::DeclareVariableStatement>();
		declareVariableNode->varName = std::move(name);
		declareVariableNode->varType = std::move(type);
		declareVariableNode->initialExpression = std::move(initialValue);

		return declareVariableNode;
	}

	inline std::unique_ptr<ShaderAst::ExpressionStatement> Impl::ExpressionStatement::operator()(ShaderAst::ExpressionPtr expression) const
	{
		auto expressionStatementNode = std::make_unique<ShaderAst::ExpressionStatement>();
		expressionStatementNode->expression = std::move(expression);

		return expressionStatementNode;
	}

	inline std::unique_ptr<ShaderAst::ForStatement> Nz::ShaderBuilder::Impl::For::operator()(std::string varName, ShaderAst::ExpressionPtr fromExpression, ShaderAst::ExpressionPtr toExpression, ShaderAst::StatementPtr statement) const
	{
		auto forNode = std::make_unique<ShaderAst::ForStatement>();
		forNode->fromExpr = std::move(fromExpression);
		forNode->statement = std::move(statement);
		forNode->toExpr = std::move(toExpression);
		forNode->varName = std::move(varName);

		return forNode;
	}

	inline std::unique_ptr<ShaderAst::ForStatement> Nz::ShaderBuilder::Impl::For::operator()(std::string varName, ShaderAst::ExpressionPtr fromExpression, ShaderAst::ExpressionPtr toExpression, ShaderAst::ExpressionPtr stepExpression, ShaderAst::StatementPtr statement) const
	{
		auto forNode = std::make_unique<ShaderAst::ForStatement>();
		forNode->fromExpr = std::move(fromExpression);
		forNode->statement = std::move(statement);
		forNode->stepExpr = std::move(stepExpression);
		forNode->toExpr = std::move(toExpression);
		forNode->varName = std::move(varName);

		return forNode;
	}

	std::unique_ptr<ShaderAst::ForEachStatement> Impl::ForEach::operator()(std::string varName, ShaderAst::ExpressionPtr expression, ShaderAst::StatementPtr statement) const
	{
		auto forEachNode = std::make_unique<ShaderAst::ForEachStatement>();
		forEachNode->expression = std::move(expression);
		forEachNode->statement = std::move(statement);
		forEachNode->varName = std::move(varName);

		return forEachNode;
	}

	inline std::unique_ptr<ShaderAst::IdentifierExpression> Impl::Identifier::operator()(std::string name) const
	{
		auto identifierNode = std::make_unique<ShaderAst::IdentifierExpression>();
		identifierNode->identifier = std::move(name);

		return identifierNode;
	}

	inline std::unique_ptr<ShaderAst::ImportStatement> Impl::Import::operator()(std::vector<std::string> modulePath) const
	{
		auto importNode = std::make_unique<ShaderAst::ImportStatement>();
		importNode->modulePath = std::move(modulePath);

		return importNode;
	}

	inline std::unique_ptr<ShaderAst::IntrinsicExpression> Impl::Intrinsic::operator()(ShaderAst::IntrinsicType intrinsicType, std::vector<ShaderAst::ExpressionPtr> parameters) const
	{
		auto intrinsicExpression = std::make_unique<ShaderAst::IntrinsicExpression>();
		intrinsicExpression->intrinsic = intrinsicType;
		intrinsicExpression->parameters = std::move(parameters);

		return intrinsicExpression;
	}

	inline std::unique_ptr<ShaderAst::MultiStatement> Impl::Multi::operator()(std::vector<ShaderAst::StatementPtr> statements) const
	{
		auto multiStatement = std::make_unique<ShaderAst::MultiStatement>();
		multiStatement->statements = std::move(statements);

		return multiStatement;
	}

	template<typename T>
	std::unique_ptr<T> Impl::NoParam<T>::operator()() const
	{
		return std::make_unique<T>();
	}

	inline std::unique_ptr<ShaderAst::ReturnStatement> Impl::Return::operator()(ShaderAst::ExpressionPtr expr) const
	{
		auto returnNode = std::make_unique<ShaderAst::ReturnStatement>();
		returnNode->returnExpr = std::move(expr);

		return returnNode;
	}

	inline std::unique_ptr<ShaderAst::ScopedStatement> Impl::Scoped::operator()(ShaderAst::StatementPtr statement) const
	{
		auto scopedNode = std::make_unique<ShaderAst::ScopedStatement>();
		scopedNode->statement = std::move(statement);

		return scopedNode;
	}

	inline std::unique_ptr<ShaderAst::SwizzleExpression> Impl::Swizzle::operator()(ShaderAst::ExpressionPtr expression, std::array<UInt32, 4> swizzleComponents, std::size_t componentCount) const
	{
		assert(componentCount > 0);
		assert(componentCount <= 4);

		auto swizzleNode = std::make_unique<ShaderAst::SwizzleExpression>();
		swizzleNode->expression = std::move(expression);
		swizzleNode->componentCount = componentCount;
		swizzleNode->components = swizzleComponents;

		return swizzleNode;
	}

	inline std::unique_ptr<ShaderAst::SwizzleExpression> Impl::Swizzle::operator()(ShaderAst::ExpressionPtr expression, std::vector<UInt32> swizzleComponents) const
	{
		auto swizzleNode = std::make_unique<ShaderAst::SwizzleExpression>();
		swizzleNode->expression = std::move(expression);

		assert(swizzleComponents.size() <= swizzleNode->components.size());
		swizzleNode->componentCount = swizzleComponents.size();
		for (std::size_t i = 0; i < swizzleNode->componentCount; ++i)
		{
			assert(swizzleComponents[i] <= 4);
			swizzleNode->components[i] = swizzleComponents[i];
		}

		return swizzleNode;
	}

	inline std::unique_ptr<ShaderAst::UnaryExpression> Impl::Unary::operator()(ShaderAst::UnaryType op, ShaderAst::ExpressionPtr expression) const
	{
		auto unaryNode = std::make_unique<ShaderAst::UnaryExpression>();
		unaryNode->expression = std::move(expression);
		unaryNode->op = op;

		return unaryNode;
	}

	inline std::unique_ptr<ShaderAst::VariableExpression> Impl::Variable::operator()(std::size_t variableId, ShaderAst::ExpressionType expressionType) const
	{
		auto varNode = std::make_unique<ShaderAst::VariableExpression>();
		varNode->variableId = variableId;
		varNode->cachedExpressionType = std::move(expressionType);

		return varNode;
	}

	inline std::unique_ptr<ShaderAst::WhileStatement> Impl::While::operator()(ShaderAst::ExpressionPtr condition, ShaderAst::StatementPtr body) const
	{
		auto whileNode = std::make_unique<ShaderAst::WhileStatement>();
		whileNode->condition = std::move(condition);
		whileNode->body = std::move(body);

		return whileNode;
	}
}

#include <Nazara/Shader/DebugOff.hpp>
