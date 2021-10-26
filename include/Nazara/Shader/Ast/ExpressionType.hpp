// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_AST_EXPRESSIONTYPE_HPP
#define NAZARA_SHADER_AST_EXPRESSIONTYPE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <Nazara/Shader/Ast/Attribute.hpp>
#include <Nazara/Shader/Ast/Enums.hpp>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace Nz::ShaderAst
{
	struct IdentifierType //< Alias or struct
	{
		std::string name;

		inline bool operator==(const IdentifierType& rhs) const;
		inline bool operator!=(const IdentifierType& rhs) const;
	};

	struct MatrixType
	{
		std::size_t columnCount;
		std::size_t rowCount;
		PrimitiveType type;

		inline bool operator==(const MatrixType& rhs) const;
		inline bool operator!=(const MatrixType& rhs) const;
	};

	struct NoType
	{
		inline bool operator==(const NoType& rhs) const;
		inline bool operator!=(const NoType& rhs) const;
	};

	struct SamplerType
	{
		ImageType dim;
		PrimitiveType sampledType;

		inline bool operator==(const SamplerType& rhs) const;
		inline bool operator!=(const SamplerType& rhs) const;
	};

	struct StructType
	{
		std::size_t structIndex;

		inline bool operator==(const StructType& rhs) const;
		inline bool operator!=(const StructType& rhs) const;
	};

	struct UniformType
	{
		std::variant<IdentifierType, StructType> containedType;

		inline bool operator==(const UniformType& rhs) const;
		inline bool operator!=(const UniformType& rhs) const;
	};

	struct VectorType
	{
		std::size_t componentCount;
		PrimitiveType type;

		inline bool operator==(const VectorType& rhs) const;
		inline bool operator!=(const VectorType& rhs) const;
	};

	using ExpressionType = std::variant<NoType, IdentifierType, PrimitiveType, MatrixType, SamplerType, StructType, UniformType, VectorType>;

	struct StructDescription
	{
		struct StructMember
		{
			AttributeValue<BuiltinEntry> builtin;
			AttributeValue<bool> cond;
			AttributeValue<UInt32> locationIndex;
			std::string name;
			ExpressionType type;
		};

		AttributeValue<StructLayout> layout;
		std::string name;
		std::vector<StructMember> members;
	};

	inline bool IsIdentifierType(const ExpressionType& type);
	inline bool IsMatrixType(const ExpressionType& type);
	inline bool IsNoType(const ExpressionType& type);
	inline bool IsPrimitiveType(const ExpressionType& type);
	inline bool IsSamplerType(const ExpressionType& type);
	inline bool IsStructType(const ExpressionType& type);
	inline bool IsUniformType(const ExpressionType& type);
	inline bool IsVectorType(const ExpressionType& type);
}

#include <Nazara/Shader/Ast/ExpressionType.inl>

#endif
