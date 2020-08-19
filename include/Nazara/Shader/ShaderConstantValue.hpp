// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_CONSTANTVALUE_HPP
#define NAZARA_SHADER_CONSTANTVALUE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Math/Vector4.hpp>
#include <variant>

namespace Nz
{
	using ShaderConstantValue = std::variant<
		bool,
		float,
		Int32,
		UInt32,
		Vector2f,
		Vector3f,
		Vector4f,
		Vector2i32,
		Vector3i32,
		Vector4i32
	>;
}

#endif
