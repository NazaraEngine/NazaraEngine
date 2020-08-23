// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SPIRVSTATEMENTVISITOR_HPP
#define NAZARA_SPIRVSTATEMENTVISITOR_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/ShaderAstVisitorExcept.hpp>

namespace Nz
{
	class SpirvWriter;

	class NAZARA_SHADER_API SpirvStatementVisitor : public ShaderAstVisitorExcept
	{
		public:
			inline SpirvStatementVisitor(SpirvWriter& writer);
			SpirvStatementVisitor(const SpirvStatementVisitor&) = delete;
			SpirvStatementVisitor(SpirvStatementVisitor&&) = delete;
			~SpirvStatementVisitor() = default;

			using ShaderAstVisitor::Visit;
			void Visit(ShaderNodes::AssignOp& node) override;
			void Visit(ShaderNodes::Branch& node) override;
			void Visit(ShaderNodes::DeclareVariable& node) override;
			void Visit(ShaderNodes::ExpressionStatement& node) override;
			void Visit(ShaderNodes::StatementBlock& node) override;

			SpirvStatementVisitor& operator=(const SpirvStatementVisitor&) = delete;
			SpirvStatementVisitor& operator=(SpirvStatementVisitor&&) = delete;

		private:
			SpirvWriter& m_writer;
	};
}

#include <Nazara/Shader/SpirvStatementVisitor.inl>

#endif
