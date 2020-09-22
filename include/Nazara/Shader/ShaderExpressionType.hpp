// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_EXPRESSIONTYPE_HPP
#define NAZARA_SHADER_EXPRESSIONTYPE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/ShaderEnums.hpp>
#include <string>
#include <variant>

namespace Nz
{
	using ShaderExpressionType = std::variant<ShaderNodes::BasicType, std::string>;

	inline bool IsBasicType(const ShaderExpressionType& type);
	inline bool IsMatrixType(const ShaderExpressionType& type);
	inline bool IsSamplerType(const ShaderExpressionType& type);
	inline bool IsStructType(const ShaderExpressionType& type);
}

#include <Nazara/Shader/ShaderExpressionType.inl>

#endif // NAZARA_SHADER_EXPRESSIONTYPE_HPP
