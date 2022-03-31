// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_AST_ASTTYPES_HPP
#define NAZARA_SHADER_AST_ASTTYPES_HPP

#include <Nazara/Shader/ShaderLangSourceLocation.hpp>
#include <Nazara/Shader/Ast/ConstantValue.hpp>
#include <Nazara/Shader/Ast/ExpressionType.hpp>
#include <functional>

namespace Nz::ShaderAst
{
	enum class TypeParameterCategory
	{
		ConstantValue,
		FullType,
		PrimitiveType,
		StructType
	};

	struct PartialType;

	using TypeParameter = std::variant<ConstantValue, ExpressionType, PartialType>;

	struct PartialType
	{
		std::vector<TypeParameterCategory> parameters;
		std::function<ExpressionType(const TypeParameter* parameters, std::size_t parameterCount, const ShaderLang::SourceLocation& sourceLocation)> buildFunc;
	};

}

#endif // NAZARA_SHADER_AST_ASTTYPES_HPP
