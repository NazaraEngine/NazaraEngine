// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_SCOPED_VISITOR_HPP
#define NAZARA_SHADER_SCOPED_VISITOR_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/ShaderAstRecursiveVisitor.hpp>

namespace Nz::ShaderAst
{
	class NAZARA_SHADER_API AstScopedVisitor : public AstRecursiveVisitor
	{
		public:
			struct Identifier;

			AstScopedVisitor() = default;
			~AstScopedVisitor() = default;

			inline const Identifier* FindIdentifier(const std::string_view& identifierName) const;

			void ScopedVisit(StatementPtr& nodePtr);

			using AstRecursiveVisitor::Visit;
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

			struct Variable
			{
				ExpressionType type;
			};

			struct Identifier
			{
				std::string name;
				std::variant<Alias, Variable, StructDescription> value;
			};

		protected:
			void PushScope();
			void PopScope();

			inline void RegisterStruct(StructDescription structDesc);
			inline void RegisterVariable(std::string name, ExpressionType type);

		private:
			std::vector<Identifier> m_identifiersInScope;
			std::vector<std::size_t> m_scopeSizes;
	};
}

#include <Nazara/Shader/ShaderAstScopedVisitor.inl>

#endif
