// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADERASTEXPRESSIONVISITOREXCEPT_HPP
#define NAZARA_SHADERASTEXPRESSIONVISITOREXCEPT_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/ShaderAstExpressionVisitor.hpp>

namespace Nz::ShaderAst
{
	class NAZARA_SHADER_API ExpressionVisitorExcept : public AstExpressionVisitor
	{
		public:
			using AstExpressionVisitor::Visit;

#define NAZARA_SHADERAST_EXPRESSION(Node) void Visit(ShaderAst::Node& node) override;
#include <Nazara/Shader/ShaderAstNodes.hpp>
	};
}

#endif
