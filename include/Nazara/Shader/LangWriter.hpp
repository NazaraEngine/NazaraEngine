// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_LANGWRITER_HPP
#define NAZARA_SHADER_LANGWRITER_HPP

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

			std::string Generate(ShaderAst::Statement& shader, const States& conditions = {});

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
			struct SetAttribute;

			void Append(const ShaderAst::ArrayType& type);
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
			template<typename T> void AppendAttributesInternal(bool& first, const T& param);
			template<typename T1, typename T2, typename... Rest> void AppendAttributesInternal(bool& first, const T1& firstParam, const T2& secondParam, Rest&&... params);
			void AppendAttribute(BindingAttribute binding);
			void AppendAttribute(BuiltinAttribute builtin);
			void AppendAttribute(DepthWriteAttribute depthWrite);
			void AppendAttribute(EarlyFragmentTestsAttribute earlyFragmentTests);
			void AppendAttribute(EntryAttribute entry);
			void AppendAttribute(LayoutAttribute layout);
			void AppendAttribute(LocationAttribute location);
			void AppendAttribute(SetAttribute location);
			void AppendCommentSection(const std::string& section);
			void AppendField(std::size_t structIndex, const ShaderAst::ExpressionPtr* memberIndices, std::size_t remainingMembers);
			void AppendHeader();
			void AppendLine(const std::string& txt = {});
			template<typename... Args> void AppendLine(Args&&... params);
			void AppendStatementList(std::vector<ShaderAst::StatementPtr>& statements);

			void EnterScope();
			void LeaveScope(bool skipLine = true);

			void RegisterConstant(std::size_t constantIndex, std::string constantName);
			void RegisterStruct(std::size_t structIndex, ShaderAst::StructDescription* desc);
			void RegisterVariable(std::size_t varIndex, std::string varName);

			void Visit(ShaderAst::ExpressionPtr& expr, bool encloseIfRequired = false);

			void Visit(ShaderAst::AccessIndexExpression& node) override;
			void Visit(ShaderAst::AssignExpression& node) override;
			void Visit(ShaderAst::BinaryExpression& node) override;
			void Visit(ShaderAst::CastExpression& node) override;
			void Visit(ShaderAst::ConditionalExpression& node) override;
			void Visit(ShaderAst::ConstantValueExpression& node) override;
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
			void Visit(ShaderAst::WhileStatement& node) override;

			struct State;

			Environment m_environment;
			State* m_currentState;
	};
}

#include <Nazara/Shader/LangWriter.inl>

#endif // NAZARA_SHADER_LANGWRITER_HPP
