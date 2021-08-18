// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_AST_NODES_HPP
#define NAZARA_SHADER_AST_NODES_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Math/Vector4.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/Ast/ConstantValue.hpp>
#include <Nazara/Shader/Ast/Enums.hpp>
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

	struct NAZARA_SHADER_API AccessIdentifierExpression : Expression
	{
		NodeType GetType() const override;
		void Visit(AstExpressionVisitor& visitor) override;

		ExpressionPtr expr;
		std::vector<std::string> identifiers;
	};

	struct NAZARA_SHADER_API AccessIndexExpression : Expression
	{
		NodeType GetType() const override;
		void Visit(AstExpressionVisitor& visitor) override;

		ExpressionPtr expr;
		std::vector<ExpressionPtr> indices;
	};

	struct NAZARA_SHADER_API AssignExpression : Expression
	{
		NodeType GetType() const override;
		void Visit(AstExpressionVisitor& visitor) override;

		AssignType    op;
		ExpressionPtr left;
		ExpressionPtr right;
	};

	struct NAZARA_SHADER_API BinaryExpression : Expression
	{
		NodeType GetType() const override;
		void Visit(AstExpressionVisitor& visitor) override;

		BinaryType    op;
		ExpressionPtr left;
		ExpressionPtr right;
	};

	struct NAZARA_SHADER_API CallFunctionExpression : Expression
	{
		NodeType GetType() const override;
		void Visit(AstExpressionVisitor& visitor) override;

		std::variant<std::string, std::size_t> targetFunction;
		std::vector<ExpressionPtr> parameters;
	};

	struct NAZARA_SHADER_API CallMethodExpression : Expression
	{
		NodeType GetType() const override;
		void Visit(AstExpressionVisitor& visitor) override;

		ExpressionPtr object;
		std::string methodName;
		std::vector<ExpressionPtr> parameters;
	};

	struct NAZARA_SHADER_API CastExpression : Expression
	{
		NodeType GetType() const override;
		void Visit(AstExpressionVisitor& visitor) override;

		ExpressionType targetType;
		std::array<ExpressionPtr, 4> expressions;
	};

	struct NAZARA_SHADER_API ConditionalExpression : Expression
	{
		NodeType GetType() const override;
		void Visit(AstExpressionVisitor& visitor) override;

		ExpressionPtr condition;
		ExpressionPtr falsePath;
		ExpressionPtr truePath;
	};

	struct NAZARA_SHADER_API ConstantExpression : Expression
	{
		NodeType GetType() const override;
		void Visit(AstExpressionVisitor& visitor) override;

		std::size_t constantId;
	};

	struct NAZARA_SHADER_API ConstantValueExpression : Expression
	{
		NodeType GetType() const override;
		void Visit(AstExpressionVisitor& visitor) override;

		ShaderAst::ConstantValue value;
	};

	struct NAZARA_SHADER_API IdentifierExpression : Expression
	{
		NodeType GetType() const override;
		void Visit(AstExpressionVisitor& visitor) override;

		std::string identifier;
	};

	struct NAZARA_SHADER_API IntrinsicExpression : Expression
	{
		NodeType GetType() const override;
		void Visit(AstExpressionVisitor& visitor) override;

		IntrinsicType intrinsic;
		std::vector<ExpressionPtr> parameters;
	};

	struct NAZARA_SHADER_API SwizzleExpression : Expression
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

	struct NAZARA_SHADER_API UnaryExpression : Expression
	{
		NodeType GetType() const override;
		void Visit(AstExpressionVisitor& visitor) override;

		UnaryType op;
		ExpressionPtr expression;
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

	struct NAZARA_SHADER_API BranchStatement : Statement
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
		bool isConst = false;
	};

	struct NAZARA_SHADER_API ConditionalStatement : Statement
	{
		NodeType GetType() const override;
		void Visit(AstStatementVisitor& visitor) override;

		ExpressionPtr condition;
		StatementPtr statement;
	};

	struct NAZARA_SHADER_API DeclareConstStatement : Statement
	{
		NodeType GetType() const override;
		void Visit(AstStatementVisitor& visitor) override;

		std::optional<std::size_t> constIndex;
		std::string name;
		ExpressionPtr expression;
		ExpressionType type;
	};

	struct NAZARA_SHADER_API DeclareExternalStatement : Statement
	{
		NodeType GetType() const override;
		void Visit(AstStatementVisitor& visitor) override;

		struct ExternalVar
		{
			AttributeValue<UInt32> bindingIndex;
			AttributeValue<UInt32> bindingSet;
			std::string name;
			ExpressionType type;
		};

		AttributeValue<UInt32> bindingSet;
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

		AttributeValue<DepthWriteMode> depthWrite;
		AttributeValue<bool> earlyFragmentTests;
		AttributeValue<ShaderStageType> entryStage;
		std::optional<std::size_t> funcIndex;
		std::optional<std::size_t> varIndex;
		std::string name;
		std::vector<Parameter> parameters;
		std::vector<StatementPtr> statements;
		ExpressionType returnType;
	};

	struct NAZARA_SHADER_API DeclareOptionStatement : Statement
	{
		NodeType GetType() const override;
		void Visit(AstStatementVisitor& visitor) override;

		std::optional<std::size_t> optIndex;
		std::string optName;
		ExpressionPtr initialValue;
		ExpressionType optType;
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

#include <Nazara/Shader/Ast/Nodes.inl>

#endif
