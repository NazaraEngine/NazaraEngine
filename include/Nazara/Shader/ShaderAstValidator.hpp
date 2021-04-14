// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADERVALIDATOR_HPP
#define NAZARA_SHADERVALIDATOR_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/ShaderAstScopedVisitor.hpp>
#include <Nazara/Utility/Enums.hpp>

namespace Nz::ShaderAst
{
	class NAZARA_SHADER_API AstValidator final : public AstScopedVisitor
	{
		public:
			inline AstValidator();
			AstValidator(const AstValidator&) = delete;
			AstValidator(AstValidator&&) = delete;
			~AstValidator() = default;

			bool Validate(StatementPtr& node, std::string* error = nullptr);

		private:
			const ExpressionType& GetExpressionType(Expression& expression);
			Expression& MandatoryExpr(ExpressionPtr& node);
			Statement& MandatoryStatement(StatementPtr& node);
			void TypeMustMatch(ExpressionPtr& left, ExpressionPtr& right);
			void TypeMustMatch(const ExpressionType& left, const ExpressionType& right);

			ExpressionType CheckField(const std::string& structName, const std::string* memberIdentifier, std::size_t remainingMembers);
			const ExpressionType& ResolveAlias(const ExpressionType& expressionType);

			void Visit(AccessMemberIdentifierExpression& node) override;
			void Visit(AssignExpression& node) override;
			void Visit(BinaryExpression& node) override;
			void Visit(CastExpression& node) override;
			void Visit(ConstantExpression& node) override;
			void Visit(ConditionalExpression& node) override;
			void Visit(IdentifierExpression& node) override;
			void Visit(IntrinsicExpression& node) override;
			void Visit(SwizzleExpression& node) override;

			void Visit(BranchStatement& node) override;
			void Visit(ConditionalStatement& node) override;
			void Visit(DeclareExternalStatement& node) override;
			void Visit(DeclareFunctionStatement& node) override;
			void Visit(DeclareStructStatement& node) override;
			void Visit(DeclareVariableStatement& node) override;
			void Visit(ExpressionStatement& node) override;
			void Visit(MultiStatement& node) override;

			struct Context;

			Context* m_context;
	};

	NAZARA_SHADER_API bool ValidateAst(StatementPtr& node, std::string* error = nullptr);
}

#include <Nazara/Shader/ShaderAstValidator.inl>

#endif
