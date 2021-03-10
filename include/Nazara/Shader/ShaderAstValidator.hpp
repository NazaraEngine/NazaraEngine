// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADERVALIDATOR_HPP
#define NAZARA_SHADERVALIDATOR_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/ShaderAstCache.hpp>
#include <Nazara/Shader/ShaderAstRecursiveVisitor.hpp>
#include <Nazara/Utility/Enums.hpp>

namespace Nz::ShaderAst
{
	class NAZARA_SHADER_API AstValidator : public AstRecursiveVisitor
	{
		public:
			inline AstValidator();
			AstValidator(const AstValidator&) = delete;
			AstValidator(AstValidator&&) = delete;
			~AstValidator() = default;

			bool Validate(StatementPtr& node, std::string* error = nullptr, AstCache* cache = nullptr);

		private:
			Expression& MandatoryExpr(ExpressionPtr& node);
			Statement& MandatoryStatement(StatementPtr& node);
			void TypeMustMatch(ExpressionPtr& left, ExpressionPtr& right);
			void TypeMustMatch(const ShaderExpressionType& left, const ShaderExpressionType& right);

			ShaderExpressionType CheckField(const std::string& structName, const std::string* memberIdentifier, std::size_t remainingMembers);

			AstCache::Scope& EnterScope();
			void ExitScope();

			void RegisterExpressionType(Expression& node, ShaderExpressionType expressionType);
			void RegisterScope(Node& node);

			void Visit(AccessMemberExpression& node) override;
			void Visit(AssignExpression& node) override;
			void Visit(BinaryExpression& node) override;
			void Visit(CastExpression& node) override;
			void Visit(ConditionalExpression& node) override;
			void Visit(ConstantExpression& node) override;
			void Visit(IdentifierExpression& node) override;
			void Visit(IntrinsicExpression& node) override;
			void Visit(SwizzleExpression& node) override;

			void Visit(BranchStatement& node) override;
			void Visit(ConditionalStatement& node) override;
			void Visit(DeclareFunctionStatement& node) override;
			void Visit(DeclareStructStatement& node) override;
			void Visit(DeclareVariableStatement& node) override;
			void Visit(ExpressionStatement& node) override;
			void Visit(MultiStatement& node) override;
			void Visit(ReturnStatement& node) override;

			struct Context;

			Context* m_context;
	};

	NAZARA_SHADER_API bool ValidateAst(StatementPtr& node, std::string* error = nullptr, AstCache* cache = nullptr);
}

#include <Nazara/Shader/ShaderAstValidator.inl>

#endif
