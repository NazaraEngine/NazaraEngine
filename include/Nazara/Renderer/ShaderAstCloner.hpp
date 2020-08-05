// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADERASTCLONER_HPP
#define NAZARA_SHADERASTCLONER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/ShaderAstVisitor.hpp>
#include <Nazara/Renderer/ShaderVarVisitor.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_RENDERER_API ShaderAstCloner : public ShaderAstVisitor, public ShaderVarVisitor
	{
		public:
			ShaderAstCloner() = default;
			ShaderAstCloner(const ShaderAstCloner&) = default;
			ShaderAstCloner(ShaderAstCloner&&) = default;
			~ShaderAstCloner() = default;

			ShaderNodes::StatementPtr Clone(const ShaderNodes::StatementPtr& statement);

			ShaderAstCloner& operator=(const ShaderAstCloner&) = default;
			ShaderAstCloner& operator=(ShaderAstCloner&&) = default;

		private:
			void Visit(const ShaderNodes::ExpressionPtr& expr);
			void Visit(const ShaderNodes::StatementPtr& statement);

			void Visit(const ShaderNodes::AccessMember& node) override;
			void Visit(const ShaderNodes::AssignOp& node) override;
			void Visit(const ShaderNodes::BinaryOp& node) override;
			void Visit(const ShaderNodes::Branch& node) override;
			void Visit(const ShaderNodes::Cast& node) override;
			void Visit(const ShaderNodes::Constant& node) override;
			void Visit(const ShaderNodes::DeclareVariable& node) override;
			void Visit(const ShaderNodes::ExpressionStatement& node) override;
			void Visit(const ShaderNodes::Identifier& node) override;
			void Visit(const ShaderNodes::IntrinsicCall& node) override;
			void Visit(const ShaderNodes::Sample2D& node) override;
			void Visit(const ShaderNodes::StatementBlock& node) override;
			void Visit(const ShaderNodes::SwizzleOp& node) override;

			using ShaderVarVisitor::Visit;
			void Visit(const ShaderNodes::BuiltinVariable& var) override;
			void Visit(const ShaderNodes::InputVariable& var) override;
			void Visit(const ShaderNodes::LocalVariable& var) override;
			void Visit(const ShaderNodes::OutputVariable& var) override;
			void Visit(const ShaderNodes::ParameterVariable& var) override;
			void Visit(const ShaderNodes::UniformVariable& var) override;

			void PushExpression(ShaderNodes::ExpressionPtr expression);
			void PushStatement(ShaderNodes::StatementPtr statement);
			void PushVariable(ShaderNodes::VariablePtr variable);

			ShaderNodes::ExpressionPtr PopExpression();
			ShaderNodes::StatementPtr PopStatement();
			ShaderNodes::VariablePtr PopVariable();

			std::vector<ShaderNodes::ExpressionPtr> m_expressionStack;
			std::vector<ShaderNodes::StatementPtr>  m_statementStack;
			std::vector<ShaderNodes::VariablePtr>   m_variableStack;
	};
}

#include <Nazara/Renderer/ShaderAstCloner.inl>

#endif
