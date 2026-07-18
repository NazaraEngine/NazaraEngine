// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_ENUMS_HPP
#define NAZARA_GRAPHICS_ENUMS_HPP

#include <NazaraUtils/Algorithm.hpp>
#include <NazaraUtils/Flags.hpp>

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

	enum class EngineShaderBinding
	{
		DirectionalLights,
		DirectionalShadowAtlasMapping,
		InstanceBuffer,
		PointLights,
		PointShadowAtlasMapping,
		SpotLights,
		SpotShadowAtlasMapping,
		ShadowAtlas,
		SkeletalDataUbo,
		ViewerDataUbo,

		Max = ViewerDataUbo
	};

	enum class FramePipelinePassFlag
	{
		LightShadowing,

		Max = LightShadowing
	};

	template<>
	struct EnumAsFlags<FramePipelinePassFlag>
	{
		static constexpr FramePipelinePassFlag max = FramePipelinePassFlag::Max;
	};

	using FramePipelinePassFlags = Flags<FramePipelinePassFlag>;

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

	enum class MaterialInstancePreset
	{
		AdditiveBlended,
		AlphaBlended,
		NoDepth,
		ReverseZ,
		UI,

		Max = UI
	};

	template<>
	struct EnumAsFlags<MaterialInstancePreset>
	{
		static constexpr MaterialInstancePreset max = MaterialInstancePreset::Max;
	};

	using MaterialInstancePresetFlags = Flags<MaterialInstancePreset>;

	constexpr std::size_t MaterialInstancePresetFlags_ValueCount = 1u << (static_cast<std::size_t>(MaterialInstancePreset::Max) + 1);

	enum class MaterialType
	{
		Basic,
		Phong,
		PhysicallyBased,

		Max = PhysicallyBased
	};

	enum class MaterialPassFlag
	{
		SortByDistance, //< Deprecated

		Max = SortByDistance
	};

	template<>
	struct EnumAsFlags<MaterialPassFlag>
	{
		static constexpr MaterialPassFlag max = MaterialPassFlag::Max;
	};

	using MaterialPassFlags = Flags<MaterialPassFlag>;

	enum class RenderQueueFlag
	{
		SortByDistance,

		Max = SortByDistance
	};

	template<>
	struct EnumAsFlags<RenderQueueFlag>
	{
		static constexpr RenderQueueFlag max = RenderQueueFlag::Max;
	};

	using RenderQueueFlags = Flags<RenderQueueFlag>;

	enum class ProjectionType
	{
		Orthographic,
		Perspective
	};
}

#endif // NAZARA_GRAPHICS_ENUMS_HPP
