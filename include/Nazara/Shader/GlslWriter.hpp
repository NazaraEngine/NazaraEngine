// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GLSLWRITER_HPP
#define NAZARA_GLSLWRITER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/ShaderAstScopedVisitor.hpp>
#include <Nazara/Shader/ShaderWriter.hpp>
#include <set>
#include <sstream>
#include <string>

namespace Nz
{
	class NAZARA_SHADER_API GlslWriter : public ShaderWriter, public ShaderAst::AstScopedVisitor
	{
		public:
			struct Environment;
			using ExtSupportCallback = std::function<bool(const std::string_view& name)>;

			GlslWriter();
			GlslWriter(const GlslWriter&) = delete;
			GlslWriter(GlslWriter&&) = delete;
			~GlslWriter() = default;

			std::string Generate(ShaderStageType shaderStage, ShaderAst::StatementPtr& shader, const States& conditions = {});

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
			void Append(const ShaderAst::ExpressionType& type);
			void Append(ShaderAst::BuiltinEntry builtin);
			void Append(const ShaderAst::IdentifierType& identifierType);
			void Append(const ShaderAst::MatrixType& matrixType);
			void Append(ShaderAst::MemoryLayout layout);
			void Append(ShaderAst::NoType);
			void Append(ShaderAst::PrimitiveType type);
			void Append(const ShaderAst::SamplerType& samplerType);
			void Append(const ShaderAst::StructType& structType);
			void Append(const ShaderAst::UniformType& uniformType);
			void Append(const ShaderAst::VectorType& vecType);
			template<typename T> void Append(const T& param);
			template<typename T1, typename T2, typename... Args> void Append(const T1& firstParam, const T2& secondParam, Args&&... params);
			void AppendCommentSection(const std::string& section);
			void AppendEntryPoint(ShaderStageType shaderStage, ShaderAst::StatementPtr& shader);
			void AppendField(const std::string& structName, const std::string* memberIdentifier, std::size_t remainingMembers);
			void AppendLine(const std::string& txt = {});
			template<typename... Args> void AppendLine(Args&&... params);

			void EnterScope();
			void LeaveScope(bool skipLine = true);

			void Visit(ShaderAst::ExpressionPtr& expr, bool encloseIfRequired = false);

			void Visit(ShaderAst::AccessMemberIdentifierExpression& node) override;
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
			void Visit(ShaderAst::DeclareExternalStatement& node) override;
			void Visit(ShaderAst::DeclareFunctionStatement& node) override;
			void Visit(ShaderAst::DeclareStructStatement& node) override;
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
