// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SPIRVEXPRESSIONSTORE_HPP
#define NAZARA_SPIRVEXPRESSIONSTORE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/SpirvData.hpp>
#include <Nazara/Shader/Ast/AstExpressionVisitorExcept.hpp>

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
			struct LocalVar
			{
				std::string varName;
			};

			struct Pointer
			{
				SpirvStorageClass storage;
				UInt32 pointerId;
			};

			SpirvAstVisitor& m_visitor;
			SpirvBlock& m_block;
			SpirvWriter& m_writer;
			std::variant<std::monostate, LocalVar, Pointer> m_value;
	};
}

#include <Nazara/Shader/SpirvExpressionStore.inl>

#endif
