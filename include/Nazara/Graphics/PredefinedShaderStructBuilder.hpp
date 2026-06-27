// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_PREDEFINEDSHADERSTRUCTBUILDER_HPP
#define NAZARA_GRAPHICS_PREDEFINEDSHADERSTRUCTBUILDER_HPP

#include <NZSL/Math/FieldOffsets.hpp>

namespace Nz
{
	struct PredefinedDirectionalLightData
	{
		nzsl::FieldOffsets fieldOffsets;

		std::size_t colorOffset;
		std::size_t directionOffset;
		std::size_t ambientFactorOffset;
		std::size_t diffuseFactorOffset;
		std::size_t shadowIndexOffset;

		std::size_t totalSize;

		static constexpr PredefinedDirectionalLightData Build();
	};
	
	struct PredefinedDirectionalLightsData
	{
		nzsl::FieldOffsets fieldOffsets;

		std::size_t lightCount;
		std::size_t lights; //< dynamic array

		std::size_t totalSize; //< not including lights

		static constexpr PredefinedDirectionalLightsData Build();
	};

	struct PredefinedDirectionalShadowAtlasEntryData
	{
		nzsl::FieldOffsets fieldOffsets;

		std::size_t offset;
		std::size_t size;
		std::size_t viewProjMatrices;
		std::size_t cascadeCount;
		std::size_t cascadeDistances;

		std::size_t totalSize;

		static constexpr std::size_t MaxLightCascadeCount = 4;

		static constexpr PredefinedDirectionalShadowAtlasEntryData Build();
	};

	struct PredefinedPointLightData
	{
		nzsl::FieldOffsets fieldOffsets;

		std::size_t colorOffset;
		std::size_t positionOffset;
		std::size_t ambientFactorOffset;
		std::size_t diffuseFactorOffset;
		std::size_t radiusOffset;
		std::size_t invRadiusOffset;
		std::size_t shadowIndexOffset;

		std::size_t totalSize;

		static constexpr PredefinedPointLightData Build();
	};

	struct PredefinedPointLightsData
	{
		nzsl::FieldOffsets fieldOffsets;

		std::size_t lightCount;
		std::size_t lights; //< dynamic array

		std::size_t totalSize; //< not including lights

		static constexpr PredefinedPointLightsData Build();
	};

	struct PredefinedPointShadowAtlasEntryData
	{
		nzsl::FieldOffsets fieldOffsets;

		std::size_t offset;
		std::size_t size;

		std::size_t totalSize;

		static constexpr PredefinedPointShadowAtlasEntryData Build();
	};

	struct PredefinedSpotLightData
	{
		nzsl::FieldOffsets fieldOffsets;

		std::size_t colorOffset;
		std::size_t directionOffset;
		std::size_t positionOffset;
		std::size_t ambientFactorOffset;
		std::size_t diffuseFactorOffset;
		std::size_t innerAngleOffset;
		std::size_t outerAngleOffset;
		std::size_t invRadiusOffset;
		std::size_t shadowIndexOffset;
		std::size_t viewProjMatrixOffset;

		std::size_t totalSize;

		static constexpr PredefinedSpotLightData Build();
	};

	struct PredefinedSpotLightsData
	{
		nzsl::FieldOffsets fieldOffsets;

		std::size_t lightCount;
		std::size_t lights; //< dynamic array

		std::size_t totalSize; //< not including lights

		static constexpr PredefinedSpotLightsData Build();
	};

	struct PredefinedSpotShadowAtlasEntryData
	{
		nzsl::FieldOffsets fieldOffsets;

		std::size_t offset;
		std::size_t size;

		std::size_t totalSize;

		static constexpr PredefinedSpotShadowAtlasEntryData Build();
	};

	struct PredefinedInstanceData
	{
		nzsl::FieldOffsets fieldOffsets;

		std::size_t invWorldMatrixOffset;
		std::size_t worldMatrixOffset;

		std::size_t totalSize;

		static constexpr PredefinedInstanceData Build();
	};

	struct PredefinedSkeletalData
	{
		nzsl::FieldOffsets fieldOffsets;

		std::size_t jointMatricesOffset;

		std::size_t totalSize;

		static constexpr std::size_t MaxMatricesCount = 256;

		static constexpr PredefinedSkeletalData Build();
	};

	struct PredefinedViewerData
	{
		nzsl::FieldOffsets fieldOffsets;

		std::size_t eyePositionOffset;
		std::size_t invProjMatrixOffset;
		std::size_t invTargetSizeOffset;
		std::size_t invViewMatrixOffset;
		std::size_t invViewProjMatrixOffset;
		std::size_t projMatrixOffset;
		std::size_t targetSizeOffset;
		std::size_t viewMatrixOffset;
		std::size_t viewProjMatrixOffset;
		std::size_t nearPlaneOffset;
		std::size_t farPlaneOffset;

		std::size_t totalSize;

		static constexpr PredefinedViewerData Build();
	};
}

#include <Nazara/Graphics/PredefinedShaderStructBuilder.inl>

#endif // NAZARA_GRAPHICS_PREDEFINEDSHADERSTRUCTBUILDER_HPP
