// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_SPIRVEXPRESSIONLOAD_HPP
#define NAZARA_SHADER_SPIRVEXPRESSIONLOAD_HPP

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

	class NAZARA_SHADER_API SpirvExpressionLoad : public ShaderAst::AstExpressionVisitorExcept
	{
		public:
			inline SpirvExpressionLoad(SpirvWriter& writer, SpirvAstVisitor& visitor, SpirvBlock& block);
			SpirvExpressionLoad(const SpirvExpressionLoad&) = delete;
			SpirvExpressionLoad(SpirvExpressionLoad&&) = delete;
			~SpirvExpressionLoad() = default;

			UInt32 Evaluate(ShaderAst::Expression& node);

			using AstExpressionVisitorExcept::Visit;
			void Visit(ShaderAst::AccessIndexExpression& node) override;
			void Visit(ShaderAst::VariableValueExpression& node) override;

			SpirvExpressionLoad& operator=(const SpirvExpressionLoad&) = delete;
			SpirvExpressionLoad& operator=(SpirvExpressionLoad&&) = delete;

		private:
			struct PointerChainAccess
			{
				std::vector<UInt32> indices;
				const ShaderAst::ExpressionType* exprType;
				SpirvStorageClass storage;
				UInt32 pointerId;
				UInt32 pointedTypeId;
			};

			struct Pointer
			{
				SpirvStorageClass storage;
				UInt32 pointerId;
				UInt32 pointedTypeId;
			};

			struct Value
			{
				UInt32 valueId;
			};

			struct ValueExtraction
			{
				std::vector<UInt32> indices;
				UInt32 typeId;
				UInt32 valueId;
			};

			SpirvAstVisitor& m_visitor;
			SpirvBlock& m_block;
			SpirvWriter& m_writer;
			std::variant<std::monostate, ValueExtraction, Pointer, PointerChainAccess, Value> m_value;
	};
}

#include <Nazara/Shader/SpirvExpressionLoad.inl>

#endif // NAZARA_SHADER_SPIRVEXPRESSIONLOAD_HPP
