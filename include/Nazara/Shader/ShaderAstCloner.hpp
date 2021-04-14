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
			virtual ExpressionPtr CloneExpression(ExpressionPtr& expr);
			virtual StatementPtr CloneStatement(StatementPtr& statement);

			virtual StatementPtr Clone(DeclareExternalStatement& node);
			virtual StatementPtr Clone(DeclareFunctionStatement& node);
			virtual StatementPtr Clone(DeclareStructStatement& node);
			virtual StatementPtr Clone(DeclareVariableStatement& node);

			virtual ExpressionPtr Clone(AccessMemberIdentifierExpression& node);
			virtual ExpressionPtr Clone(AccessMemberIndexExpression& node);
			virtual ExpressionPtr Clone(CastExpression& node);
			virtual ExpressionPtr Clone(IdentifierExpression& node);
			virtual ExpressionPtr Clone(VariableExpression& node);

			using AstExpressionVisitor::Visit;
			using AstStatementVisitor::Visit;

			void Visit(AccessMemberIdentifierExpression& node) override;
			void Visit(AccessMemberIndexExpression& node) override;
			void Visit(AssignExpression& node) override;
			void Visit(BinaryExpression& node) override;
			void Visit(CastExpression& node) override;
			void Visit(ConditionalExpression& node) override;
			void Visit(ConstantExpression& node) override;
			void Visit(IdentifierExpression& node) override;
			void Visit(IntrinsicExpression& node) override;
			void Visit(SwizzleExpression& node) override;
			void Visit(VariableExpression& node) override;

			void Visit(BranchStatement& node) override;
			void Visit(ConditionalStatement& node) override;
			void Visit(DeclareExternalStatement& node) override;
			void Visit(DeclareFunctionStatement& node) override;
			void Visit(DeclareStructStatement& node) override;
			void Visit(DeclareVariableStatement& node) override;
			void Visit(DiscardStatement& node) override;
			void Visit(ExpressionStatement& node) override;
			void Visit(MultiStatement& node) override;
			void Visit(NoOpStatement& node) override;
			void Visit(ReturnStatement& node) override;

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
