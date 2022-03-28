// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_AST_ASTCLONER_HPP
#define NAZARA_SHADER_AST_ASTCLONER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/Ast/AstExpressionVisitor.hpp>
#include <Nazara/Shader/Ast/AstStatementVisitor.hpp>
#include <Nazara/Shader/Ast/ExpressionValue.hpp>
#include <vector>

namespace Nz::ShaderAst
{
	class NAZARA_SHADER_API AstCloner : public AstExpressionVisitor, public AstStatementVisitor
	{
		public:
			AstCloner() = default;
			AstCloner(const AstCloner&) = delete;
			AstCloner(AstCloner&&) = delete;
			~AstCloner() = default;

			template<typename T> ExpressionValue<T> Clone(const ExpressionValue<T>& expressionValue);
			inline ExpressionValue<ExpressionType> Clone(const ExpressionValue<ExpressionType>& expressionValue);
			ExpressionPtr Clone(Expression& statement);
			StatementPtr Clone(Statement& statement);

			AstCloner& operator=(const AstCloner&) = delete;
			AstCloner& operator=(AstCloner&&) = delete;

		protected:
			inline ExpressionPtr CloneExpression(const ExpressionPtr& expr);
			inline StatementPtr CloneStatement(const StatementPtr& statement);

			virtual ExpressionPtr CloneExpression(Expression& expr);
			virtual StatementPtr CloneStatement(Statement& statement);
			virtual ExpressionValue<ExpressionType> CloneType(const ExpressionValue<ExpressionType>& exprType);

			virtual ExpressionPtr Clone(AccessIdentifierExpression& node);
			virtual ExpressionPtr Clone(AccessIndexExpression& node);
			virtual ExpressionPtr Clone(AliasValueExpression& node);
			virtual ExpressionPtr Clone(AssignExpression& node);
			virtual ExpressionPtr Clone(BinaryExpression& node);
			virtual ExpressionPtr Clone(CallFunctionExpression& node);
			virtual ExpressionPtr Clone(CallMethodExpression& node);
			virtual ExpressionPtr Clone(CastExpression& node);
			virtual ExpressionPtr Clone(ConditionalExpression& node);
			virtual ExpressionPtr Clone(ConstantExpression& node);
			virtual ExpressionPtr Clone(ConstantValueExpression& node);
			virtual ExpressionPtr Clone(FunctionExpression& node);
			virtual ExpressionPtr Clone(IdentifierExpression& node);
			virtual ExpressionPtr Clone(IntrinsicExpression& node);
			virtual ExpressionPtr Clone(IntrinsicFunctionExpression& node);
			virtual ExpressionPtr Clone(StructTypeExpression& node);
			virtual ExpressionPtr Clone(SwizzleExpression& node);
			virtual ExpressionPtr Clone(TypeExpression& node);
			virtual ExpressionPtr Clone(VariableValueExpression& node);
			virtual ExpressionPtr Clone(UnaryExpression& node);

			virtual StatementPtr Clone(BranchStatement& node);
			virtual StatementPtr Clone(ConditionalStatement& node);
			virtual StatementPtr Clone(DeclareAliasStatement& node);
			virtual StatementPtr Clone(DeclareConstStatement& node);
			virtual StatementPtr Clone(DeclareExternalStatement& node);
			virtual StatementPtr Clone(DeclareFunctionStatement& node);
			virtual StatementPtr Clone(DeclareOptionStatement& node);
			virtual StatementPtr Clone(DeclareStructStatement& node);
			virtual StatementPtr Clone(DeclareVariableStatement& node);
			virtual StatementPtr Clone(DiscardStatement& node);
			virtual StatementPtr Clone(ExpressionStatement& node);
			virtual StatementPtr Clone(ForStatement& node);
			virtual StatementPtr Clone(ForEachStatement& node);
			virtual StatementPtr Clone(ImportStatement& node);
			virtual StatementPtr Clone(MultiStatement& node);
			virtual StatementPtr Clone(NoOpStatement& node);
			virtual StatementPtr Clone(ReturnStatement& node);
			virtual StatementPtr Clone(ScopedStatement& node);
			virtual StatementPtr Clone(WhileStatement& node);

#define NAZARA_SHADERAST_NODE(NodeType) void Visit(NodeType& node) override;
#include <Nazara/Shader/Ast/AstNodeList.hpp>

			void PushExpression(ExpressionPtr expression);
			void PushStatement(StatementPtr statement);

			ExpressionPtr PopExpression();
			StatementPtr PopStatement();

		private:
			std::vector<ExpressionPtr> m_expressionStack;
			std::vector<StatementPtr>  m_statementStack;
	};

	template<typename T> ExpressionValue<T> Clone(const ExpressionValue<T>& attribute);
	inline ExpressionPtr Clone(Expression& node);
	inline StatementPtr Clone(Statement& node);
}

#include <Nazara/Shader/Ast/AstCloner.inl>

#endif // NAZARA_SHADER_AST_ASTCLONER_HPP
