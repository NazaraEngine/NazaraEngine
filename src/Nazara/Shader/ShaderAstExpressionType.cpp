// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderAstExpressionType.hpp>
#include <Nazara/Shader/ShaderAstCache.hpp>
#include <optional>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	ShaderExpressionType ExpressionTypeVisitor::GetExpressionType(Expression& expression, AstCache* cache = nullptr)
	{
		m_cache = cache;
		ShaderExpressionType type = GetExpressionTypeInternal(expression);
		m_cache = nullptr;

		return type;
	}

	ShaderExpressionType ExpressionTypeVisitor::GetExpressionTypeInternal(Expression& expression)
	{
		m_lastExpressionType.reset();

		Visit(expression);

		assert(m_lastExpressionType.has_value());
		return std::move(*m_lastExpressionType);
	}

	void ExpressionTypeVisitor::Visit(Expression& expression)
	{
		if (m_cache)
		{
			auto it = m_cache->nodeExpressionType.find(&expression);
			if (it != m_cache->nodeExpressionType.end())
			{
				m_lastExpressionType = it->second;
				return;
			}
		}

		expression.Visit(*this);

		if (m_cache)
		{
			assert(m_lastExpressionType.has_value());
			m_cache->nodeExpressionType.emplace(&expression, *m_lastExpressionType);
		}
	}

	void ExpressionTypeVisitor::Visit(AccessMemberExpression& node)
	{
		throw std::runtime_error("unhandled accessmember expression");
	}

	void ExpressionTypeVisitor::Visit(AssignExpression& node)
	{
		Visit(*node.left);
	}

	void ExpressionTypeVisitor::Visit(BinaryExpression& node)
	{
		switch (node.op)
		{
			case BinaryType::Add:
			case BinaryType::Subtract:
				return Visit(*node.left);

			case BinaryType::Divide:
			case BinaryType::Multiply:
			{
				ShaderExpressionType leftExprType = GetExpressionTypeInternal(*node.left);
				assert(IsBasicType(leftExprType));

				ShaderExpressionType rightExprType = GetExpressionTypeInternal(*node.right);
				assert(IsBasicType(rightExprType));

				switch (std::get<BasicType>(leftExprType))
				{
					case BasicType::Boolean:
					case BasicType::Float2:
					case BasicType::Float3:
					case BasicType::Float4:
					case BasicType::Int2:
					case BasicType::Int3:
					case BasicType::Int4:
					case BasicType::UInt2:
					case BasicType::UInt3:
					case BasicType::UInt4:
						m_lastExpressionType = std::move(leftExprType);
						break;

					case BasicType::Float1:
					case BasicType::Int1:
					case BasicType::Mat4x4:
					case BasicType::UInt1:
						m_lastExpressionType = std::move(rightExprType);
						break;

					case BasicType::Sampler2D:
					case BasicType::Void:
						break;
				}

				break;
			}

			case BinaryType::CompEq:
			case BinaryType::CompGe:
			case BinaryType::CompGt:
			case BinaryType::CompLe:
			case BinaryType::CompLt:
			case BinaryType::CompNe:
				m_lastExpressionType = BasicType::Boolean;
				break;
		}
	}

	void ExpressionTypeVisitor::Visit(CastExpression& node)
	{
		m_lastExpressionType = node.targetType;
	}

	void ExpressionTypeVisitor::Visit(ConditionalExpression& node)
	{
		ShaderExpressionType leftExprType = GetExpressionTypeInternal(*node.truePath);
		assert(leftExprType == GetExpressionTypeInternal(*node.falsePath));

		m_lastExpressionType = std::move(leftExprType);
	}

	void ExpressionTypeVisitor::Visit(ConstantExpression& node)
	{
		m_lastExpressionType = std::visit([&](auto&& arg)
		{
			using T = std::decay_t<decltype(arg)>;

			if constexpr (std::is_same_v<T, bool>)
				return BasicType::Boolean;
			else if constexpr (std::is_same_v<T, float>)
				return BasicType::Float1;
			else if constexpr (std::is_same_v<T, Int32>)
				return BasicType::Int1;
			else if constexpr (std::is_same_v<T, UInt32>)
				return BasicType::Int1;
			else if constexpr (std::is_same_v<T, Vector2f>)
				return BasicType::Float2;
			else if constexpr (std::is_same_v<T, Vector3f>)
				return BasicType::Float3;
			else if constexpr (std::is_same_v<T, Vector4f>)
				return BasicType::Float4;
			else if constexpr (std::is_same_v<T, Vector2i32>)
				return BasicType::Int2;
			else if constexpr (std::is_same_v<T, Vector3i32>)
				return BasicType::Int3;
			else if constexpr (std::is_same_v<T, Vector4i32>)
				return BasicType::Int4;
			else
				static_assert(AlwaysFalse<T>::value, "non-exhaustive visitor");
		}, node.value);
	}

	void ExpressionTypeVisitor::Visit(IdentifierExpression& node)
	{
		auto scopeIt = m_cache->scopeIdByNode.find(&node);
		if (scopeIt == m_cache->scopeIdByNode.end())
			throw std::runtime_error("internal error");

		const AstCache::Identifier* identifier = m_cache->FindIdentifier(scopeIt->second, node.identifier);
		if (!identifier || !std::holds_alternative<AstCache::Variable>(identifier->value))
			throw std::runtime_error("internal error");

		m_lastExpressionType = std::get<AstCache::Variable>(identifier->value).type;
	}

	void ExpressionTypeVisitor::Visit(IntrinsicExpression& node)
	{
		switch (node.intrinsic)
		{
			case IntrinsicType::CrossProduct:
				Visit(*node.parameters.front());
				break;

			case IntrinsicType::DotProduct:
				m_lastExpressionType = BasicType::Float1;
				break;
		}
	}

	void ExpressionTypeVisitor::Visit(SwizzleExpression& node)
	{
		const ShaderExpressionType& exprType = GetExpressionTypeInternal(*node.expression);
		assert(IsBasicType(exprType));

		m_lastExpressionType = static_cast<BasicType>(UnderlyingCast(GetComponentType(std::get<BasicType>(exprType))) + node.componentCount - 1);
	}
}
