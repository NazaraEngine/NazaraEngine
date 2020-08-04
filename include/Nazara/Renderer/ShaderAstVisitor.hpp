// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADERVISITOR_HPP
#define NAZARA_SHADERVISITOR_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/ShaderNodes.hpp>
#include <string>
#include <unordered_set>

namespace Nz
{
	class NAZARA_RENDERER_API ShaderAstVisitor
	{
		public:
			ShaderAstVisitor() = default;
			ShaderAstVisitor(const ShaderAstVisitor&) = delete;
			ShaderAstVisitor(ShaderAstVisitor&&) = delete;
			virtual ~ShaderAstVisitor();

			void EnableCondition(const std::string& name, bool cond);

			bool IsConditionEnabled(const std::string& name) const;

			virtual void Visit(const ShaderNodes::AccessMember& node) = 0;
			virtual void Visit(const ShaderNodes::AssignOp& node) = 0;
			virtual void Visit(const ShaderNodes::BinaryOp& node) = 0;
			virtual void Visit(const ShaderNodes::Branch& node) = 0;
			virtual void Visit(const ShaderNodes::Cast& node) = 0;
			virtual void Visit(const ShaderNodes::Constant& node) = 0;
			virtual void Visit(const ShaderNodes::DeclareVariable& node) = 0;
			virtual void Visit(const ShaderNodes::ExpressionStatement& node) = 0;
			virtual void Visit(const ShaderNodes::Identifier& node) = 0;
			virtual void Visit(const ShaderNodes::IntrinsicCall& node) = 0;
			void Visit(const ShaderNodes::NodePtr& node);
			virtual void Visit(const ShaderNodes::Sample2D& node) = 0;
			virtual void Visit(const ShaderNodes::StatementBlock& node) = 0;
			virtual void Visit(const ShaderNodes::SwizzleOp& node) = 0;

		private:
			std::unordered_set<std::string> m_conditions;
	};
}

#endif
