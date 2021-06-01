// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderBuilder.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderBuilder
{
	inline std::unique_ptr<ShaderAst::AccessIdentifierExpression> Impl::AccessMember::operator()(ShaderAst::ExpressionPtr expr, std::vector<std::string> memberIdentifiers) const
	{
		auto accessMemberNode = std::make_unique<ShaderAst::AccessIdentifierExpression>();
		accessMemberNode->expr = std::move(expr);
		accessMemberNode->memberIdentifiers = std::move(memberIdentifiers);

		return accessMemberNode;
	}

	inline std::unique_ptr<ShaderAst::AccessIndexExpression> Impl::AccessIndex::operator()(ShaderAst::ExpressionPtr expr, std::vector<std::size_t> memberIndices) const
	{
		auto accessMemberNode = std::make_unique<ShaderAst::AccessIndexExpression>();
		accessMemberNode->expr = std::move(expr);
		accessMemberNode->memberIndices = std::move(memberIndices);

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

	inline std::unique_ptr<ShaderAst::BranchStatement> Impl::Branch::operator()(ShaderAst::ExpressionPtr condition, ShaderAst::StatementPtr truePath, ShaderAst::StatementPtr falsePath) const
	{
		auto branchNode = std::make_unique<ShaderAst::BranchStatement>();

		auto& condStatement = branchNode->condStatements.emplace_back();
		condStatement.condition = std::move(condition);
		condStatement.statement = std::move(truePath);

		branchNode->elseStatement = std::move(falsePath);

		return branchNode;
	}

	inline std::unique_ptr<ShaderAst::BranchStatement> Impl::Branch::operator()(std::vector<ShaderAst::BranchStatement::ConditionalStatement> condStatements, ShaderAst::StatementPtr elseStatement) const
	{
		auto branchNode = std::make_unique<ShaderAst::BranchStatement>();
		branchNode->condStatements = std::move(condStatements);
		branchNode->elseStatement = std::move(elseStatement);

		return branchNode;
	}

	inline std::unique_ptr<ShaderAst::CallFunctionExpression> Impl::CallFunction::operator()(std::string functionName, std::vector<ShaderAst::ExpressionPtr> parameters) const
	{
		auto callFunctionExpression = std::make_unique<ShaderAst::CallFunctionExpression>();
		callFunctionExpression->targetFunction = std::move(functionName);
		callFunctionExpression->parameters = std::move(parameters);

		return callFunctionExpression;
	}

	inline std::unique_ptr<ShaderAst::CastExpression> Impl::Cast::operator()(ShaderAst::ExpressionType targetType, std::array<ShaderAst::ExpressionPtr, 4> expressions) const
	{
		auto castNode = std::make_unique<ShaderAst::CastExpression>();
		castNode->expressions = std::move(expressions);
		castNode->targetType = std::move(targetType);

		return castNode;
	}

	inline std::unique_ptr<ShaderAst::CastExpression> Impl::Cast::operator()(ShaderAst::ExpressionType targetType, std::vector<ShaderAst::ExpressionPtr> expressions) const
	{
		auto castNode = std::make_unique<ShaderAst::CastExpression>();
		castNode->targetType = std::move(targetType);

		assert(expressions.size() <= castNode->expressions.size());
		for (std::size_t i = 0; i < expressions.size(); ++i)
			castNode->expressions[i] = std::move(expressions[i]);

		return castNode;
	}

	inline std::unique_ptr<ShaderAst::ConditionalExpression> Impl::ConditionalExpression::operator()(std::size_t optionIndex, ShaderAst::ExpressionPtr truePath, ShaderAst::ExpressionPtr falsePath) const
	{
		auto condExprNode = std::make_unique<ShaderAst::ConditionalExpression>();
		condExprNode->optionIndex = optionIndex;
		condExprNode->falsePath = std::move(falsePath);
		condExprNode->truePath = std::move(truePath);

		return condExprNode;
	}

	inline std::unique_ptr<ShaderAst::ConditionalStatement> Impl::ConditionalStatement::operator()(std::size_t optionIndex, ShaderAst::StatementPtr statement) const
	{
		auto condStatementNode = std::make_unique<ShaderAst::ConditionalStatement>();
		condStatementNode->optionIndex = optionIndex;
		condStatementNode->statement = std::move(statement);

		return condStatementNode;
	}

	inline std::unique_ptr<ShaderAst::ConstantExpression> Impl::Constant::operator()(ShaderAst::ConstantValue value) const
	{
		auto constantNode = std::make_unique<ShaderAst::ConstantExpression>();
		constantNode->value = std::move(value);

		return constantNode;
	}

	inline std::unique_ptr<ShaderAst::DeclareFunctionStatement> Impl::DeclareFunction::operator()(std::string name, ShaderAst::StatementPtr statement) const
	{
		auto declareFunctionNode = std::make_unique<ShaderAst::DeclareFunctionStatement>();
		declareFunctionNode->name = std::move(name);
		declareFunctionNode->statements.push_back(std::move(statement));

		return declareFunctionNode;
	}

	inline std::unique_ptr<ShaderAst::DeclareFunctionStatement> Impl::DeclareFunction::operator()(std::string name, std::vector<ShaderAst::DeclareFunctionStatement::Parameter> parameters, std::vector<ShaderAst::StatementPtr> statements, ShaderAst::ExpressionType returnType) const
	{
		auto declareFunctionNode = std::make_unique<ShaderAst::DeclareFunctionStatement>();
		declareFunctionNode->name = std::move(name);
		declareFunctionNode->parameters = std::move(parameters);
		declareFunctionNode->returnType = std::move(returnType);
		declareFunctionNode->statements = std::move(statements);

		return declareFunctionNode;
	}

	inline std::unique_ptr<ShaderAst::DeclareFunctionStatement> Impl::DeclareFunction::operator()(std::optional<ShaderStageType> entryStage, std::string name, std::vector<ShaderAst::DeclareFunctionStatement::Parameter> parameters, std::vector<ShaderAst::StatementPtr> statements, ShaderAst::ExpressionType returnType) const
	{
		auto declareFunctionNode = std::make_unique<ShaderAst::DeclareFunctionStatement>();
		declareFunctionNode->entryStage = entryStage;
		declareFunctionNode->name = std::move(name);
		declareFunctionNode->parameters = std::move(parameters);
		declareFunctionNode->returnType = std::move(returnType);
		declareFunctionNode->statements = std::move(statements);

		return declareFunctionNode;
	}

	inline std::unique_ptr<ShaderAst::DeclareOptionStatement> Impl::DeclareOption::operator()(std::string name, ShaderAst::ExpressionType type, ShaderAst::ExpressionPtr initialValue) const
	{
		auto declareOptionNode = std::make_unique<ShaderAst::DeclareOptionStatement>();
		declareOptionNode->optName = std::move(name);
		declareOptionNode->optType = std::move(type);
		declareOptionNode->initialValue = std::move(initialValue);

		return declareOptionNode;
	}

	inline std::unique_ptr<ShaderAst::DeclareStructStatement> Impl::DeclareStruct::operator()(ShaderAst::StructDescription description) const
	{
		auto declareStructNode = std::make_unique<ShaderAst::DeclareStructStatement>();
		declareStructNode->description = std::move(description);

		return declareStructNode;
	}

	inline std::unique_ptr<ShaderAst::DeclareVariableStatement> Impl::DeclareVariable::operator()(std::string name, ShaderAst::ExpressionPtr initialValue) const
	{
		auto declareVariableNode = std::make_unique<ShaderAst::DeclareVariableStatement>();
		declareVariableNode->varName = std::move(name);
		declareVariableNode->initialExpression = std::move(initialValue);

		return declareVariableNode;
	}

	inline std::unique_ptr<ShaderAst::DeclareVariableStatement> Impl::DeclareVariable::operator()(std::string name, ShaderAst::ExpressionType type, ShaderAst::ExpressionPtr initialValue) const
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

	inline std::unique_ptr<ShaderAst::IdentifierExpression> Impl::Identifier::operator()(std::string name) const
	{
		auto identifierNode = std::make_unique<ShaderAst::IdentifierExpression>();
		identifierNode->identifier = std::move(name);

		return identifierNode;
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

	inline std::unique_ptr<ShaderAst::SelectOptionExpression> Impl::SelectOption::operator()(std::string optionName, ShaderAst::ExpressionPtr truePath, ShaderAst::ExpressionPtr falsePath) const
	{
		auto selectOptNode = std::make_unique<ShaderAst::SelectOptionExpression>();
		selectOptNode->optionName = std::move(optionName);
		selectOptNode->falsePath = std::move(falsePath);
		selectOptNode->truePath = std::move(truePath);

		return selectOptNode;
	}

	inline std::unique_ptr<ShaderAst::SwizzleExpression> Impl::Swizzle::operator()(ShaderAst::ExpressionPtr expression, std::vector<ShaderAst::SwizzleComponent> swizzleComponents) const
	{
		auto swizzleNode = std::make_unique<ShaderAst::SwizzleExpression>();
		swizzleNode->expression = std::move(expression);

		assert(swizzleComponents.size() <= swizzleNode->components.size());
		swizzleNode->componentCount = swizzleComponents.size();
		for (std::size_t i = 0; i < swizzleNode->componentCount; ++i)
			swizzleNode->components[i] = swizzleComponents[i];

		return swizzleNode;
	}

	inline std::unique_ptr<ShaderAst::UnaryExpression> Impl::Unary::operator()(ShaderAst::UnaryType op, ShaderAst::ExpressionPtr expression) const
	{
		auto unaryNode = std::make_unique<ShaderAst::UnaryExpression>();
		unaryNode->expression = std::move(expression);
		unaryNode->op = op;

		return unaryNode;
	}
}

#include <Nazara/Shader/DebugOff.hpp>
