// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GLSLWRITER_HPP
#define NAZARA_GLSLWRITER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/ShaderAstRecursiveVisitor.hpp>
#include <Nazara/Shader/ShaderWriter.hpp>
#include <set>
#include <sstream>
#include <string>

namespace Nz
{
	class NAZARA_SHADER_API GlslWriter : public ShaderWriter, public ShaderAst::AstRecursiveVisitor
	{
		public:
			struct Environment;
			using ExtSupportCallback = std::function<bool(const std::string_view& name)>;

			GlslWriter();
			GlslWriter(const GlslWriter&) = delete;
			GlslWriter(GlslWriter&&) = delete;
			~GlslWriter() = default;

			std::string Generate(ShaderAst::StatementPtr& shader, const States& conditions = {});

			void SetEnv(Environment environment);

			struct Environment
			{
				ExtSupportCallback extCallback;
				unsigned int glMajorVersion = 3;
				unsigned int glMinorVersion = 0;
				bool glES = false;
				bool flipYPosition = false;
			};

			static const char* GetFlipYUniformName();

		private:
			void Append(ShaderAst::ShaderExpressionType type);
			void Append(ShaderAst::BuiltinEntry builtin);
			void Append(ShaderAst::BasicType type);
			void Append(ShaderAst::MemoryLayout layout);
			template<typename T> void Append(const T& param);
			void AppendCommentSection(const std::string& section);
			void AppendField(std::size_t scopeId, const std::string& structName, const std::string* memberIdentifier, std::size_t remainingMembers);
			void AppendLine(const std::string& txt = {});

			void EnterScope();
			void LeaveScope();

			void Visit(ShaderAst::ExpressionPtr& expr, bool encloseIfRequired = false);

			void Visit(ShaderAst::AccessMemberExpression& node) override;
			void Visit(ShaderAst::AssignExpression& node) override;
			void Visit(ShaderAst::BinaryExpression& node) override;
			void Visit(ShaderAst::CastExpression& node) override;
			void Visit(ShaderAst::ConditionalExpression& node) override;
			void Visit(ShaderAst::ConstantExpression& node) override;
			void Visit(ShaderAst::IdentifierExpression& node) override;
			void Visit(ShaderAst::IntrinsicExpression& node) override;
			void Visit(ShaderAst::SwizzleExpression& node) override;

			void Visit(ShaderAst::BranchStatement& node) override;
			void Visit(ShaderAst::ConditionalStatement& node) override;
			void Visit(ShaderAst::DeclareFunctionStatement& node) override;
			void Visit(ShaderAst::DeclareVariableStatement& node) override;
			void Visit(ShaderAst::DiscardStatement& node) override;
			void Visit(ShaderAst::ExpressionStatement& node) override;
			void Visit(ShaderAst::MultiStatement& node) override;
			void Visit(ShaderAst::NoOpStatement& node) override;
			void Visit(ShaderAst::ReturnStatement& node) override;

			static bool HasExplicitBinding(ShaderAst::StatementPtr& shader);
			static bool HasExplicitLocation(ShaderAst::StatementPtr& shader);

			struct State;

			Environment m_environment;
			State* m_currentState;
	};
}

#include <Nazara/Shader/GlslWriter.inl>

#endif // NAZARA_GLSLWRITER_HPP
