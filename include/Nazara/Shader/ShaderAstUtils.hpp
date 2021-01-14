// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADERASTUTILS_HPP
#define NAZARA_SHADERASTUTILS_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/ShaderEnums.hpp>
#include <Nazara/Shader/ShaderAstVisitorExcept.hpp>
#include <vector>

namespace Nz
{
	class ShaderAst;

	class NAZARA_SHADER_API ShaderAstValueCategory final : public ShaderAstVisitorExcept
	{
		public:
			ShaderAstValueCategory() = default;
			ShaderAstValueCategory(const ShaderAstValueCategory&) = delete;
			ShaderAstValueCategory(ShaderAstValueCategory&&) = delete;
			~ShaderAstValueCategory() = default;

			ShaderNodes::ExpressionCategory GetExpressionCategory(const ShaderNodes::ExpressionPtr& expression);

			ShaderAstValueCategory& operator=(const ShaderAstValueCategory&) = delete;
			ShaderAstValueCategory& operator=(ShaderAstValueCategory&&) = delete;

		private:
			using ShaderAstVisitorExcept::Visit;
			void Visit(ShaderNodes::AccessMember& node) override;
			void Visit(ShaderNodes::AssignOp& node) override;
			void Visit(ShaderNodes::BinaryOp& node) override;
			void Visit(ShaderNodes::Cast& node) override;
			void Visit(ShaderNodes::ConditionalExpression& node) override;
			void Visit(ShaderNodes::Constant& node) override;
			void Visit(ShaderNodes::Identifier& node) override;
			void Visit(ShaderNodes::IntrinsicCall& node) override;
			void Visit(ShaderNodes::Sample2D& node) override;
			void Visit(ShaderNodes::SwizzleOp& node) override;

			ShaderNodes::ExpressionCategory m_expressionCategory;
	};

	inline ShaderNodes::ExpressionCategory GetExpressionCategory(const ShaderNodes::ExpressionPtr& expression);
}

#include <Nazara/Shader/ShaderAstUtils.inl>

#endif
