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
	class NAZARA_SHADER_API TransformVisitor : AstCloner
	{
		public:
			inline TransformVisitor();
			TransformVisitor(const TransformVisitor&) = delete;
			TransformVisitor(TransformVisitor&&) = delete;
			~TransformVisitor() = default;

			StatementPtr Transform(StatementPtr& statement);

			TransformVisitor& operator=(const TransformVisitor&) = delete;
			TransformVisitor& operator=(TransformVisitor&&) = delete;

		private:
			struct Identifier;

			ExpressionPtr Clone(AccessMemberIdentifierExpression& node) override;
			ExpressionPtr Clone(CastExpression& node) override;
			ExpressionPtr Clone(IdentifierExpression& node) override;
			ExpressionPtr CloneExpression(ExpressionPtr& expr) override;

			inline const Identifier* FindIdentifier(const std::string_view& identifierName) const;

			void PushScope();
			void PopScope();

			inline std::size_t RegisterFunction(std::string name);
			inline std::size_t RegisterStruct(std::string name, StructDescription description);
			inline std::size_t RegisterVariable(std::string name);

			ExpressionType ResolveType(const ExpressionType& exprType);

			using AstCloner::Visit;
			void Visit(BranchStatement& node) override;
			void Visit(ConditionalStatement& node) override;
			void Visit(DeclareExternalStatement& node) override;
			void Visit(DeclareFunctionStatement& node) override;
			void Visit(DeclareStructStatement& node) override;
			void Visit(DeclareVariableStatement& node) override;
			void Visit(MultiStatement& node) override;

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

		private:
			std::size_t m_nextFuncIndex;
			std::size_t m_nextVarIndex;
			std::vector<Identifier> m_identifiersInScope;
			std::vector<StructDescription> m_structs;
			std::vector<std::size_t> m_scopeSizes;
	};
}

#include <Nazara/Shader/Ast/TransformVisitor.inl>

#endif
