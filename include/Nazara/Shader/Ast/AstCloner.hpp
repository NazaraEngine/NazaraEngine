// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADERASTCLONER_HPP
#define NAZARA_SHADERASTCLONER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/Ast/Attribute.hpp>
#include <Nazara/Shader/Ast/AstExpressionVisitor.hpp>
#include <Nazara/Shader/Ast/AstStatementVisitor.hpp>
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

			ExpressionPtr Clone(Expression& statement);
			StatementPtr Clone(Statement& statement);

			AstCloner& operator=(const AstCloner&) = delete;
			AstCloner& operator=(AstCloner&&) = delete;

		protected:
			template<typename T> AttributeValue<T> CloneAttribute(const AttributeValue<T>& attribute);
			inline ExpressionPtr CloneExpression(const ExpressionPtr& expr);
			inline StatementPtr CloneStatement(const StatementPtr& statement);

			virtual ExpressionPtr CloneExpression(Expression& expr);
			virtual StatementPtr CloneStatement(Statement& statement);

			virtual ExpressionPtr Clone(AccessIdentifierExpression& node);
			virtual ExpressionPtr Clone(AccessIndexExpression& node);
			virtual ExpressionPtr Clone(AssignExpression& node);
			virtual ExpressionPtr Clone(BinaryExpression& node);
			virtual ExpressionPtr Clone(CallFunctionExpression& node);
			virtual ExpressionPtr Clone(CallMethodExpression& node);
			virtual ExpressionPtr Clone(CastExpression& node);
			virtual ExpressionPtr Clone(ConditionalExpression& node);
			virtual ExpressionPtr Clone(ConstantIndexExpression& node);
			virtual ExpressionPtr Clone(ConstantExpression& node);
			virtual ExpressionPtr Clone(IdentifierExpression& node);
			virtual ExpressionPtr Clone(IntrinsicExpression& node);
			virtual ExpressionPtr Clone(SwizzleExpression& node);
			virtual ExpressionPtr Clone(VariableExpression& node);
			virtual ExpressionPtr Clone(UnaryExpression& node);

			virtual StatementPtr Clone(BranchStatement& node);
			virtual StatementPtr Clone(ConditionalStatement& node);
			virtual StatementPtr Clone(DeclareConstStatement& node);
			virtual StatementPtr Clone(DeclareExternalStatement& node);
			virtual StatementPtr Clone(DeclareFunctionStatement& node);
			virtual StatementPtr Clone(DeclareOptionStatement& node);
			virtual StatementPtr Clone(DeclareStructStatement& node);
			virtual StatementPtr Clone(DeclareVariableStatement& node);
			virtual StatementPtr Clone(DiscardStatement& node);
			virtual StatementPtr Clone(ExpressionStatement& node);
			virtual StatementPtr Clone(MultiStatement& node);
			virtual StatementPtr Clone(NoOpStatement& node);
			virtual StatementPtr Clone(ReturnStatement& node);

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

	inline ExpressionPtr Clone(Expression& node);
	inline StatementPtr Clone(Statement& node);
}

#include <Nazara/Shader/Ast/AstCloner.inl>

#endif
