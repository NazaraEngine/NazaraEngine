// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADERASTEXPRESSIONVISITOR_HPP
#define NAZARA_SHADERASTEXPRESSIONVISITOR_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/ShaderNodes.hpp>

namespace Nz::ShaderAst
{
	class NAZARA_SHADER_API AstExpressionVisitor
	{
		public:
			AstExpressionVisitor() = default;
			AstExpressionVisitor(const AstExpressionVisitor&) = delete;
			AstExpressionVisitor(AstExpressionVisitor&&) = delete;
			virtual ~AstExpressionVisitor();

#define NAZARA_SHADERAST_EXPRESSION(NodeType) virtual void Visit(NodeType& node) = 0;
#include <Nazara/Shader/ShaderAstNodes.hpp>

			AstExpressionVisitor& operator=(const AstExpressionVisitor&) = delete;
			AstExpressionVisitor& operator=(AstExpressionVisitor&&) = delete;
	};
}

#endif
