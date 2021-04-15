// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADERAST_TRANSFORMVISITOR_HPP
#define NAZARA_SHADERAST_TRANSFORMVISITOR_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/ShaderAstCloner.hpp>
#include <vector>

namespace Nz::ShaderAst
{
	class NAZARA_SHADER_API SanitizeVisitor final : AstCloner
	{
		public:
			inline SanitizeVisitor();
			SanitizeVisitor(const SanitizeVisitor&) = delete;
			SanitizeVisitor(SanitizeVisitor&&) = delete;
			~SanitizeVisitor() = default;

			StatementPtr Sanitize(StatementPtr& statement, std::string* error = nullptr);

			SanitizeVisitor& operator=(const SanitizeVisitor&) = delete;
			SanitizeVisitor& operator=(SanitizeVisitor&&) = delete;

		private:
			struct Identifier;

			const ExpressionType& CheckField(const ExpressionType& structType, const std::string* memberIdentifier, std::size_t remainingMembers, std::size_t* structIndices);

			using AstCloner::CloneExpression;

			ExpressionPtr Clone(AccessMemberIdentifierExpression& node) override;
			ExpressionPtr Clone(AssignExpression& node) override;
			ExpressionPtr Clone(BinaryExpression& node) override;
			ExpressionPtr Clone(CastExpression& node) override;
			ExpressionPtr Clone(ConditionalExpression& node) override;
			ExpressionPtr Clone(ConstantExpression& node) override;
			ExpressionPtr Clone(IdentifierExpression& node) override;
			ExpressionPtr Clone(IntrinsicExpression& node) override;
			ExpressionPtr Clone(SwizzleExpression& node) override;

			StatementPtr Clone(BranchStatement& node) override;
			StatementPtr Clone(ConditionalStatement& node) override;
			StatementPtr Clone(DeclareExternalStatement& node) override;
			StatementPtr Clone(DeclareFunctionStatement& node) override;
			StatementPtr Clone(DeclareStructStatement& node) override;
			StatementPtr Clone(DeclareVariableStatement& node) override;
			StatementPtr Clone(ExpressionStatement& node) override;
			StatementPtr Clone(MultiStatement& node) override;

			inline const Identifier* FindIdentifier(const std::string_view& identifierName) const;

			Expression& MandatoryExpr(ExpressionPtr& node);
			Statement& MandatoryStatement(StatementPtr& node);
			void TypeMustMatch(ExpressionPtr& left, ExpressionPtr& right);
			void TypeMustMatch(const ExpressionType& left, const ExpressionType& right);

			void PushScope();
			void PopScope();

			inline std::size_t RegisterFunction(std::string name);
			inline std::size_t RegisterStruct(std::string name, StructDescription description);
			inline std::size_t RegisterVariable(std::string name, ExpressionType type);

			std::size_t ResolveStruct(const ExpressionType& exprType);
			std::size_t ResolveStruct(const IdentifierType& identifierType);
			std::size_t ResolveStruct(const StructType& structType);
			std::size_t ResolveStruct(const UniformType& uniformType);
			ExpressionType ResolveType(const ExpressionType& exprType);

			struct Alias
			{
				std::variant<ExpressionType> value;
			};

			struct Struct
			{
				std::size_t structIndex;
			};

			struct Variable
			{
				std::size_t varIndex;
			};

			struct Identifier
			{
				std::string name;
				std::variant<Alias, Struct, Variable> value;
			};

			std::size_t m_nextFuncIndex;
			std::vector<Identifier> m_identifiersInScope;
			std::vector<StructDescription> m_structs;
			std::vector<ExpressionType> m_variables;
			std::vector<std::size_t> m_scopeSizes;

			struct Context;
			Context* m_context;
	};

	inline StatementPtr Sanitize(StatementPtr& ast, std::string* error = nullptr);
}

#include <Nazara/Shader/Ast/SanitizeVisitor.inl>

#endif
