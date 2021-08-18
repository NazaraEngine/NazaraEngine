// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_RECURSIVE_VISITOR_HPP
#define NAZARA_SHADER_RECURSIVE_VISITOR_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/Ast/AstExpressionVisitor.hpp>
#include <Nazara/Shader/Ast/AstStatementVisitor.hpp>

namespace Nz::ShaderAst
{
	class NAZARA_SHADER_API AstRecursiveVisitor : public AstExpressionVisitor, public AstStatementVisitor
	{
		public:
			AstRecursiveVisitor() = default;
			~AstRecursiveVisitor() = default;

			void Visit(AccessIdentifierExpression& node) override;
			void Visit(AccessIndexExpression& node) override;
			void Visit(AssignExpression& node) override;
			void Visit(BinaryExpression& node) override;
			void Visit(CallFunctionExpression& node) override;
			void Visit(CallMethodExpression& node) override;
			void Visit(CastExpression& node) override;
			void Visit(ConditionalExpression& node) override;
			void Visit(ConstantValueExpression& node) override;
			void Visit(ConstantExpression& node) override;
			void Visit(IdentifierExpression& node) override;
			void Visit(IntrinsicExpression& node) override;
			void Visit(SwizzleExpression& node) override;
			void Visit(VariableExpression& node) override;
			void Visit(UnaryExpression& node) override;

			void Visit(BranchStatement& node) override;
			void Visit(ConditionalStatement& node) override;
			void Visit(DeclareConstStatement& node) override;
			void Visit(DeclareExternalStatement& node) override;
			void Visit(DeclareFunctionStatement& node) override;
			void Visit(DeclareOptionStatement& node) override;
			void Visit(DeclareStructStatement& node) override;
			void Visit(DeclareVariableStatement& node) override;
			void Visit(DiscardStatement& node) override;
			void Visit(ExpressionStatement& node) override;
			void Visit(MultiStatement& node) override;
			void Visit(NoOpStatement& node) override;
			void Visit(ReturnStatement& node) override;
	};
}

#include <Nazara/Shader/Ast/AstRecursiveVisitor.inl>

#endif
