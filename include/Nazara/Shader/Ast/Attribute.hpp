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
	class ExpressionValue
	{
		public:
			ExpressionValue() = default;
			ExpressionValue(T value);
			ExpressionValue(ExpressionPtr expr);
			ExpressionValue(const ExpressionValue&) = default;
			ExpressionValue(ExpressionValue&&) noexcept = default;
			~ExpressionValue() = default;

			ExpressionPtr&& GetExpression() &&;
			const ExpressionPtr& GetExpression() const &;
			const T& GetResultingValue() const;

			bool IsExpression() const;
			bool IsResultingValue() const;

			bool HasValue() const;

			void Reset();

			ExpressionValue& operator=(const ExpressionValue&) = default;
			ExpressionValue& operator=(ExpressionValue&&) noexcept = default;

		private:
			std::variant<std::monostate, T, ExpressionPtr> m_value;
	};

	struct ExprValue
	{
		using Param = std::optional<ExpressionPtr>;

		AttributeType type;
		Param args;
	};
}

#include <Nazara/Shader/Ast/Attribute.inl>

#endif // NAZARA_SHADER_AST_ATTRIBUTE_HPP
