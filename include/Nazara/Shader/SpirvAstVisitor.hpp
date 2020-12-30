// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SPIRVEXPRESSIONLOAD_HPP
#define NAZARA_SPIRVEXPRESSIONLOAD_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/ShaderAstVisitorExcept.hpp>
#include <Nazara/Shader/ShaderVarVisitorExcept.hpp>
#include <vector>

namespace Nz
{
	class SpirvWriter;

	class NAZARA_SHADER_API SpirvAstVisitor : public ShaderAstVisitorExcept
	{
		public:
			inline SpirvAstVisitor(SpirvWriter& writer);
			SpirvAstVisitor(const SpirvAstVisitor&) = delete;
			SpirvAstVisitor(SpirvAstVisitor&&) = delete;
			~SpirvAstVisitor() = default;

			UInt32 EvaluateExpression(const ShaderNodes::ExpressionPtr& expr);

			using ShaderAstVisitorExcept::Visit;
			void Visit(ShaderNodes::AccessMember& node) override;
			void Visit(ShaderNodes::AssignOp& node) override;
			void Visit(ShaderNodes::BinaryOp& node) override;
			void Visit(ShaderNodes::Cast& node) override;
			void Visit(ShaderNodes::ConditionalExpression& node) override;
			void Visit(ShaderNodes::ConditionalStatement& node) override;
			void Visit(ShaderNodes::Constant& node) override;
			void Visit(ShaderNodes::DeclareVariable& node) override;
			void Visit(ShaderNodes::Discard& node) override;
			void Visit(ShaderNodes::ExpressionStatement& node) override;
			void Visit(ShaderNodes::Identifier& node) override;
			void Visit(ShaderNodes::IntrinsicCall& node) override;
			void Visit(ShaderNodes::Sample2D& node) override;
			void Visit(ShaderNodes::StatementBlock& node) override;
			void Visit(ShaderNodes::SwizzleOp& node) override;

			SpirvAstVisitor& operator=(const SpirvAstVisitor&) = delete;
			SpirvAstVisitor& operator=(SpirvAstVisitor&&) = delete;

		private:
			void PushResultId(UInt32 value);
			UInt32 PopResultId();

			std::vector<UInt32> m_resultIds;
			SpirvWriter& m_writer;
	};
}

#include <Nazara/Shader/SpirvAstVisitor.inl>

#endif
