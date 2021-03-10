// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderNodes.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Shader/ShaderAstExpressionVisitor.hpp>
#include <Nazara/Shader/ShaderAstStatementVisitor.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	Node::~Node() = default;

#define NAZARA_SHADERAST_NODE(Node) NodeType Node::GetType() const \
	{ \
		return NodeType:: Node; \
	}
#include <Nazara/Shader/ShaderAstNodes.hpp>

#define NAZARA_SHADERAST_EXPRESSION(Node) void Node::Visit(AstExpressionVisitor& visitor) \
	{\
		visitor.Visit(*this); \
	}

#define NAZARA_SHADERAST_STATEMENT(Node) void Node::Visit(AstStatementVisitor& visitor) \
	{\
		visitor.Visit(*this); \
	}

#include <Nazara/Shader/ShaderAstNodes.hpp>
}
