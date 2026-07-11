// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Renderer/DrawIndirect.hpp>

namespace Nz
{
NAZARA_WARNING_PUSH()
NAZARA_WARNING_CLANG_GCC_DISABLE("-Wmissing-field-initializers")

	// PredefinedDirectionalLightData
	constexpr PredefinedDirectionalLightData PredefinedDirectionalLightData::Build()
	{
		PredefinedDirectionalLightData lightData = { nzsl::FieldOffsets(nzsl::StructLayout::Std430) };
		lightData.colorOffset = lightData.fieldOffsets.AddField(nzsl::StructFieldType::Float3);
		lightData.directionOffset = lightData.fieldOffsets.AddField(nzsl::StructFieldType::Float3);
		lightData.ambientFactorOffset = lightData.fieldOffsets.AddField(nzsl::StructFieldType::Float1);
		lightData.diffuseFactorOffset = lightData.fieldOffsets.AddField(nzsl::StructFieldType::Float1);
		lightData.shadowIndexOffset = lightData.fieldOffsets.AddField(nzsl::StructFieldType::UInt1);

		lightData.totalSize = lightData.fieldOffsets.GetAlignedSize();

		return lightData;
	}

	// PredefinedDirectionalLightsData
	constexpr PredefinedDirectionalLightsData PredefinedDirectionalLightsData::Build()
	{
		constexpr PredefinedDirectionalLightData PredefinedDirectionalLightOffset = PredefinedDirectionalLightData::Build();

		PredefinedDirectionalLightsData lightsData = { nzsl::FieldOffsets(nzsl::StructLayout::Std430) };
		lightsData.lightCount = lightsData.fieldOffsets.AddField(nzsl::StructFieldType::UInt1);
		lightsData.lights = lightsData.fieldOffsets.AddStructArray(PredefinedDirectionalLightOffset.fieldOffsets, 1);

		lightsData.totalSize = lightsData.lights;

		return lightsData;
	}

	// PredefinedDirectionalShadowAtlasEntryData
	constexpr PredefinedDirectionalShadowAtlasEntryData PredefinedDirectionalShadowAtlasEntryData::Build()
	{
		PredefinedDirectionalShadowAtlasEntryData entryData = { nzsl::FieldOffsets(nzsl::StructLayout::Std430) };
		entryData.offset = entryData.fieldOffsets.AddFieldArray(nzsl::StructFieldType::Float2, MaxLightCascadeCount);
		entryData.size = entryData.fieldOffsets.AddFieldArray(nzsl::StructFieldType::Float2, MaxLightCascadeCount);
		entryData.cascadeCount = entryData.fieldOffsets.AddField(nzsl::StructFieldType::UInt1);
		entryData.viewProjMatrices = entryData.fieldOffsets.AddMatrixArray(nzsl::StructFieldType::Float1, 4, 4, true, MaxLightCascadeCount);
		entryData.cascadeDistances = entryData.fieldOffsets.AddFieldArray(nzsl::StructFieldType::Float1, MaxLightCascadeCount);

		entryData.totalSize = entryData.fieldOffsets.GetAlignedSize();

		return entryData;
	}

	// PredefinedIndirectDrawData
	constexpr PredefinedIndirectDrawData PredefinedIndirectDrawData::Build()
	{
		PredefinedIndirectDrawData entryData = { nzsl::FieldOffsets(nzsl::StructLayout::Std430) };
		entryData.drawCommand = entryData.fieldOffsets.AddStruct(sizeof(DrawIndexedIndirectCommand), alignof(DrawIndexedIndirectCommand));
		entryData.boundingSphere = entryData.fieldOffsets.AddField(nzsl::StructFieldType::Float4);

		entryData.totalSize = entryData.fieldOffsets.GetAlignedSize();

		return entryData;
	}

