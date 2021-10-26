// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/Ast/AstExpressionVisitorExcept.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
#define NAZARA_SHADERAST_EXPRESSION(Node) void ExpressionVisitorExcept::Visit(ShaderAst::Node& /*node*/) \
	{ \
		throw std::runtime_error("unexpected " #Node " node"); \
	}
#include <Nazara/Shader/Ast/AstNodeList.hpp>
}
