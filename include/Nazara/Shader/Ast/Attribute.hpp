// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADERAST_ATTRIBUTES_HPP
#define NAZARA_SHADERAST_ATTRIBUTES_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Ast/Enums.hpp>
#include <variant>

namespace Nz::ShaderAst
{
	struct Attribute
	{
		using Param = std::variant<std::monostate, long long, std::string>;

		AttributeType type;
		Param args;
	};
}

#endif