	// PredefinedPointLightData
	constexpr PredefinedPointLightData PredefinedPointLightData::Build()
	{
		PredefinedPointLightData lightData = { nzsl::FieldOffsets(nzsl::StructLayout::Std430) };
		lightData.colorOffset = lightData.fieldOffsets.AddField(nzsl::StructFieldType::Float3);
		lightData.positionOffset = lightData.fieldOffsets.AddField(nzsl::StructFieldType::Float3);
		lightData.ambientFactorOffset = lightData.fieldOffsets.AddField(nzsl::StructFieldType::Float1);
		lightData.diffuseFactorOffset = lightData.fieldOffsets.AddField(nzsl::StructFieldType::Float1);
		lightData.radiusOffset = lightData.fieldOffsets.AddField(nzsl::StructFieldType::Float1);
		lightData.invRadiusOffset = lightData.fieldOffsets.AddField(nzsl::StructFieldType::Float1);
		lightData.shadowIndexOffset = lightData.fieldOffsets.AddField(nzsl::StructFieldType::UInt1);

		lightData.totalSize = lightData.fieldOffsets.GetAlignedSize();

		return lightData;
	}

	// PredefinedPointLightsData
	constexpr PredefinedPointLightsData PredefinedPointLightsData::Build()
	{
		constexpr PredefinedPointLightData PredefinedPointLightOffset = PredefinedPointLightData::Build();

		PredefinedPointLightsData lightsData = { nzsl::FieldOffsets(nzsl::StructLayout::Std430) };
		lightsData.lightCount = lightsData.fieldOffsets.AddField(nzsl::StructFieldType::UInt1);
		lightsData.lights = lightsData.fieldOffsets.AddStructArray(PredefinedPointLightOffset.fieldOffsets, 1);

		lightsData.totalSize = lightsData.lights;

		return lightsData;
	}

	// PredefinedPointShadowAtlasEntryData
	constexpr PredefinedPointShadowAtlasEntryData PredefinedPointShadowAtlasEntryData::Build()
	{
		PredefinedPointShadowAtlasEntryData entryData = { nzsl::FieldOffsets(nzsl::StructLayout::Std430) };
		entryData.offset = entryData.fieldOffsets.AddFieldArray(nzsl::StructFieldType::Float2, 6);
		entryData.size = entryData.fieldOffsets.AddFieldArray(nzsl::StructFieldType::Float2, 6);

		entryData.totalSize = entryData.fieldOffsets.GetAlignedSize();

		return entryData;
	}

	// PredefinedSpotLightData
	constexpr PredefinedSpotLightData PredefinedSpotLightData::Build()
	{
		PredefinedSpotLightData lightData = { nzsl::FieldOffsets(nzsl::StructLayout::Std430) };
		lightData.colorOffset = lightData.fieldOffsets.AddField(nzsl::StructFieldType::Float3);
		lightData.directionOffset = lightData.fieldOffsets.AddField(nzsl::StructFieldType::Float3);
		lightData.positionOffset = lightData.fieldOffsets.AddField(nzsl::StructFieldType::Float3);
		lightData.ambientFactorOffset = lightData.fieldOffsets.AddField(nzsl::StructFieldType::Float1);
		lightData.diffuseFactorOffset = lightData.fieldOffsets.AddField(nzsl::StructFieldType::Float1);
		lightData.innerAngleOffset = lightData.fieldOffsets.AddField(nzsl::StructFieldType::Float1);
		lightData.outerAngleOffset = lightData.fieldOffsets.AddField(nzsl::StructFieldType::Float1);
		lightData.invRadiusOffset = lightData.fieldOffsets.AddField(nzsl::StructFieldType::Float1);
		lightData.shadowIndexOffset = lightData.fieldOffsets.AddField(nzsl::StructFieldType::UInt1);
		lightData.viewProjMatrixOffset = lightData.fieldOffsets.AddMatrix(nzsl::StructFieldType::Float1, 4, 4, true);

		lightData.totalSize = lightData.fieldOffsets.GetAlignedSize();

		return lightData;
	}

	// PredefinedSpotLightsData
	constexpr PredefinedSpotLightsData PredefinedSpotLightsData::Build()
	{
		constexpr PredefinedSpotLightData PredefinedSpotLightOffset = PredefinedSpotLightData::Build();

		PredefinedSpotLightsData lightsData = { nzsl::FieldOffsets(nzsl::StructLayout::Std430) };
		lightsData.lightCount = lightsData.fieldOffsets.AddField(nzsl::StructFieldType::UInt1);
		lightsData.lights = lightsData.fieldOffsets.AddStructArray(PredefinedSpotLightOffset.fieldOffsets, 1);

		lightsData.totalSize = lightsData.lights;

		return lightsData;
	}

