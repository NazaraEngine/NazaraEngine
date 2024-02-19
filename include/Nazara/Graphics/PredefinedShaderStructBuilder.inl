// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
NAZARA_WARNING_PUSH()
NAZARA_WARNING_CLANG_GCC_DISABLE("-Wmissing-field-initializers")

	// PredefinedDirectionalLightData
	constexpr PredefinedDirectionalLightData PredefinedDirectionalLightData::Build()
	{
		PredefinedDirectionalLightData lightData = { nzsl::FieldOffsets(nzsl::StructLayout::Std140) };
		lightData.colorOffset = lightData.fieldOffsets.AddField(nzsl::StructFieldType::Float3);
		lightData.directionOffset = lightData.fieldOffsets.AddField(nzsl::StructFieldType::Float3);
		lightData.invShadowMapSizeOffset = lightData.fieldOffsets.AddField(nzsl::StructFieldType::Float2);
		lightData.ambientFactorOffset = lightData.fieldOffsets.AddField(nzsl::StructFieldType::Float1);
		lightData.diffuseFactorOffset = lightData.fieldOffsets.AddField(nzsl::StructFieldType::Float1);
		lightData.cascadeCountOffset = lightData.fieldOffsets.AddField(nzsl::StructFieldType::UInt1);
		lightData.cascadeFarPlanesOffset = lightData.fieldOffsets.AddFieldArray(nzsl::StructFieldType::Float1, MaxLightCascadeCount);
		lightData.cascadeViewProjMatricesOffset = lightData.fieldOffsets.AddMatrixArray(nzsl::StructFieldType::Float1, 4, 4, true, MaxLightCascadeCount);

		lightData.totalSize = lightData.fieldOffsets.GetAlignedSize();

		return lightData;
	}

	// PredefinedPointLightData
	constexpr PredefinedPointLightData PredefinedPointLightData::Build()
	{
		PredefinedPointLightData lightData = { nzsl::FieldOffsets(nzsl::StructLayout::Std140) };
		lightData.colorOffset = lightData.fieldOffsets.AddField(nzsl::StructFieldType::Float3);
		lightData.positionOffset = lightData.fieldOffsets.AddField(nzsl::StructFieldType::Float3);
		lightData.invShadowMapSizeOffset = lightData.fieldOffsets.AddField(nzsl::StructFieldType::Float2);
		lightData.ambientFactorOffset = lightData.fieldOffsets.AddField(nzsl::StructFieldType::Float1);
		lightData.diffuseFactorOffset = lightData.fieldOffsets.AddField(nzsl::StructFieldType::Float1);
		lightData.radiusOffset = lightData.fieldOffsets.AddField(nzsl::StructFieldType::Float1);
		lightData.invRadiusOffset = lightData.fieldOffsets.AddField(nzsl::StructFieldType::Float1);

		lightData.totalSize = lightData.fieldOffsets.GetAlignedSize();

		return lightData;
	}

	// PredefinedSpotLightData
	constexpr PredefinedSpotLightData PredefinedSpotLightData::Build()
	{
		PredefinedSpotLightData lightData = { nzsl::FieldOffsets(nzsl::StructLayout::Std140) };
		lightData.colorOffset = lightData.fieldOffsets.AddField(nzsl::StructFieldType::Float3);
		lightData.directionOffset = lightData.fieldOffsets.AddField(nzsl::StructFieldType::Float3);
		lightData.positionOffset = lightData.fieldOffsets.AddField(nzsl::StructFieldType::Float3);
		lightData.invShadowMapSizeOffset = lightData.fieldOffsets.AddField(nzsl::StructFieldType::Float2);
		lightData.ambientFactorOffset = lightData.fieldOffsets.AddField(nzsl::StructFieldType::Float1);
		lightData.diffuseFactorOffset = lightData.fieldOffsets.AddField(nzsl::StructFieldType::Float1);
		lightData.innerAngleOffset = lightData.fieldOffsets.AddField(nzsl::StructFieldType::Float1);
		lightData.outerAngleOffset = lightData.fieldOffsets.AddField(nzsl::StructFieldType::Float1);
		lightData.invRadiusOffset = lightData.fieldOffsets.AddField(nzsl::StructFieldType::Float1);
		lightData.viewProjMatrixOffset = lightData.fieldOffsets.AddMatrix(nzsl::StructFieldType::Float1, 4, 4, true);

		lightData.totalSize = lightData.fieldOffsets.GetAlignedSize();

		return lightData;
	}

	// PredefinedLightData
	constexpr PredefinedLightData PredefinedLightData::Build()
	{
		constexpr PredefinedDirectionalLightData DirectionalLight = PredefinedDirectionalLightData::Build();
		constexpr PredefinedPointLightData PointLight = PredefinedPointLightData::Build();
		constexpr PredefinedSpotLightData SpotLight = PredefinedSpotLightData::Build();

		PredefinedLightData lightData = { nzsl::FieldOffsets(nzsl::StructLayout::Std140) };
		lightData.directionalLightsOffset = lightData.fieldOffsets.AddStructArray(DirectionalLight.fieldOffsets, MaxLightCount);
		lightData.pointLightsOffset = lightData.fieldOffsets.AddStructArray(PointLight.fieldOffsets, MaxLightCount);
		lightData.spotLightsOffset = lightData.fieldOffsets.AddStructArray(SpotLight.fieldOffsets, MaxLightCount);

		lightData.directionalLightCountOffset = lightData.fieldOffsets.AddField(nzsl::StructFieldType::UInt1);
		lightData.pointLightCountOffset = lightData.fieldOffsets.AddField(nzsl::StructFieldType::UInt1);
		lightData.spotLightCountOffset = lightData.fieldOffsets.AddField(nzsl::StructFieldType::UInt1);

		lightData.totalSize = lightData.fieldOffsets.GetAlignedSize();

		return lightData;
	}

	// PredefinedInstanceData
	constexpr PredefinedInstanceData PredefinedInstanceData::Build()
	{
		PredefinedInstanceData instanceData = { nzsl::FieldOffsets(nzsl::StructLayout::Std140) };
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

