// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
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
