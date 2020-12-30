// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADERASTVISITOR_HPP
#define NAZARA_SHADERASTVISITOR_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/ShaderNodes.hpp>

namespace Nz
{
	class NAZARA_SHADER_API ShaderAstVisitor
	{
		public:
			ShaderAstVisitor() = default;
			ShaderAstVisitor(const ShaderAstVisitor&) = delete;
			ShaderAstVisitor(ShaderAstVisitor&&) = delete;
			virtual ~ShaderAstVisitor();

			void Visit(const ShaderNodes::NodePtr& node);
			virtual void Visit(ShaderNodes::AccessMember& node) = 0;
			virtual void Visit(ShaderNodes::AssignOp& node) = 0;
			virtual void Visit(ShaderNodes::BinaryOp& node) = 0;
			virtual void Visit(ShaderNodes::Branch& node) = 0;
			virtual void Visit(ShaderNodes::Cast& node) = 0;
			virtual void Visit(ShaderNodes::ConditionalExpression& node) = 0;
			virtual void Visit(ShaderNodes::ConditionalStatement& node) = 0;
			virtual void Visit(ShaderNodes::Constant& node) = 0;
			virtual void Visit(ShaderNodes::DeclareVariable& node) = 0;
			virtual void Visit(ShaderNodes::Discard& node) = 0;
			virtual void Visit(ShaderNodes::ExpressionStatement& node) = 0;
			virtual void Visit(ShaderNodes::Identifier& node) = 0;
			virtual void Visit(ShaderNodes::IntrinsicCall& node) = 0;
			virtual void Visit(ShaderNodes::Sample2D& node) = 0;
			virtual void Visit(ShaderNodes::StatementBlock& node) = 0;
			virtual void Visit(ShaderNodes::SwizzleOp& node) = 0;

			ShaderAstVisitor& operator=(const ShaderAstVisitor&) = delete;
			ShaderAstVisitor& operator=(ShaderAstVisitor&&) = delete;
	};
}

#endif
