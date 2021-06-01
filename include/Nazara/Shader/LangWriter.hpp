// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_LANGWRITER_HPP
#define NAZARA_LANGWRITER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/ShaderWriter.hpp>
#include <Nazara/Shader/Ast/AstExpressionVisitorExcept.hpp>
#include <Nazara/Shader/Ast/AstStatementVisitorExcept.hpp>
#include <set>
#include <sstream>
#include <string>

namespace Nz
{
	class NAZARA_SHADER_API LangWriter : public ShaderWriter, public ShaderAst::ExpressionVisitorExcept, public ShaderAst::StatementVisitorExcept
	{
		public:
			struct Environment;

			inline LangWriter();
			LangWriter(const LangWriter&) = delete;
			LangWriter(LangWriter&&) = delete;
			~LangWriter() = default;

			std::string Generate(ShaderAst::StatementPtr& shader, const States& conditions = {});

			void SetEnv(Environment environment);

			struct Environment
			{
			};

		private:
			struct BindingAttribute;
			struct BuiltinAttribute;
			struct DepthWriteAttribute;
			struct EarlyFragmentTestsAttribute;
			struct EntryAttribute;
			struct LayoutAttribute;
			struct LocationAttribute;

			void Append(const ShaderAst::ExpressionType& type);
			void Append(const ShaderAst::IdentifierType& identifierType);
			void Append(const ShaderAst::MatrixType& matrixType);
			void Append(ShaderAst::NoType);
			void Append(ShaderAst::PrimitiveType type);
			void Append(const ShaderAst::SamplerType& samplerType);
			void Append(const ShaderAst::StructType& structType);
			void Append(const ShaderAst::UniformType& uniformType);
			void Append(const ShaderAst::VectorType& vecType);
			template<typename T> void Append(const T& param);
			template<typename T1, typename T2, typename... Args> void Append(const T1& firstParam, const T2& secondParam, Args&&... params);
			template<typename... Args> void AppendAttributes(bool appendLine, Args&&... params);
			void AppendAttribute(BindingAttribute binding);
			void AppendAttribute(BuiltinAttribute builtin);
			void AppendAttribute(DepthWriteAttribute depthWrite);
			void AppendAttribute(EarlyFragmentTestsAttribute earlyFragmentTests);
			void AppendAttribute(EntryAttribute entry);
			void AppendAttribute(LayoutAttribute layout);
			void AppendAttribute(LocationAttribute location);
			void AppendCommentSection(const std::string& section);
			void AppendField(std::size_t structIndex, const std::size_t* memberIndices, std::size_t remainingMembers);
			void AppendHeader();
			void AppendLine(const std::string& txt = {});
			template<typename... Args> void AppendLine(Args&&... params);
			void AppendStatementList(std::vector<ShaderAst::StatementPtr>& statements);

			void EnterScope();
			void LeaveScope(bool skipLine = true);

			void RegisterOption(std::size_t optionIndex, std::string optionName);
			void RegisterStruct(std::size_t structIndex, ShaderAst::StructDescription desc);
			void RegisterVariable(std::size_t varIndex, std::string varName);

			void Visit(ShaderAst::ExpressionPtr& expr, bool encloseIfRequired = false);

			void Visit(ShaderAst::AccessMemberIndexExpression& node) override;
			void Visit(ShaderAst::AssignExpression& node) override;
			void Visit(ShaderAst::BinaryExpression& node) override;
			void Visit(ShaderAst::CastExpression& node) override;
			void Visit(ShaderAst::ConditionalExpression& node) override;
			void Visit(ShaderAst::ConstantExpression& node) override;
			void Visit(ShaderAst::IntrinsicExpression& node) override;
			void Visit(ShaderAst::SwizzleExpression& node) override;
			void Visit(ShaderAst::VariableExpression& node) override;
			void Visit(ShaderAst::UnaryExpression& node) override;

			void Visit(ShaderAst::BranchStatement& node) override;
			void Visit(ShaderAst::ConditionalStatement& node) override;
			void Visit(ShaderAst::DeclareExternalStatement& node) override;
			void Visit(ShaderAst::DeclareFunctionStatement& node) override;
			void Visit(ShaderAst::DeclareOptionStatement& node) override;
			void Visit(ShaderAst::DeclareStructStatement& node) override;
			void Visit(ShaderAst::DeclareVariableStatement& node) override;
			void Visit(ShaderAst::DiscardStatement& node) override;
			void Visit(ShaderAst::ExpressionStatement& node) override;
			void Visit(ShaderAst::MultiStatement& node) override;
			void Visit(ShaderAst::NoOpStatement& node) override;
			void Visit(ShaderAst::ReturnStatement& node) override;

			struct State;

			Environment m_environment;
			State* m_currentState;
	};
}

#include <Nazara/Shader/LangWriter.inl>

#endif 
