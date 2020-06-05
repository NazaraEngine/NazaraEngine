// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GLSLWRITER_HPP
#define NAZARA_GLSLWRITER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/ShaderWriter.hpp>
#include <set>
#include <unordered_map>

namespace Nz
{
	class NAZARA_RENDERER_API GlslWriter : public ShaderWriter
	{
		public:
			GlslWriter();
			GlslWriter(const GlslWriter&) = delete;
			GlslWriter(GlslWriter&&) = delete;
			~GlslWriter() = default;

			Nz::String Generate(const ShaderAst::StatementPtr& node) override;

			void RegisterFunction(const String& name, ShaderAst::StatementPtr statement, std::initializer_list<ShaderAst::NamedVariablePtr> parameters, ShaderAst::ExpressionType ret) override;
			void RegisterVariable(ShaderAst::VariableType kind, const String& name, ShaderAst::ExpressionType type) override;

			void SetGlslVersion(unsigned int version);

			using ShaderWriter::Visit;
			void Visit(const ShaderAst::AssignOp& node) override;
			void Visit(const ShaderAst::Branch& node) override;
			void Visit(const ShaderAst::BinaryFunc& node) override;
			void Visit(const ShaderAst::BinaryOp& node) override;
			void Visit(const ShaderAst::BuiltinVariable& node) override;
			void Visit(const ShaderAst::Cast& node) override;
			void Visit(const ShaderAst::Constant& node) override;
			void Visit(const ShaderAst::DeclareVariable& node) override;
			void Visit(const ShaderAst::ExpressionStatement& node) override;
			void Visit(const ShaderAst::NamedVariable& node) override;
			void Visit(const ShaderAst::Sample2D& node) override;
			void Visit(const ShaderAst::StatementBlock& node) override;
			void Visit(const ShaderAst::SwizzleOp& node) override;

		private:
			struct Function;
			using VariableContainer = std::set<std::pair<ShaderAst::ExpressionType, String>>;

			void Append(ShaderAst::BuiltinEntry builtin);
			void Append(ShaderAst::ExpressionType type);
			void Append(const String& txt);
			void AppendCommentSection(const String& section);
			void AppendFunction(Function& func);
			void AppendLine(const String& txt = String());

			void DeclareVariables(const VariableContainer& variables, const String& keyword = String(), const String& section = String());

			void EnterScope();
			void LeaveScope();


			struct Function
			{
				std::vector<ShaderAst::NamedVariablePtr> parameters;
				ShaderAst::ExpressionType retType;
				ShaderAst::StatementPtr node;
				String name;
			};

			struct State
			{
				VariableContainer inputs;
				VariableContainer outputs;
				VariableContainer uniforms;
				StringStream stream;
				unsigned int indentLevel = 0;
			};

			std::unordered_map<String, Function> m_functions;
			Function* m_currentFunction;
			State* m_currentState;
			unsigned int m_glslVersion;
	};
}

#endif // NAZARA_GLSLWRITER_HPP
