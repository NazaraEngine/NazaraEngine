// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_AST_ASTEXPRESSIONVISITOREXCEPT_HPP
#define NAZARA_SHADER_AST_ASTEXPRESSIONVISITOREXCEPT_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/Ast/AstExpressionVisitor.hpp>

namespace Nz::ShaderAst
{
	class NAZARA_SHADER_API ExpressionVisitorExcept : public AstExpressionVisitor
	{
		public:
			using AstExpressionVisitor::Visit;

#define NAZARA_SHADERAST_EXPRESSION(Node) void Visit(ShaderAst::Node& node) override;
#include <Nazara/Shader/Ast/AstNodeList.hpp>
	};
}

#endif // NAZARA_SHADER_AST_ASTEXPRESSIONVISITOREXCEPT_HPP
