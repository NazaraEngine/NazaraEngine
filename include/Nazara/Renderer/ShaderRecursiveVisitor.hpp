// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_RECURSIVE_VISITOR_HPP
#define NAZARA_SHADER_RECURSIVE_VISITOR_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/ShaderVisitor.hpp>

namespace Nz
{
	class NAZARA_RENDERER_API ShaderRecursiveVisitor : public ShaderVisitor
	{
		public:
			ShaderRecursiveVisitor() = default;
			~ShaderRecursiveVisitor() = default;

			using ShaderVisitor::Visit;

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
	};
}

#include <Nazara/Renderer/ShaderRecursiveVisitor.inl>

#endif
