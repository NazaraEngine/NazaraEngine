// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADERASTCLONER_HPP
#define NAZARA_SHADERASTCLONER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/ShaderAstExpressionVisitor.hpp>
#include <Nazara/Shader/ShaderAstStatementVisitor.hpp>
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

			ExpressionPtr Clone(ExpressionPtr& statement);
			StatementPtr Clone(StatementPtr& statement);

			AstCloner& operator=(const AstCloner&) = delete;
			AstCloner& operator=(AstCloner&&) = delete;

		protected:
			inline ExpressionPtr CloneExpression(ExpressionPtr& expr);
			inline StatementPtr CloneStatement(StatementPtr& statement);

			virtual ExpressionPtr CloneExpression(Expression& expr);
			virtual StatementPtr CloneStatement(Statement& statement);

			virtual ExpressionPtr Clone(AccessMemberIdentifierExpression& node);
			virtual ExpressionPtr Clone(AccessMemberIndexExpression& node);
			virtual ExpressionPtr Clone(AssignExpression& node);
			virtual ExpressionPtr Clone(BinaryExpression& node);
			virtual ExpressionPtr Clone(CastExpression& node);
			virtual ExpressionPtr Clone(ConditionalExpression& node);
			virtual ExpressionPtr Clone(ConstantExpression& node);
			virtual ExpressionPtr Clone(IdentifierExpression& node);
			virtual ExpressionPtr Clone(IntrinsicExpression& node);
			virtual ExpressionPtr Clone(SwizzleExpression& node);
			virtual ExpressionPtr Clone(VariableExpression& node);

			virtual StatementPtr Clone(BranchStatement& node);
			virtual StatementPtr Clone(ConditionalStatement& node);
			virtual StatementPtr Clone(DeclareExternalStatement& node);
			virtual StatementPtr Clone(DeclareFunctionStatement& node);
			virtual StatementPtr Clone(DeclareStructStatement& node);
			virtual StatementPtr Clone(DeclareVariableStatement& node);
			virtual StatementPtr Clone(DiscardStatement& node);
			virtual StatementPtr Clone(ExpressionStatement& node);
			virtual StatementPtr Clone(MultiStatement& node);
			virtual StatementPtr Clone(NoOpStatement& node);
			virtual StatementPtr Clone(ReturnStatement& node);

#define NAZARA_SHADERAST_NODE(NodeType) void Visit(NodeType& node) override;
#include <Nazara/Shader/ShaderAstNodes.hpp>

			void PushExpression(ExpressionPtr expression);
			void PushStatement(StatementPtr statement);

			ExpressionPtr PopExpression();
			StatementPtr PopStatement();

		private:
			std::vector<ExpressionPtr> m_expressionStack;
			std::vector<StatementPtr>  m_statementStack;
	};

	inline ExpressionPtr Clone(ExpressionPtr& node);
	inline StatementPtr Clone(StatementPtr& node);
}

#include <Nazara/Shader/ShaderAstCloner.inl>

#endif
