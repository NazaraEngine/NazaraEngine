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
#include <Nazara/Renderer/ShaderVisitor.hpp>

namespace Nz::ShaderAst
{
	class NAZARA_RENDERER_API ShaderValidator : public ShaderVisitor
	{
		public:
			ShaderValidator() = default;
			ShaderValidator(const ShaderValidator&) = delete;
			ShaderValidator(ShaderValidator&&) = delete;
			~ShaderValidator() = default;

			bool Validate(const StatementPtr& shader, std::string* error = nullptr);

		private:
			const ExpressionPtr& MandatoryExpr(const ExpressionPtr& node);
			const NodePtr& MandatoryNode(const NodePtr& node);
			void TypeMustMatch(const ExpressionPtr& left, const ExpressionPtr& right);

			using ShaderVisitor::Visit;
			void Visit(const AssignOp& node) override;
			void Visit(const BinaryFunc& node) override;
			void Visit(const BinaryOp& node) override;
			void Visit(const Branch& node) override;
			void Visit(const BuiltinVariable& node) override;
			void Visit(const Cast& node) override;
			void Visit(const Constant& node) override;
			void Visit(const DeclareVariable& node) override;
			void Visit(const ExpressionStatement& node) override;
			void Visit(const NamedVariable& node) override;
			void Visit(const Sample2D& node) override;
			void Visit(const StatementBlock& node) override;
			void Visit(const SwizzleOp& node) override;
	};

	NAZARA_RENDERER_API bool Validate(const StatementPtr& shader, std::string* error = nullptr);
}

#include <Nazara/Renderer/ShaderValidator.inl>

#endif
