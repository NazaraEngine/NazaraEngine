// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADERASTCLONER_HPP
#define NAZARA_SHADERASTCLONER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/ShaderAstVisitor.hpp>
#include <Nazara/Shader/ShaderVarVisitor.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_SHADER_API ShaderAstCloner : public ShaderAstVisitor, public ShaderVarVisitor
	{
		public:
			ShaderAstCloner() = default;
			ShaderAstCloner(const ShaderAstCloner&) = delete;
			ShaderAstCloner(ShaderAstCloner&&) = delete;
			~ShaderAstCloner() = default;

			ShaderNodes::StatementPtr Clone(const ShaderNodes::StatementPtr& statement);

			ShaderAstCloner& operator=(const ShaderAstCloner&) = delete;
			ShaderAstCloner& operator=(ShaderAstCloner&&) = delete;

		protected:
			ShaderNodes::ExpressionPtr CloneExpression(const ShaderNodes::ExpressionPtr& expr);
			ShaderNodes::StatementPtr CloneStatement(const ShaderNodes::StatementPtr& statement);
			ShaderNodes::VariablePtr CloneVariable(const ShaderNodes::VariablePtr& statement);

			void Visit(ShaderNodes::AccessMember& node) override;
			void Visit(ShaderNodes::AssignOp& node) override;
			void Visit(ShaderNodes::BinaryOp& node) override;
			void Visit(ShaderNodes::Branch& node) override;
			void Visit(ShaderNodes::Cast& node) override;
			void Visit(ShaderNodes::ConditionalExpression& node) override;
			void Visit(ShaderNodes::ConditionalStatement& node) override;
			void Visit(ShaderNodes::Constant& node) override;
			void Visit(ShaderNodes::DeclareVariable& node) override;
			void Visit(ShaderNodes::ExpressionStatement& node) override;
			void Visit(ShaderNodes::Identifier& node) override;
			void Visit(ShaderNodes::IntrinsicCall& node) override;
			void Visit(ShaderNodes::Sample2D& node) override;
			void Visit(ShaderNodes::StatementBlock& node) override;
			void Visit(ShaderNodes::SwizzleOp& node) override;

			void Visit(ShaderNodes::BuiltinVariable& var) override;
			void Visit(ShaderNodes::InputVariable& var) override;
			void Visit(ShaderNodes::LocalVariable& var) override;
			void Visit(ShaderNodes::OutputVariable& var) override;
			void Visit(ShaderNodes::ParameterVariable& var) override;
			void Visit(ShaderNodes::UniformVariable& var) override;

			void PushExpression(ShaderNodes::ExpressionPtr expression);
			void PushStatement(ShaderNodes::StatementPtr statement);
			void PushVariable(ShaderNodes::VariablePtr variable);

			ShaderNodes::ExpressionPtr PopExpression();
			ShaderNodes::StatementPtr PopStatement();
			ShaderNodes::VariablePtr PopVariable();

		private:
			std::vector<ShaderNodes::ExpressionPtr> m_expressionStack;
			std::vector<ShaderNodes::StatementPtr>  m_statementStack;
			std::vector<ShaderNodes::VariablePtr>   m_variableStack;
	};
}

#include <Nazara/Shader/ShaderAstCloner.inl>

#endif
