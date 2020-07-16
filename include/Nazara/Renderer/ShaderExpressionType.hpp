// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_EXPRESSIONTYPE_HPP
#define NAZARA_SHADER_EXPRESSIONTYPE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/ShaderNodes.hpp>
#include <string>
#include <variant>

namespace Nz
{
	using ShaderExpressionType = std::variant<ShaderNodes::BasicType, std::string>;
}

#endif // NAZARA_SHADER_EXPRESSIONTYPE_HPP
