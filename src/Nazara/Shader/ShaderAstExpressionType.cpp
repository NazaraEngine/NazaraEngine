// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderAstExpressionType.hpp>
#include <Nazara/Shader/ShaderAstCache.hpp>
#include <optional>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	ExpressionType ExpressionTypeVisitor::GetExpressionType(Expression& expression, AstCache* cache)
	{
		m_cache = cache;
		ExpressionType type = GetExpressionTypeInternal(expression);
		m_cache = nullptr;

		return type;
	}

	ExpressionType ExpressionTypeVisitor::GetExpressionTypeInternal(Expression& expression)
	{
		m_lastExpressionType.reset();

		Visit(expression);

		assert(m_lastExpressionType.has_value());
		return std::move(*m_lastExpressionType);
	}

	ExpressionType ExpressionTypeVisitor::ResolveAlias(Expression& expression, ExpressionType expressionType)
	{
		if (IsIdentifierType(expressionType))
		{
			auto scopeIt = m_cache->scopeIdByNode.find(&expression);
			if (scopeIt == m_cache->scopeIdByNode.end())
				throw std::runtime_error("internal error");

			const AstCache::Identifier* identifier = m_cache->FindIdentifier(scopeIt->second, std::get<IdentifierType>(expressionType).name);
			if (identifier && std::holds_alternative<AstCache::Alias>(identifier->value))
			{
				const AstCache::Alias& alias = std::get<AstCache::Alias>(identifier->value);
				return std::visit([&](auto&& arg) -> ShaderAst::ExpressionType
				{
					using T = std::decay_t<decltype(arg)>;

					if constexpr (std::is_same_v<T, ExpressionType>)
						return arg;
					else
						static_assert(AlwaysFalse<T>::value, "non-exhaustive visitor");
				}, alias.value);
			}
		}

		return expressionType;
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
		auto scopeIt = m_cache->scopeIdByNode.find(&node);
		if (scopeIt == m_cache->scopeIdByNode.end())
			throw std::runtime_error("internal error");

		ExpressionType expressionType = ResolveAlias(node, GetExpressionTypeInternal(*node.structExpr));
		if (!IsIdentifierType(expressionType))
			throw std::runtime_error("internal error");

		const AstCache::Identifier* identifier = m_cache->FindIdentifier(scopeIt->second, std::get<IdentifierType>(expressionType).name);

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
				ExpressionType leftExprType = ResolveAlias(node, GetExpressionTypeInternal(*node.left));
				ExpressionType rightExprType = ResolveAlias(node, GetExpressionTypeInternal(*node.right));

				if (IsPrimitiveType(leftExprType))
				{
					switch (std::get<PrimitiveType>(leftExprType))
					{
						case PrimitiveType::Boolean:
							m_lastExpressionType = std::move(leftExprType);
							break;

						case PrimitiveType::Float32:
						case PrimitiveType::Int32:
						case PrimitiveType::UInt32:
							m_lastExpressionType = std::move(rightExprType);
							break;
					}
				}
				else if (IsMatrixType(leftExprType))
				{
					if (IsVectorType(rightExprType))
						m_lastExpressionType = std::move(rightExprType);
					else
						m_lastExpressionType = std::move(leftExprType);
				}
				else if (IsVectorType(leftExprType))
					m_lastExpressionType = std::move(leftExprType);
				else
					throw std::runtime_error("validation failure");

				break;
			}

			case BinaryType::CompEq:
			case BinaryType::CompGe:
			case BinaryType::CompGt:
			case BinaryType::CompLe:
			case BinaryType::CompLt:
			case BinaryType::CompNe:
				m_lastExpressionType = PrimitiveType::Boolean;
				break;
		}
	}

	void ExpressionTypeVisitor::Visit(CastExpression& node)
	{
		m_lastExpressionType = node.targetType;
	}

	void ExpressionTypeVisitor::Visit(ConditionalExpression& node)
	{
		ExpressionType leftExprType = ResolveAlias(node, GetExpressionTypeInternal(*node.truePath));
		assert(leftExprType == ResolveAlias(node, GetExpressionTypeInternal(*node.falsePath)));

		m_lastExpressionType = std::move(leftExprType);
	}

	void ExpressionTypeVisitor::Visit(ConstantExpression& node)
	{
		m_lastExpressionType = std::visit([&](auto&& arg) -> ShaderAst::ExpressionType
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
		}, node.value);
	}

	void ExpressionTypeVisitor::Visit(IdentifierExpression& node)
	{
		assert(m_cache);

		auto scopeIt = m_cache->scopeIdByNode.find(&node);
		if (scopeIt == m_cache->scopeIdByNode.end())
			throw std::runtime_error("internal error");

		const AstCache::Identifier* identifier = m_cache->FindIdentifier(scopeIt->second, node.identifier);
		if (!identifier || !std::holds_alternative<AstCache::Variable>(identifier->value))
			throw std::runtime_error("internal error");

		m_lastExpressionType = ResolveAlias(node, std::get<AstCache::Variable>(identifier->value).type);
	}

	void ExpressionTypeVisitor::Visit(IntrinsicExpression& node)
	{
		switch (node.intrinsic)
		{
			case IntrinsicType::CrossProduct:
				Visit(*node.parameters.front());
				break;

			case IntrinsicType::DotProduct:
				m_lastExpressionType = PrimitiveType::Float32;
				break;

			case IntrinsicType::SampleTexture:
			{
				if (node.parameters.empty())
					throw std::runtime_error("validation failure");

				ExpressionType firstParamType = ResolveAlias(node, GetExpressionTypeInternal(*node.parameters.front()));

				if (!IsSamplerType(firstParamType))
					throw std::runtime_error("validation failure");

				const auto& sampler = std::get<SamplerType>(firstParamType);

				m_lastExpressionType = VectorType{
					4,
					sampler.sampledType
				};

				break;
			}
		}
	}

	void ExpressionTypeVisitor::Visit(SwizzleExpression& node)
	{
		ExpressionType exprType = GetExpressionTypeInternal(*node.expression);

		if (IsMatrixType(exprType))
			m_lastExpressionType = std::get<MatrixType>(exprType).type;
		else if (IsVectorType(exprType))
			m_lastExpressionType = std::get<VectorType>(exprType).type;
		else
			throw std::runtime_error("validation failure");
	}
}
