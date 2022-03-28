// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/Ast/ExpressionValue.hpp>
#include <cassert>
#include <stdexcept>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	template<typename T>
	ExpressionValue<T>::ExpressionValue(T value) :
	m_value(std::move(value))
	{
	}

	template<typename T>
	ExpressionValue<T>::ExpressionValue(ExpressionPtr expr)
	{
		assert(expr);
		m_value = std::move(expr);
	}

	template<typename T>
	ExpressionPtr&& ExpressionValue<T>::GetExpression() &&
	{
		if (!IsExpression())
			throw std::runtime_error("excepted expression");

		return std::get<ExpressionPtr>(std::move(m_value));
	}

	template<typename T>
	const ExpressionPtr& ExpressionValue<T>::GetExpression() const &
	{
		if (!IsExpression())
			throw std::runtime_error("excepted expression");

		assert(std::get<ExpressionPtr>(m_value));
		return std::get<ExpressionPtr>(m_value);
	}

	template<typename T>
	const T& ExpressionValue<T>::GetResultingValue() const
	{
		if (!IsResultingValue())
			throw std::runtime_error("excepted resulting value");

		return std::get<T>(m_value);
	}

	template<typename T>
	bool ExpressionValue<T>::IsExpression() const
	{
		return std::holds_alternative<ExpressionPtr>(m_value);
	}

	template<typename T>
	bool ExpressionValue<T>::IsResultingValue() const
	{
		return std::holds_alternative<T>(m_value);
	}

	template<typename T>
	bool ExpressionValue<T>::HasValue() const
	{
		return !std::holds_alternative<std::monostate>(m_value);
	}

	template<typename T>
	void ExpressionValue<T>::Reset()
	{
		m_value = {};
	}
}

#include <Nazara/Shader/DebugOff.hpp>
