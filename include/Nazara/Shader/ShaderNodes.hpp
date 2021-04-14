// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_NODES_HPP
#define NAZARA_SHADER_NODES_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Math/Vector4.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/Ast/ConstantValue.hpp>
#include <Nazara/Shader/ShaderEnums.hpp>
#include <Nazara/Shader/Ast/Attribute.hpp>
#include <Nazara/Shader/Ast/ExpressionType.hpp>
#include <array>
#include <memory>
#include <optional>
#include <string>

namespace Nz::ShaderAst
{
	class AstExpressionVisitor;
	class AstStatementVisitor;

	struct Node;

	using NodePtr = std::unique_ptr<Node>;

	struct NAZARA_SHADER_API Node
	{
		Node() = default;
		Node(const Node&) = delete;
		Node(Node&&) noexcept = default;
		virtual ~Node();

		virtual NodeType GetType() const = 0;

		Node& operator=(const Node&) = delete;
		Node& operator=(Node&&) noexcept = default;
	};

	// Expressions

	struct Expression;

	using ExpressionPtr = std::unique_ptr<Expression>;

	struct NAZARA_SHADER_API Expression : Node
	{
		Expression() = default;
		Expression(const Expression&) = delete;
		Expression(Expression&&) noexcept = default;
		~Expression() = default;

		virtual void Visit(AstExpressionVisitor& visitor) = 0;

		Expression& operator=(const Expression&) = delete;
		Expression& operator=(Expression&&) noexcept = default;

		std::optional<ExpressionType> cachedExpressionType;
	};

	struct NAZARA_SHADER_API AccessMemberIdentifierExpression : public Expression
	{
		NodeType GetType() const override;
		void Visit(AstExpressionVisitor& visitor) override;

		ExpressionPtr structExpr;
		std::vector<std::string> memberIdentifiers;
	};

	struct NAZARA_SHADER_API AccessMemberIndexExpression : public Expression
	{
		NodeType GetType() const override;
		void Visit(AstExpressionVisitor& visitor) override;

		ExpressionPtr structExpr;
		std::vector<std::size_t> memberIndices;
	};

	struct NAZARA_SHADER_API AssignExpression : public Expression
	{
		NodeType GetType() const override;
		void Visit(AstExpressionVisitor& visitor) override;

		AssignType    op;
		ExpressionPtr left;
		ExpressionPtr right;
	};

	struct NAZARA_SHADER_API BinaryExpression : public Expression
	{
		NodeType GetType() const override;
		void Visit(AstExpressionVisitor& visitor) override;

		BinaryType    op;
		ExpressionPtr left;
		ExpressionPtr right;
	};

	struct NAZARA_SHADER_API CastExpression : public Expression
	{
		NodeType GetType() const override;
		void Visit(AstExpressionVisitor& visitor) override;

		ExpressionType targetType;
		std::array<ExpressionPtr, 4> expressions;
	};

	struct NAZARA_SHADER_API ConditionalExpression : public Expression
	{
		NodeType GetType() const override;
		void Visit(AstExpressionVisitor& visitor) override;

		std::string conditionName;
		ExpressionPtr falsePath;
		ExpressionPtr truePath;
	};

	struct NAZARA_SHADER_API ConstantExpression : public Expression
	{
		NodeType GetType() const override;
		void Visit(AstExpressionVisitor& visitor) override;

		ShaderAst::ConstantValue value;
	};

	struct NAZARA_SHADER_API IdentifierExpression : public Expression
	{
		NodeType GetType() const override;
		void Visit(AstExpressionVisitor& visitor) override;

		std::string identifier;
	};

	struct NAZARA_SHADER_API IntrinsicExpression : public Expression
	{
		NodeType GetType() const override;
		void Visit(AstExpressionVisitor& visitor) override;

		IntrinsicType intrinsic;
		std::vector<ExpressionPtr> parameters;
	};

	struct NAZARA_SHADER_API SwizzleExpression : public Expression
	{
		NodeType GetType() const override;
		void Visit(AstExpressionVisitor& visitor) override;

		std::array<SwizzleComponent, 4> components;
		std::size_t componentCount;
		ExpressionPtr expression;
	};

