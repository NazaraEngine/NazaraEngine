// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GLSLWRITER_HPP
#define NAZARA_GLSLWRITER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/ShaderAst.hpp>
#include <Nazara/Renderer/ShaderVarVisitor.hpp>
#include <Nazara/Renderer/ShaderAstVisitor.hpp>
#include <Nazara/Renderer/ShaderWriter.hpp>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>

namespace Nz
{
	class NAZARA_RENDERER_API GlslWriter : public ShaderWriter, public ShaderVarVisitor, public ShaderAstVisitor
	{
		public:
			struct Environment;
			using ExtSupportCallback = std::function<bool(const std::string_view& name)>;

			GlslWriter();
			GlslWriter(const GlslWriter&) = delete;
			GlslWriter(GlslWriter&&) = delete;
			~GlslWriter() = default;

			std::string Generate(const ShaderAst& shader) override;

			void SetEnv(Environment environment);

			struct Environment
			{
				ExtSupportCallback extCallback;
				unsigned int glMajorVersion = 3;
				unsigned int glMinorVersion = 0;
				bool glES = false;
			};

		private:
			void Append(ShaderExpressionType type);
			void Append(ShaderNodes::BuiltinEntry builtin);
			void Append(ShaderNodes::BasicType type);
			void Append(ShaderNodes::MemoryLayout layout);
			template<typename T> void Append(const T& param);
			void AppendCommentSection(const std::string& section);
			void AppendFunction(const ShaderAst::Function& func);
			void AppendFunctionPrototype(const ShaderAst::Function& func);
			void AppendLine(const std::string& txt = {});

			template<typename T> void DeclareVariables(const ShaderAst& shader, const std::vector<T>& variables, const std::string& keyword = {}, const std::string& section = {});

			void EnterScope();
			void LeaveScope();

			using ShaderVarVisitor::Visit;
			using ShaderAstVisitor::Visit;
			void Visit(ShaderNodes::ExpressionPtr& expr, bool encloseIfRequired = false);
			void Visit(ShaderNodes::AccessMember& node) override;
			void Visit(ShaderNodes::AssignOp& node) override;
			void Visit(ShaderNodes::Branch& node) override;
			void Visit(ShaderNodes::BinaryOp& node) override;
			void Visit(ShaderNodes::BuiltinVariable& var) override;
			void Visit(ShaderNodes::Cast& node) override;
			void Visit(ShaderNodes::Constant& node) override;
			void Visit(ShaderNodes::DeclareVariable& node) override;
			void Visit(ShaderNodes::ExpressionStatement& node) override;
			void Visit(ShaderNodes::Identifier& node) override;
			void Visit(ShaderNodes::InputVariable& var) override;
			void Visit(ShaderNodes::IntrinsicCall& node) override;
			void Visit(ShaderNodes::LocalVariable& var) override;
			void Visit(ShaderNodes::ParameterVariable& var) override;
			void Visit(ShaderNodes::OutputVariable& var) override;
			void Visit(ShaderNodes::Sample2D& node) override;
			void Visit(ShaderNodes::StatementBlock& node) override;
			void Visit(ShaderNodes::SwizzleOp& node) override;
			void Visit(ShaderNodes::UniformVariable& var) override;

			static bool HasExplicitBinding(const ShaderAst& shader);
			static bool HasExplicitLocation(const ShaderAst& shader);

			struct Context
			{
				const ShaderAst* shader = nullptr;
				const ShaderAst::Function* currentFunction = nullptr;
			};

			struct State
			{
				std::stringstream stream;
				unsigned int indentLevel = 0;
			};

			Context m_context;
			Environment m_environment;
			State* m_currentState;
	};
}

#include <Nazara/Renderer/GlslWriter.inl>

#endif // NAZARA_GLSLWRITER_HPP
