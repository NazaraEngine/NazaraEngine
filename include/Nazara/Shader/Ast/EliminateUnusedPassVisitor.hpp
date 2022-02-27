// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_AST_ELIMINATEUNUSEDPASSVISITOR_HPP
#define NAZARA_SHADER_AST_ELIMINATEUNUSEDPASSVISITOR_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Bitset.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/Ast/AstCloner.hpp>

namespace Nz::ShaderAst
{
	class NAZARA_SHADER_API EliminateUnusedPassVisitor : AstCloner
	{
		public:
			struct Config;

			EliminateUnusedPassVisitor() = default;
			EliminateUnusedPassVisitor(const EliminateUnusedPassVisitor&) = delete;
			EliminateUnusedPassVisitor(EliminateUnusedPassVisitor&&) = delete;
			~EliminateUnusedPassVisitor() = default;

			StatementPtr Process(Statement& statement, const Config& config = {});

			EliminateUnusedPassVisitor& operator=(const EliminateUnusedPassVisitor&) = delete;
			EliminateUnusedPassVisitor& operator=(EliminateUnusedPassVisitor&&) = delete;

			struct Config
			{
				ShaderStageTypeFlags usedShaderStages = ShaderStageType_All;
			};

		private:
			using AstCloner::Clone;
			StatementPtr Clone(DeclareExternalStatement& node) override;
			StatementPtr Clone(DeclareFunctionStatement& node) override;
			StatementPtr Clone(DeclareStructStatement& node) override;
			StatementPtr Clone(DeclareVariableStatement& node) override;

			bool IsFunctionUsed(std::size_t varIndex) const;
			bool IsStructUsed(std::size_t varIndex) const;
			bool IsVariableUsed(std::size_t varIndex) const;

			struct Context;
			Context* m_context;
	};

	inline StatementPtr EliminateUnusedPass(Statement& ast, const EliminateUnusedPassVisitor::Config& config = {});
}

#include <Nazara/Shader/Ast/EliminateUnusedPassVisitor.inl>

#endif // NAZARA_SHADER_AST_ELIMINATEUNUSEDPASSVISITOR_HPP
