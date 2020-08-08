// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADERVALIDATOR_HPP
#define NAZARA_SHADERVALIDATOR_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/ByteArray.hpp>
#include <Nazara/Core/ByteStream.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/ShaderAst.hpp>
#include <Nazara/Renderer/ShaderAstRecursiveVisitor.hpp>
#include <Nazara/Renderer/ShaderVarVisitor.hpp>

namespace Nz
{
	class NAZARA_RENDERER_API ShaderAstValidator : public ShaderAstRecursiveVisitor, public ShaderVarVisitor
	{
		public:
			inline ShaderAstValidator(const ShaderAst& shader);
			ShaderAstValidator(const ShaderAstValidator&) = delete;
			ShaderAstValidator(ShaderAstValidator&&) = delete;
			~ShaderAstValidator() = default;

			bool Validate(std::string* error = nullptr);

		private:
			const ShaderNodes::ExpressionPtr& MandatoryExpr(const ShaderNodes::ExpressionPtr& node);
			const ShaderNodes::NodePtr& MandatoryNode(const ShaderNodes::NodePtr& node);
			void TypeMustMatch(const ShaderNodes::ExpressionPtr& left, const ShaderNodes::ExpressionPtr& right);
			void TypeMustMatch(const ShaderExpressionType& left, const ShaderExpressionType& right);

			using ShaderAstRecursiveVisitor::Visit;
			void Visit(ShaderNodes::AccessMember& node) override;
			void Visit(ShaderNodes::AssignOp& node) override;
			void Visit(ShaderNodes::BinaryOp& node) override;
			void Visit(ShaderNodes::Branch& node) override;
			void Visit(ShaderNodes::Cast& node) override;
			void Visit(ShaderNodes::Constant& node) override;
			void Visit(ShaderNodes::DeclareVariable& node) override;
			void Visit(ShaderNodes::ExpressionStatement& node) override;
			void Visit(ShaderNodes::Identifier& node) override;
			void Visit(ShaderNodes::IntrinsicCall& node) override;
			void Visit(ShaderNodes::Sample2D& node) override;
			void Visit(ShaderNodes::StatementBlock& node) override;
			void Visit(ShaderNodes::SwizzleOp& node) override;

			using ShaderVarVisitor::Visit;
			void Visit(ShaderNodes::BuiltinVariable& var) override;
			void Visit(ShaderNodes::InputVariable& var) override;
			void Visit(ShaderNodes::LocalVariable& var) override;
			void Visit(ShaderNodes::OutputVariable& var) override;
			void Visit(ShaderNodes::ParameterVariable& var) override;
			void Visit(ShaderNodes::UniformVariable& var) override;

			struct Context;

			const ShaderAst& m_shader;
			Context* m_context;
	};

	NAZARA_RENDERER_API bool ValidateShader(const ShaderAst& shader, std::string* error = nullptr);
}

#include <Nazara/Renderer/ShaderAstValidator.inl>

#endif
