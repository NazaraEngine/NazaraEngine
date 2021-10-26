// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SPIRVEXPRESSIONLOAD_HPP
#define NAZARA_SPIRVEXPRESSIONLOAD_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/SpirvData.hpp>
#include <Nazara/Shader/Ast/AstExpressionVisitorExcept.hpp>
#include <vector>

namespace Nz
{
	class SpirvAstVisitor;
	class SpirvBlock;
	class SpirvWriter;

	class NAZARA_SHADER_API SpirvExpressionLoad : public ShaderAst::ExpressionVisitorExcept
	{
		public:
			inline SpirvExpressionLoad(SpirvWriter& writer, SpirvAstVisitor& visitor, SpirvBlock& block);
			SpirvExpressionLoad(const SpirvExpressionLoad&) = delete;
			SpirvExpressionLoad(SpirvExpressionLoad&&) = delete;
			~SpirvExpressionLoad() = default;

			UInt32 Evaluate(ShaderAst::Expression& node);

			using ExpressionVisitorExcept::Visit;
			void Visit(ShaderAst::AccessIndexExpression& node) override;
			void Visit(ShaderAst::VariableExpression& node) override;

			SpirvExpressionLoad& operator=(const SpirvExpressionLoad&) = delete;
			SpirvExpressionLoad& operator=(SpirvExpressionLoad&&) = delete;

		private:
			struct Pointer
			{
				SpirvStorageClass storage;
				UInt32 pointerId;
				UInt32 pointedTypeId;
			};

			struct Value
			{
				UInt32 resultId;
			};

			SpirvAstVisitor& m_visitor;
			SpirvBlock& m_block;
			SpirvWriter& m_writer;
			std::variant<std::monostate, Pointer, Value> m_value;
	};
}

#include <Nazara/Shader/SpirvExpressionLoad.inl>

#endif
