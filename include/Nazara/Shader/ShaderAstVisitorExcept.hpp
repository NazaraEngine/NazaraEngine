// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADERASTVISITOREXCEPT_HPP
#define NAZARA_SHADERASTVISITOREXCEPT_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/ShaderAstVisitor.hpp>

namespace Nz
{
	class NAZARA_SHADER_API ShaderAstVisitorExcept : public ShaderAstVisitor
	{
		public:
			using ShaderAstVisitor::Visit;
			void Visit(ShaderNodes::AccessMember& node) override;
			void Visit(ShaderNodes::AssignOp& node) override;
			void Visit(ShaderNodes::BinaryOp& node) override;
			void Visit(ShaderNodes::Branch& node) override;
			void Visit(ShaderNodes::Cast& node) override;
			void Visit(ShaderNodes::ConditionalExpression& node) override;
			void Visit(ShaderNodes::ConditionalStatement& node) override;
			void Visit(ShaderNodes::Constant& node) override;
			void Visit(ShaderNodes::DeclareVariable& node) override;
			void Visit(ShaderNodes::Discard& node) override;
			void Visit(ShaderNodes::ExpressionStatement& node) override;
			void Visit(ShaderNodes::Identifier& node) override;
			void Visit(ShaderNodes::IntrinsicCall& node) override;
			void Visit(ShaderNodes::NoOp& node) override;
			void Visit(ShaderNodes::ReturnStatement& node) override;
			void Visit(ShaderNodes::Sample2D& node) override;
			void Visit(ShaderNodes::StatementBlock& node) override;
			void Visit(ShaderNodes::SwizzleOp& node) override;
	};
}

#endif
