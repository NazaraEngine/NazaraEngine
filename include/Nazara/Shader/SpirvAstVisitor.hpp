// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SPIRVASTVISITOR_HPP
#define NAZARA_SPIRVASTVISITOR_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/ShaderAstExpressionVisitorExcept.hpp>
#include <Nazara/Shader/ShaderAstStatementVisitorExcept.hpp>
#include <Nazara/Shader/SpirvBlock.hpp>
#include <vector>

namespace Nz
{
	class SpirvWriter;

	class NAZARA_SHADER_API SpirvAstVisitor : public ShaderAst::ExpressionVisitorExcept, public ShaderAst::StatementVisitorExcept
	{
		public:
			inline SpirvAstVisitor(SpirvWriter& writer, std::vector<SpirvBlock>& blocks, ShaderAst::AstCache* cache);
			SpirvAstVisitor(const SpirvAstVisitor&) = delete;
			SpirvAstVisitor(SpirvAstVisitor&&) = delete;
			~SpirvAstVisitor() = default;

			UInt32 EvaluateExpression(ShaderAst::ExpressionPtr& expr);

			using ExpressionVisitorExcept::Visit;
			using StatementVisitorExcept::Visit;

			void Visit(ShaderAst::AccessMemberExpression& node) override;
			void Visit(ShaderAst::AssignExpression& node) override;
			void Visit(ShaderAst::BinaryExpression& node) override;
			void Visit(ShaderAst::BranchStatement& node) override;
			void Visit(ShaderAst::CastExpression& node) override;
			void Visit(ShaderAst::ConditionalExpression& node) override;
			void Visit(ShaderAst::ConditionalStatement& node) override;
			void Visit(ShaderAst::ConstantExpression& node) override;
			void Visit(ShaderAst::DeclareVariableStatement& node) override;
			void Visit(ShaderAst::DiscardStatement& node) override;
			void Visit(ShaderAst::ExpressionStatement& node) override;
			void Visit(ShaderAst::IdentifierExpression& node) override;
			void Visit(ShaderAst::IntrinsicExpression& node) override;
			void Visit(ShaderAst::MultiStatement& node) override;
			void Visit(ShaderAst::NoOpStatement& node) override;
			void Visit(ShaderAst::ReturnStatement& node) override;
			void Visit(ShaderAst::SwizzleExpression& node) override;

			SpirvAstVisitor& operator=(const SpirvAstVisitor&) = delete;
			SpirvAstVisitor& operator=(SpirvAstVisitor&&) = delete;

		private:
			void PushResultId(UInt32 value);
			UInt32 PopResultId();

			ShaderAst::AstCache* m_cache;
			std::vector<SpirvBlock>& m_blocks;
			std::vector<UInt32> m_resultIds;
			SpirvBlock* m_currentBlock;
			SpirvWriter& m_writer;
	};
}

#include <Nazara/Shader/SpirvAstVisitor.inl>

#endif
