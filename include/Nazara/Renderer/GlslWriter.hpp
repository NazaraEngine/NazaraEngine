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
#include <Nazara/Renderer/ShaderVisitor.hpp>
#include <Nazara/Renderer/ShaderWriter.hpp>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>

namespace Nz
{
	class NAZARA_RENDERER_API GlslWriter : public ShaderWriter, public ShaderVarVisitor, public ShaderVisitor
	{
		public:
			GlslWriter();
			GlslWriter(const GlslWriter&) = delete;
			GlslWriter(GlslWriter&&) = delete;
			~GlslWriter() = default;

			std::string Generate(const ShaderAst& shader) override;

			void SetGlslVersion(unsigned int version);

		private:
			void Append(ShaderNodes::BuiltinEntry builtin);
			void Append(ShaderNodes::ExpressionType type);
			template<typename T> void Append(const T& param);
			void AppendCommentSection(const std::string& section);
			void AppendFunction(const ShaderAst::Function& func);
			void AppendFunctionPrototype(const ShaderAst::Function& func);
			void AppendLine(const std::string& txt = {});

			template<typename T> void DeclareVariables(const std::vector<T>& variables, const std::string& keyword = {}, const std::string& section = {});

			void EnterScope();
			void LeaveScope();

			using ShaderVarVisitor::Visit;
			using ShaderVisitor::Visit;
			void Visit(const ShaderNodes::AssignOp& node) override;
			void Visit(const ShaderNodes::Branch& node) override;
			void Visit(const ShaderNodes::BinaryOp& node) override;
			void Visit(const ShaderNodes::BuiltinVariable& var) override;
			void Visit(const ShaderNodes::Cast& node) override;
			void Visit(const ShaderNodes::Constant& node) override;
			void Visit(const ShaderNodes::DeclareVariable& node) override;
			void Visit(const ShaderNodes::ExpressionStatement& node) override;
			void Visit(const ShaderNodes::Identifier& node) override;
			void Visit(const ShaderNodes::InputVariable& var) override;
			void Visit(const ShaderNodes::IntrinsicCall& node) override;
			void Visit(const ShaderNodes::LocalVariable& var) override;
			void Visit(const ShaderNodes::ParameterVariable& var) override;
			void Visit(const ShaderNodes::OutputVariable& var) override;
			void Visit(const ShaderNodes::Sample2D& node) override;
			void Visit(const ShaderNodes::StatementBlock& node) override;
			void Visit(const ShaderNodes::SwizzleOp& node) override;
			void Visit(const ShaderNodes::UniformVariable& var) override;

			static bool HasExplicitBinding(const ShaderAst& shader);
			static bool HasExplicitLocation(const ShaderAst& shader);

			struct Context
			{
				const ShaderAst::Function* currentFunction = nullptr;
			};

			struct State
			{
				std::stringstream stream;
				unsigned int indentLevel = 0;
			};

			Context m_context;
			State* m_currentState;
			unsigned int m_glslVersion;
	};
}

#include <Nazara/Renderer/GlslWriter.inl>

#endif // NAZARA_GLSLWRITER_HPP
