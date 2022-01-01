// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_AST_ATTRIBUTE_HPP
#define NAZARA_SHADER_AST_ATTRIBUTE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Ast/Enums.hpp>
#include <memory>
#include <optional>
#include <variant>

namespace Nz::ShaderAst
{
	struct Expression;

	using ExpressionPtr = std::unique_ptr<Expression>;

	template<typename T>
	class AttributeValue
	{
		public:
			AttributeValue() = default;
			AttributeValue(T value);
			AttributeValue(ExpressionPtr expr);
			AttributeValue(const AttributeValue&) = default;
			AttributeValue(AttributeValue&&) = default;
			~AttributeValue() = default;

			ExpressionPtr&& GetExpression() &&;
			const ExpressionPtr& GetExpression() const &;
			const T& GetResultingValue() const;

			bool IsExpression() const;
			bool IsResultingValue() const;

			bool HasValue() const;

			AttributeValue& operator=(const AttributeValue&) = default;
			AttributeValue& operator=(AttributeValue&&) = default;

		private:
			std::variant<std::monostate, T, ExpressionPtr> m_value;
	};

	struct Attribute
	{
		using Param = std::optional<ExpressionPtr>;

		AttributeType type;
		Param args;
	};
}

#include <Nazara/Shader/Ast/Attribute.inl>

#endif // NAZARA_SHADER_AST_ATTRIBUTE_HPP