	struct NAZARA_SHADER_API VariableExpression : Expression
	{
		NodeType GetType() const override;
		void Visit(AstExpressionVisitor& visitor) override;

		std::size_t variableId;
	};

	// Statements

	struct Statement;

	using StatementPtr = std::unique_ptr<Statement>;

	struct NAZARA_SHADER_API Statement : Node
	{
		Statement() = default;
		Statement(const Statement&) = delete;
		Statement(Statement&&) noexcept = default;
		~Statement() = default;

		virtual void Visit(AstStatementVisitor& visitor) = 0;

		Statement& operator=(const Statement&) = delete;
		Statement& operator=(Statement&&) noexcept = default;
	};

	struct NAZARA_SHADER_API BranchStatement : public Statement
	{
		NodeType GetType() const override;
		void Visit(AstStatementVisitor& visitor) override;

		struct ConditionalStatement
		{
			ExpressionPtr condition;
			StatementPtr  statement;
		};

		std::vector<ConditionalStatement> condStatements;
		StatementPtr elseStatement;
	};

	struct NAZARA_SHADER_API ConditionalStatement : Statement
	{
		NodeType GetType() const override;
		void Visit(AstStatementVisitor& visitor) override;

		std::string conditionName;
		StatementPtr statement;
	};

	struct NAZARA_SHADER_API DeclareExternalStatement : Statement
	{
		NodeType GetType() const override;
		void Visit(AstStatementVisitor& visitor) override;

		struct ExternalVar
		{
			std::optional<unsigned int> bindingIndex;
			std::string name;
			ExpressionType type;
		};

		std::optional<std::size_t> varIndex;
		std::vector<ExternalVar> externalVars;
	};

	struct NAZARA_SHADER_API DeclareFunctionStatement : Statement
	{
		NodeType GetType() const override;
		void Visit(AstStatementVisitor& visitor) override;

		struct Parameter
		{
			std::string name;
			ExpressionType type;
		};

		std::optional<ShaderStageType> entryStage;
		std::optional<std::size_t> funcIndex;
		std::optional<std::size_t> varIndex;
		std::string name;
		std::vector<Parameter> parameters;
		std::vector<StatementPtr> statements;
		ExpressionType returnType;
	};

	struct NAZARA_SHADER_API DeclareStructStatement : Statement
	{
		NodeType GetType() const override;
		void Visit(AstStatementVisitor& visitor) override;

		std::optional<std::size_t> structIndex;
		StructDescription description;
	};

	struct NAZARA_SHADER_API DeclareVariableStatement : Statement
	{
		NodeType GetType() const override;
		void Visit(AstStatementVisitor& visitor) override;

		std::optional<std::size_t> varIndex;
		std::string varName;
		ExpressionPtr initialExpression;
		ExpressionType varType;
	};

	struct NAZARA_SHADER_API DiscardStatement : Statement
	{
		NodeType GetType() const override;
		void Visit(AstStatementVisitor& visitor) override;
	};

	struct NAZARA_SHADER_API ExpressionStatement : Statement
	{
		NodeType GetType() const override;
		void Visit(AstStatementVisitor& visitor) override;

		ExpressionPtr expression;
	};

	struct NAZARA_SHADER_API MultiStatement : Statement
	{
		NodeType GetType() const override;
		void Visit(AstStatementVisitor& visitor) override;

		std::vector<StatementPtr> statements;
	};

	struct NAZARA_SHADER_API NoOpStatement : Statement
	{
		NodeType GetType() const override;
		void Visit(AstStatementVisitor& visitor) override;
	};

	struct NAZARA_SHADER_API ReturnStatement : Statement
	{
		NodeType GetType() const override;
		void Visit(AstStatementVisitor& visitor) override;

		ExpressionPtr returnExpr;
	};

	inline const ShaderAst::ExpressionType& GetExpressionType(ShaderAst::Expression& expr);
	inline bool IsExpression(NodeType nodeType);
	inline bool IsStatement(NodeType nodeType);
}

#include <Nazara/Shader/ShaderNodes.inl>

#endif
