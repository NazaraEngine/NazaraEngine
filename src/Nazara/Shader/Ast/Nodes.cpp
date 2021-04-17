// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/Ast/Nodes.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Shader/Ast/AstExpressionVisitor.hpp>
#include <Nazara/Shader/Ast/AstStatementVisitor.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	Node::~Node() = default;

#define NAZARA_SHADERAST_NODE(Node) NodeType Node::GetType() const \
	{ \
		return NodeType:: Node; \
	}
#include <Nazara/Shader/Ast/AstNodeList.hpp>

#define NAZARA_SHADERAST_EXPRESSION(Node) void Node::Visit(AstExpressionVisitor& visitor) \
	{\
		visitor.Visit(*this); \
	}

#define NAZARA_SHADERAST_STATEMENT(Node) void Node::Visit(AstStatementVisitor& visitor) \
	{\
		visitor.Visit(*this); \
	}

#include <Nazara/Shader/Ast/AstNodeList.hpp>

	ExpressionType ConstantExpression::GetExpressionType() const
	{
		return std::visit([&](auto&& arg) -> ShaderAst::ExpressionType
		{
			using T = std::decay_t<decltype(arg)>;

			if constexpr (std::is_same_v<T, bool>)
				return PrimitiveType::Boolean;
			else if constexpr (std::is_same_v<T, float>)
				return PrimitiveType::Float32;
			else if constexpr (std::is_same_v<T, Int32>)
				return PrimitiveType::Int32;
			else if constexpr (std::is_same_v<T, UInt32>)
				return PrimitiveType::UInt32;
			else if constexpr (std::is_same_v<T, Vector2f>)
				return VectorType{ 2, PrimitiveType::Float32 };
			else if constexpr (std::is_same_v<T, Vector3f>)
				return VectorType{ 3, PrimitiveType::Float32 };
			else if constexpr (std::is_same_v<T, Vector4f>)
				return VectorType{ 4, PrimitiveType::Float32 };
			else if constexpr (std::is_same_v<T, Vector2i32>)
				return VectorType{ 2, PrimitiveType::Int32 };
			else if constexpr (std::is_same_v<T, Vector3i32>)
				return VectorType{ 3, PrimitiveType::Int32 };
			else if constexpr (std::is_same_v<T, Vector4i32>)
				return VectorType{ 4, PrimitiveType::Int32 };
			else
				static_assert(AlwaysFalse<T>::value, "non-exhaustive visitor");
		}, value);
	}
}