	// PredefinedSpotShadowAtlasEntryData
	constexpr PredefinedSpotShadowAtlasEntryData PredefinedSpotShadowAtlasEntryData::Build()
	{
		PredefinedSpotShadowAtlasEntryData entryData = { nzsl::FieldOffsets(nzsl::StructLayout::Std430) };
		entryData.offset = entryData.fieldOffsets.AddField(nzsl::StructFieldType::Float2);
		entryData.size = entryData.fieldOffsets.AddField(nzsl::StructFieldType::Float2);

		entryData.totalSize = entryData.fieldOffsets.GetAlignedSize();

		return entryData;
	}

	// PredefinedInstanceData
	constexpr PredefinedInstanceData PredefinedInstanceData::Build()
	{
		PredefinedInstanceData instanceData = { nzsl::FieldOffsets(nzsl::StructLayout::Std430) };
		instanceData.worldMatrixOffset = instanceData.fieldOffsets.AddMatrix(nzsl::StructFieldType::Float1, 4, 4, true);
		instanceData.invWorldMatrixOffset = instanceData.fieldOffsets.AddMatrix(nzsl::StructFieldType::Float1, 4, 4, true);

		instanceData.totalSize = instanceData.fieldOffsets.GetAlignedSize();

		return instanceData;
	}

	// PredefinedSkeletalData
	constexpr PredefinedSkeletalData PredefinedSkeletalData::Build()
	{
		PredefinedSkeletalData skeletalData = { nzsl::FieldOffsets(nzsl::StructLayout::Std140) };
		skeletalData.jointMatricesOffset = skeletalData.fieldOffsets.AddMatrixArray(nzsl::StructFieldType::Float1, 4, 4, true, MaxMatricesCount);

		skeletalData.totalSize = skeletalData.fieldOffsets.GetAlignedSize();

		return skeletalData;
	}

	// PredefinedViewerData
	constexpr PredefinedViewerData PredefinedViewerData::Build()
	{
		PredefinedViewerData viewerData = { nzsl::FieldOffsets(nzsl::StructLayout::Std140) };
		viewerData.projMatrixOffset = viewerData.fieldOffsets.AddMatrix(nzsl::StructFieldType::Float1, 4, 4, true);
		viewerData.invProjMatrixOffset = viewerData.fieldOffsets.AddMatrix(nzsl::StructFieldType::Float1, 4, 4, true);
		viewerData.viewMatrixOffset = viewerData.fieldOffsets.AddMatrix(nzsl::StructFieldType::Float1, 4, 4, true);
		viewerData.invViewMatrixOffset = viewerData.fieldOffsets.AddMatrix(nzsl::StructFieldType::Float1, 4, 4, true);
		viewerData.viewProjMatrixOffset = viewerData.fieldOffsets.AddMatrix(nzsl::StructFieldType::Float1, 4, 4, true);
		viewerData.invViewProjMatrixOffset = viewerData.fieldOffsets.AddMatrix(nzsl::StructFieldType::Float1, 4, 4, true);
		viewerData.frustumPlaneOffset = viewerData.fieldOffsets.AddFieldArray(nzsl::StructFieldType::Float4, 6);
		viewerData.targetSizeOffset = viewerData.fieldOffsets.AddField(nzsl::StructFieldType::Float2);
		viewerData.invTargetSizeOffset = viewerData.fieldOffsets.AddField(nzsl::StructFieldType::Float2);
		viewerData.eyePositionOffset = viewerData.fieldOffsets.AddField(nzsl::StructFieldType::Float3);
		viewerData.nearPlaneOffset = viewerData.fieldOffsets.AddField(nzsl::StructFieldType::Float1);
		viewerData.farPlaneOffset = viewerData.fieldOffsets.AddField(nzsl::StructFieldType::Float1);

		viewerData.totalSize = viewerData.fieldOffsets.GetAlignedSize();

		return viewerData;
	}

NAZARA_WARNING_POP()
}
