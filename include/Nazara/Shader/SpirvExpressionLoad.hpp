// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SPIRVEXPRESSIONLOAD_HPP
#define NAZARA_SPIRVEXPRESSIONLOAD_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/ShaderAstExpressionVisitorExcept.hpp>
#include <Nazara/Shader/SpirvData.hpp>
#include <vector>

namespace Nz
{
	class SpirvBlock;
	class SpirvWriter;

	class NAZARA_SHADER_API SpirvExpressionLoad : public ShaderAst::ExpressionVisitorExcept
	{
		public:
			inline SpirvExpressionLoad(SpirvWriter& writer, SpirvBlock& block);
			SpirvExpressionLoad(const SpirvExpressionLoad&) = delete;
			SpirvExpressionLoad(SpirvExpressionLoad&&) = delete;
			~SpirvExpressionLoad() = default;

			UInt32 Evaluate(ShaderAst::Expression& node);

			using ExpressionVisitorExcept::Visit;
			//void Visit(ShaderAst::AccessMemberExpression& node) override;
			void Visit(ShaderAst::IdentifierExpression& node) override;

			SpirvExpressionLoad& operator=(const SpirvExpressionLoad&) = delete;
			SpirvExpressionLoad& operator=(SpirvExpressionLoad&&) = delete;

		private:
			struct Pointer
			{
				SpirvStorageClass storage;
				UInt32 resultId;
				UInt32 pointedTypeId;
			};

			struct Value
			{
				UInt32 resultId;
			};

			SpirvBlock& m_block;
			SpirvWriter& m_writer;
			std::variant<std::monostate, Pointer, Value> m_value;
	};
}

#include <Nazara/Shader/SpirvExpressionLoad.inl>

#endif
