// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_ENUMS_HPP
#define NAZARA_GRAPHICS_ENUMS_HPP

#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/Flags.hpp>

namespace Nz
{
	enum class BasicRenderElement
	{
		SpriteChain = 0,
		Submesh = 1,

		Max = Submesh
	};

	constexpr std::size_t BasicRenderElementCount = UnderlyingCast(BasicRenderElement::Max) + 1;

	enum class CullTest
	{
		Box,
		NoTest,
		Sphere,
		Volume
	};

	enum class MaterialPassFlag
	{
		Transparent,

		Max = Transparent
	};

	template<>
	struct EnumAsFlags<MaterialPassFlag>
	{
		static constexpr MaterialPassFlag max = MaterialPassFlag::Max;
	};

	using MaterialPassFlags = Flags<MaterialPassFlag>;

	enum class ProjectionType
	{
		Orthographic,
		Perspective
	};
}

#endif // NAZARA_GRAPHICS_ENUMS_HPP
