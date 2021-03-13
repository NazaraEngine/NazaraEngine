// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderBuilder.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderBuilder
{
	inline std::unique_ptr<ShaderAst::BinaryExpression> Impl::Binary::operator()(ShaderAst::BinaryType op, ShaderAst::ExpressionPtr left, ShaderAst::ExpressionPtr right) const
	{
		auto constantNode = std::make_unique<ShaderAst::BinaryExpression>();
		constantNode->op = op;
		constantNode->left = std::move(left);
		constantNode->right = std::move(right);

		return constantNode;
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

	inline std::unique_ptr<ShaderAst::ConstantExpression> Impl::Constant::operator()(ShaderConstantValue value) const
	{
		auto constantNode = std::make_unique<ShaderAst::ConstantExpression>();
		constantNode->value = std::move(value);

		return constantNode;
	}

	inline std::unique_ptr<ShaderAst::DeclareFunctionStatement> Impl::DeclareFunction::operator()(std::string name, std::vector<ShaderAst::DeclareFunctionStatement::Parameter> parameters, std::vector<ShaderAst::StatementPtr> statements, ShaderAst::ShaderExpressionType returnType) const
	{
		auto declareFunctionNode = std::make_unique<ShaderAst::DeclareFunctionStatement>();
		declareFunctionNode->name = std::move(name);
		declareFunctionNode->parameters = std::move(parameters);
		declareFunctionNode->returnType = std::move(returnType);
		declareFunctionNode->statements = std::move(statements);

		return declareFunctionNode;
	}

	inline std::unique_ptr<ShaderAst::DeclareFunctionStatement> Impl::DeclareFunction::operator()(std::vector<ShaderAst::Attribute> attributes, std::string name, std::vector<ShaderAst::DeclareFunctionStatement::Parameter> parameters, std::vector<ShaderAst::StatementPtr> statements, ShaderAst::ShaderExpressionType returnType) const
	{
		auto declareFunctionNode = std::make_unique<ShaderAst::DeclareFunctionStatement>();
		declareFunctionNode->attributes = std::move(attributes);
		declareFunctionNode->name = std::move(name);
		declareFunctionNode->parameters = std::move(parameters);
		declareFunctionNode->returnType = std::move(returnType);
		declareFunctionNode->statements = std::move(statements);

		return declareFunctionNode;
	}

	inline std::unique_ptr<ShaderAst::DeclareVariableStatement> Nz::ShaderBuilder::Impl::DeclareVariable::operator()(std::string name, ShaderAst::ShaderExpressionType type, ShaderAst::ExpressionPtr initialValue) const
	{
		auto declareVariableNode = std::make_unique<ShaderAst::DeclareVariableStatement>();
		declareVariableNode->varName = std::move(name);
		declareVariableNode->varType = std::move(type);
		declareVariableNode->initialExpression = std::move(initialValue);

		return declareVariableNode;
	}

	inline std::unique_ptr<ShaderAst::IdentifierExpression> Impl::Identifier::operator()(std::string name) const
	{
		auto identifierNode = std::make_unique<ShaderAst::IdentifierExpression>();
		identifierNode->identifier = std::move(name);

		return identifierNode;
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
}

#include <Nazara/Shader/DebugOff.hpp>
