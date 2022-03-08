// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_AST_INDEXREMAPPER_HPP
#define NAZARA_SHADER_AST_INDEXREMAPPER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/Ast/AstCloner.hpp>
#include <functional>

namespace Nz::ShaderAst
{
	class NAZARA_SHADER_API IndexRemapperVisitor : public AstCloner
	{
		public:
			struct Callbacks;

			IndexRemapperVisitor() = default;
			IndexRemapperVisitor(const IndexRemapperVisitor&) = delete;
			IndexRemapperVisitor(IndexRemapperVisitor&&) = delete;
			~IndexRemapperVisitor() = default;

			StatementPtr Clone(Statement& statement, const Callbacks& callbacks);

			IndexRemapperVisitor& operator=(const IndexRemapperVisitor&) = delete;
			IndexRemapperVisitor& operator=(IndexRemapperVisitor&&) = delete;

			struct Callbacks
			{
				std::function<std::size_t(std::size_t previousIndex)> constIndexGenerator;
				std::function<std::size_t(std::size_t previousIndex)> funcIndexGenerator;
				std::function<std::size_t(std::size_t previousIndex) > structIndexGenerator;
				//std::function<std::size_t()> typeIndexGenerator;
				std::function<std::size_t(std::size_t previousIndex)> varIndexGenerator;
			};

		private:
			StatementPtr Clone(DeclareConstStatement& node) override;
			StatementPtr Clone(DeclareExternalStatement& node) override;
			StatementPtr Clone(DeclareFunctionStatement& node) override;
			StatementPtr Clone(DeclareStructStatement& node) override;
			StatementPtr Clone(DeclareVariableStatement& node) override;

			ExpressionPtr Clone(FunctionExpression& node) override;
			ExpressionPtr Clone(StructTypeExpression& node) override;
			ExpressionPtr Clone(VariableExpression& node) override;

			void HandleType(ExpressionValue<ExpressionType>& exprType);

			struct Context;
			Context* m_context;
	};

	inline StatementPtr RemapIndices(Statement& statement, const IndexRemapperVisitor::Callbacks& callbacks);
}

#include <Nazara/Shader/Ast/IndexRemapperVisitor.inl>

#endif // NAZARA_SHADER_AST_INDEXREMAPPER_HPP
