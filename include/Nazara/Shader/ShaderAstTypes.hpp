// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_ASTTYPES_HPP
#define NAZARA_SHADER_ASTTYPES_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/ShaderEnums.hpp>
#include <string>
#include <variant>
#include <vector>

namespace Nz::ShaderAst
{
	using ShaderExpressionType = std::variant<BasicType, std::string>;

	struct StructDescription
	{
		struct StructMember
		{
			std::string name;
			ShaderExpressionType type;
		};

		std::string name;
		std::vector<StructMember> members;
	};

	inline bool IsBasicType(const ShaderExpressionType& type);
	inline bool IsMatrixType(const ShaderExpressionType& type);
	inline bool IsSamplerType(const ShaderExpressionType& type);
	inline bool IsStructType(const ShaderExpressionType& type);
}

#include <Nazara/Shader/ShaderAstTypes.inl>

#endif // NAZARA_SHADER_ASTTYPES_HPP
