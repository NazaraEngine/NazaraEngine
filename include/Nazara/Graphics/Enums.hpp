// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_ENUMS_HPP
#define NAZARA_GRAPHICS_ENUMS_HPP

#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Utils/Flags.hpp>

namespace Nz
{
	enum class BasicLightType
	{
		Directional,
		Point,
		Spot,

		Max = Spot
	};

	constexpr std::size_t BasicLightTypeCount = UnderlyingCast(BasicLightType::Max) + 1;

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

	enum class MaterialPropertyType
	{
		Bool,
		Bool2,
		Bool3,
		Bool4,
		Color,
		Float,
		Float2,
		Float3,
		Float4,
		Int,
		Int2,
		Int3,
		Int4,
		UInt,
		UInt2,
		UInt3,
		UInt4
	};

	enum class MaterialLightingType
	{
		None,
		Phong,
		PhysicallyBased
	};

	enum class MaterialPassFlag
	{
		SortByDistance,

		Max = SortByDistance
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

	enum class EngineShaderBinding
	{
		InstanceDataUbo,
		LightDataUbo,
		OverlayTexture,
		SkeletalDataUbo,
		ViewerDataUbo,

		Max = ViewerDataUbo
	};

	constexpr std::size_t PredefinedShaderBindingCount = static_cast<std::size_t>(EngineShaderBinding::Max) + 1;
}

#endif // NAZARA_GRAPHICS_ENUMS_HPP
