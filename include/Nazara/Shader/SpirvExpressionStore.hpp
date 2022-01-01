// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_SPIRVEXPRESSIONSTORE_HPP
#define NAZARA_SHADER_SPIRVEXPRESSIONSTORE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/SpirvData.hpp>
#include <Nazara/Shader/Ast/AstExpressionVisitorExcept.hpp>
#include <Nazara/Shader/Ast/Enums.hpp>

namespace Nz
{
	class SpirvAstVisitor;
	class SpirvBlock;
	class SpirvWriter;

	class NAZARA_SHADER_API SpirvExpressionStore : public ShaderAst::ExpressionVisitorExcept
	{
		public:
			inline SpirvExpressionStore(SpirvWriter& writer, SpirvAstVisitor& visitor, SpirvBlock& block);
			SpirvExpressionStore(const SpirvExpressionStore&) = delete;
			SpirvExpressionStore(SpirvExpressionStore&&) = delete;
			~SpirvExpressionStore() = default;

			void Store(ShaderAst::ExpressionPtr& node, UInt32 resultId);

			using ExpressionVisitorExcept::Visit;
			void Visit(ShaderAst::AccessIndexExpression& node) override;
			void Visit(ShaderAst::SwizzleExpression& node) override;
			void Visit(ShaderAst::VariableExpression& node) override;

			SpirvExpressionStore& operator=(const SpirvExpressionStore&) = delete;
			SpirvExpressionStore& operator=(SpirvExpressionStore&&) = delete;

		private:
			struct Pointer
			{
				SpirvStorageClass storage;
				UInt32 pointerId;
			};

			struct SwizzledPointer : Pointer
			{
				ShaderAst::VectorType swizzledType;
				std::array<UInt32, 4> swizzleIndices;
				std::size_t componentCount;
			};

			SpirvAstVisitor& m_visitor;
			SpirvBlock& m_block;
			SpirvWriter& m_writer;
			std::variant<std::monostate, Pointer, SwizzledPointer> m_value;
	};
}

#include <Nazara/Shader/SpirvExpressionStore.inl>

#endif // NAZARA_SHADER_SPIRVEXPRESSIONSTORE_HPP
