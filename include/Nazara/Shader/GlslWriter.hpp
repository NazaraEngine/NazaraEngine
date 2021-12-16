// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_GLSLWRITER_HPP
#define NAZARA_SHADER_GLSLWRITER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/ShaderWriter.hpp>
#include <Nazara/Shader/Ast/AstExpressionVisitorExcept.hpp>
#include <Nazara/Shader/Ast/AstStatementVisitorExcept.hpp>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace Nz
{
	class NAZARA_SHADER_API GlslWriter : public ShaderWriter, public ShaderAst::ExpressionVisitorExcept, public ShaderAst::StatementVisitorExcept
	{
		public:
			using BindingMapping = std::unordered_map<UInt64 /* set | binding */, unsigned /*glBinding*/>;
			struct Environment;
			using ExtSupportCallback = std::function<bool(const std::string_view& name)>;

			inline GlslWriter();
			GlslWriter(const GlslWriter&) = delete;
			GlslWriter(GlslWriter&&) = delete;
			~GlslWriter() = default;

			inline std::string Generate(ShaderAst::Statement& shader, const BindingMapping& bindingMapping = {}, const States& states = {});
			std::string Generate(std::optional<ShaderStageType> shaderStage, ShaderAst::Statement& shader, const BindingMapping& bindingMapping = {}, const States& states = {});

			void SetEnv(Environment environment);

			struct Environment
			{
				ExtSupportCallback extCallback;
				unsigned int glMajorVersion = 3;
				unsigned int glMinorVersion = 0;
				bool glES = false;
				bool flipYPosition = false;
				bool remapZPosition = false;
			};

			static const char* GetFlipYUniformName();
			static ShaderAst::StatementPtr Sanitize(ShaderAst::Statement& ast, std::unordered_map<std::size_t, ShaderAst::ConstantValue> optionValues, std::string* error = nullptr);

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
			void AppendFunctionDeclaration(const ShaderAst::DeclareFunctionStatement& node, bool forward = false);
			void AppendField(std::size_t structIndex, const ShaderAst::ExpressionPtr* memberIndices, std::size_t remainingMembers);
			void AppendHeader();
			void AppendLine(const std::string& txt = {});
			template<typename... Args> void AppendLine(Args&&... params);
			void AppendStatementList(std::vector<ShaderAst::StatementPtr>& statements);

			void EnterScope();
			void LeaveScope(bool skipLine = true);

			void HandleEntryPoint(ShaderAst::DeclareFunctionStatement& node);
			void HandleInOut();

			void RegisterStruct(std::size_t structIndex, ShaderAst::StructDescription* desc);
			void RegisterVariable(std::size_t varIndex, std::string varName);

			void Visit(ShaderAst::ExpressionPtr& expr, bool encloseIfRequired = false);

			void Visit(ShaderAst::AccessIndexExpression& node) override;
			void Visit(ShaderAst::AssignExpression& node) override;
			void Visit(ShaderAst::BinaryExpression& node) override;
			void Visit(ShaderAst::CallFunctionExpression& node) override;
			void Visit(ShaderAst::CastExpression& node) override;
			void Visit(ShaderAst::ConstantValueExpression& node) override;
			void Visit(ShaderAst::IntrinsicExpression& node) override;
			void Visit(ShaderAst::SwizzleExpression& node) override;
			void Visit(ShaderAst::VariableExpression& node) override;
			void Visit(ShaderAst::UnaryExpression& node) override;

			void Visit(ShaderAst::BranchStatement& node) override;
			void Visit(ShaderAst::DeclareConstStatement& node) override;
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

			static bool HasExplicitBinding(ShaderAst::StatementPtr& shader);
			static bool HasExplicitLocation(ShaderAst::StatementPtr& shader);

			struct State;

			Environment m_environment;
			State* m_currentState;
	};
}

#include <Nazara/Shader/GlslWriter.inl>

#endif // NAZARA_SHADER_GLSLWRITER_HPP
