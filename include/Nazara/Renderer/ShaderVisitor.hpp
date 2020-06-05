// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADERVISITOR_HPP
#define NAZARA_SHADERVISITOR_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/ShaderAst.hpp>
#include <unordered_set>

namespace Nz
{
	class NAZARA_RENDERER_API ShaderVisitor
	{
		public:
			ShaderVisitor() = default;
			ShaderVisitor(const ShaderVisitor&) = delete;
			ShaderVisitor(ShaderVisitor&&) = delete;
			virtual ~ShaderVisitor();

			void EnableCondition(const String& name, bool cond);

			bool IsConditionEnabled(const String& name) const;

			virtual void Visit(const ShaderAst::AssignOp& node) = 0;
			virtual void Visit(const ShaderAst::BinaryFunc& node) = 0;
			virtual void Visit(const ShaderAst::BinaryOp& node) = 0;
			virtual void Visit(const ShaderAst::Branch& node) = 0;
			virtual void Visit(const ShaderAst::BuiltinVariable& node) = 0;
			virtual void Visit(const ShaderAst::Cast& node) = 0;
			virtual void Visit(const ShaderAst::Constant& node) = 0;
			virtual void Visit(const ShaderAst::DeclareVariable& node) = 0;
			virtual void Visit(const ShaderAst::ExpressionStatement& node) = 0;
			virtual void Visit(const ShaderAst::NamedVariable& node) = 0;
			void Visit(const ShaderAst::NodePtr& node);
			virtual void Visit(const ShaderAst::Sample2D& node) = 0;
			virtual void Visit(const ShaderAst::StatementBlock& node) = 0;
			virtual void Visit(const ShaderAst::SwizzleOp& node) = 0;

		private:
			std::unordered_set<String> m_conditions;
	};
}

#endif
