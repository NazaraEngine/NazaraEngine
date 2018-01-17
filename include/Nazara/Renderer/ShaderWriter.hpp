// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADERWRITER_HPP
#define NAZARA_SHADERWRITER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/ShaderAst.hpp>
#include <unordered_set>

namespace Nz
{
	class NAZARA_RENDERER_API ShaderWriter
	{
		public:
			ShaderWriter() = default;
			ShaderWriter(const ShaderWriter&) = delete;
			ShaderWriter(ShaderWriter&&) = delete;
			virtual ~ShaderWriter();

			void EnableCondition(const String& name, bool cond);

			bool IsConditionEnabled(const String& name) const;

			virtual Nz::String Generate(const ShaderAst::StatementPtr& node) = 0;

			virtual void RegisterFunction(const String& name, ShaderAst::StatementPtr node, std::initializer_list<ShaderAst::NamedVariablePtr> parameters, ShaderAst::ExpressionType ret) = 0;
			virtual void RegisterVariable(ShaderAst::VariableType kind, const String& name, ShaderAst::ExpressionType type) = 0;

			virtual void Write(const ShaderAst::AssignOp& node) = 0;
			virtual void Write(const ShaderAst::Branch& node) = 0;
			virtual void Write(const ShaderAst::BinaryOp& node) = 0;
			virtual void Write(const ShaderAst::BuiltinVariable& node) = 0;
			virtual void Write(const ShaderAst::Cast& node) = 0;
			virtual void Write(const ShaderAst::Constant& node) = 0;
			virtual void Write(const ShaderAst::ExpressionStatement& node) = 0;
			virtual void Write(const ShaderAst::NamedVariable& node) = 0;
			virtual void Write(const ShaderAst::NodePtr& node) = 0;
			virtual void Write(const ShaderAst::StatementBlock& node) = 0;
			virtual void Write(const ShaderAst::SwizzleOp& node) = 0;

		private:
			std::unordered_set<String> m_conditions;
	};
}

#endif // NAZARA_SHADERWRITER_HPP
