// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GLSLWRITER_HPP
#define NAZARA_GLSLWRITER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/ShaderWriter.hpp>
#include <map>
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

			void Write(const ShaderAst::AssignOp& node) override;
			void Write(const ShaderAst::Branch& node) override;
			void Write(const ShaderAst::BinaryOp& node) override;
			void Write(const ShaderAst::BuiltinVariable& node) override;
			void Write(const ShaderAst::Constant& node) override;
			void Write(const ShaderAst::ExpressionStatement& node) override;
			void Write(const ShaderAst::NamedVariable& node) override;
			void Write(const ShaderAst::NodePtr& node) override;
			void Write(const ShaderAst::StatementBlock& node) override;

		private:
			struct Function;
			using VariableContainer = std::set<std::pair<ShaderAst::ExpressionType, String>>;

			void Append(ShaderAst::Builtin builtin);
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
				VariableContainer variables;
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
