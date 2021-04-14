// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderBuilder.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderBuilder
{
	inline std::unique_ptr<ShaderAst::AccessMemberIdentifierExpression> Impl::AccessMember::operator()(ShaderAst::ExpressionPtr structExpr, std::vector<std::string> memberIdentifiers) const
	{
		auto accessMemberNode = std::make_unique<ShaderAst::AccessMemberIdentifierExpression>();
		accessMemberNode->structExpr = std::move(structExpr);
		accessMemberNode->memberIdentifiers = std::move(memberIdentifiers);

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

	inline std::unique_ptr<ShaderAst::CastExpression> Impl::Cast::operator()(ShaderAst::ExpressionType targetType, std::vector<ShaderAst::ExpressionPtr> expressions) const
	{
		auto castNode = std::make_unique<ShaderAst::CastExpression>();
		castNode->targetType = std::move(targetType);

		assert(expressions.size() <= castNode->expressions.size());
		for (std::size_t i = 0; i < expressions.size(); ++i)
			castNode->expressions[i] = std::move(expressions[i]);

		return castNode;
	}

	inline std::unique_ptr<ShaderAst::ConditionalExpression> Impl::ConditionalExpression::operator()(std::string conditionName, ShaderAst::ExpressionPtr truePath, ShaderAst::ExpressionPtr falsePath) const
	{
		auto condExprNode = std::make_unique<ShaderAst::ConditionalExpression>();
		condExprNode->conditionName = std::move(conditionName);
		condExprNode->falsePath = std::move(falsePath);
		condExprNode->truePath = std::move(truePath);

		return condExprNode;
	}

	inline std::unique_ptr<ShaderAst::ConditionalStatement> Impl::ConditionalStatement::operator()(std::string conditionName, ShaderAst::StatementPtr statement) const
	{
		auto condStatementNode = std::make_unique<ShaderAst::ConditionalStatement>();
		condStatementNode->conditionName = std::move(conditionName);
		condStatementNode->statement = std::move(statement);

		return condStatementNode;
	}

	inline std::unique_ptr<ShaderAst::ConstantExpression> Impl::Constant::operator()(ShaderAst::ConstantValue value) const
	{
		auto constantNode = std::make_unique<ShaderAst::ConstantExpression>();
		constantNode->value = std::move(value);

		return constantNode;
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

	inline std::unique_ptr<ShaderAst::DeclareStructStatement> Impl::DeclareStruct::operator()(ShaderAst::StructDescription description) const
	{
		auto declareStructNode = std::make_unique<ShaderAst::DeclareStructStatement>();
		declareStructNode->description = std::move(description);

		return declareStructNode;
	}

	inline std::unique_ptr<ShaderAst::DeclareVariableStatement> Nz::ShaderBuilder::Impl::DeclareVariable::operator()(std::string name, ShaderAst::ExpressionType type, ShaderAst::ExpressionPtr initialValue) const
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

	inline std::unique_ptr<ShaderAst::ReturnStatement> Impl::Return::operator()(ShaderAst::ExpressionPtr expr) const
	{
		auto returnNode = std::make_unique<ShaderAst::ReturnStatement>();
		returnNode->returnExpr = std::move(expr);

		return returnNode;
	}

	template<typename T>
	std::unique_ptr<T> Impl::NoParam<T>::operator()() const
	{
		return std::make_unique<T>();
	}

	inline std::unique_ptr<ShaderAst::SwizzleExpression> Impl::Swizzle::operator()(ShaderAst::ExpressionPtr expression, std::vector<ShaderAst::SwizzleComponent> swizzleComponents) const
	{
		auto swizzleNode = std::make_unique<ShaderAst::SwizzleExpression>();
		swizzleNode->expression = std::move(expression);

		assert(swizzleComponents.size() <= swizzleNode->components.size());
		for (std::size_t i = 0; i < swizzleComponents.size(); ++i)
			swizzleNode->components[i] = swizzleComponents[i];

		return swizzleNode;
	}
}

#include <Nazara/Shader/DebugOff.hpp>
