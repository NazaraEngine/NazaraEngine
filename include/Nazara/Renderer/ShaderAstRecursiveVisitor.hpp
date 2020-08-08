// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_RECURSIVE_VISITOR_HPP
#define NAZARA_SHADER_RECURSIVE_VISITOR_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/ShaderAstVisitor.hpp>

namespace Nz
{
	class NAZARA_RENDERER_API ShaderAstRecursiveVisitor : public ShaderAstVisitor
	{
		public:
			ShaderAstRecursiveVisitor() = default;
			~ShaderAstRecursiveVisitor() = default;

			using ShaderAstVisitor::Visit;

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
	};
}

#include <Nazara/Renderer/ShaderAstRecursiveVisitor.inl>

#endif
