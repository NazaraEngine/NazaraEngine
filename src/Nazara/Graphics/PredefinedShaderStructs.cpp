// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/PredefinedShaderStructs.hpp>
#include <NZSL/Math/FieldOffsets.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	// PredefinedLightData
	PredefinedLightData PredefinedLightData::GetOffsets()
	{
		PredefinedLightData lightData;

		nzsl::FieldOffsets lightStruct(nzsl::StructLayout::Std140);
		lightData.lightMemberOffsets.type = lightStruct.AddField(nzsl::StructFieldType::Int1);
		lightData.lightMemberOffsets.color = lightStruct.AddField(nzsl::StructFieldType::Float4); 
		lightData.lightMemberOffsets.factor = lightStruct.AddField(nzsl::StructFieldType::Float2); 
		lightData.lightMemberOffsets.parameter1 = lightStruct.AddField(nzsl::StructFieldType::Float4);
		lightData.lightMemberOffsets.parameter2 = lightStruct.AddField(nzsl::StructFieldType::Float4);
		lightData.lightMemberOffsets.parameter3 = lightStruct.AddField(nzsl::StructFieldType::Float4);
		lightData.lightMemberOffsets.shadowMapSize = lightStruct.AddField(nzsl::StructFieldType::Float2);
		lightData.lightMemberOffsets.viewProjMatrix = lightStruct.AddMatrix(nzsl::StructFieldType::Float1, 4, 4, true);

		lightData.lightSize = lightStruct.GetAlignedSize();

		nzsl::FieldOffsets lightDataStruct(nzsl::StructLayout::Std140);
		lightData.lightsOffset = lightDataStruct.AddStructArray(lightStruct, MaxLightCount);
		lightData.lightCountOffset = lightDataStruct.AddField(nzsl::StructFieldType::UInt1);

		lightData.totalSize = lightDataStruct.GetAlignedSize();

		return lightData;
	}

	// PredefinedInstanceData
	PredefinedInstanceData PredefinedInstanceData::GetOffsets()
	{
		nzsl::FieldOffsets viewerStruct(nzsl::StructLayout::Std140);

		PredefinedInstanceData instanceData;
		instanceData.worldMatrixOffset = viewerStruct.AddMatrix(nzsl::StructFieldType::Float1, 4, 4, true);
		instanceData.invWorldMatrixOffset = viewerStruct.AddMatrix(nzsl::StructFieldType::Float1, 4, 4, true);

		instanceData.totalSize = viewerStruct.GetAlignedSize();

		return instanceData;
	}

	// PredefinedSkeletalData
	PredefinedSkeletalData PredefinedSkeletalData::GetOffsets()
	{
		nzsl::FieldOffsets skeletalStruct(nzsl::StructLayout::Std140);

		PredefinedSkeletalData skeletalData;
		skeletalData.jointMatricesOffset = skeletalStruct.AddMatrixArray(nzsl::StructFieldType::Float1, 4, 4, true, MaxMatricesCount);

		skeletalData.totalSize = skeletalStruct.GetAlignedSize();

		return skeletalData;
	}

	// PredefinedViewerData
	PredefinedViewerData PredefinedViewerData::GetOffsets()
	{
		nzsl::FieldOffsets viewerStruct(nzsl::StructLayout::Std140);

		PredefinedViewerData viewerData;
		viewerData.projMatrixOffset = viewerStruct.AddMatrix(nzsl::StructFieldType::Float1, 4, 4, true);
		viewerData.invProjMatrixOffset = viewerStruct.AddMatrix(nzsl::StructFieldType::Float1, 4, 4, true);
		viewerData.viewMatrixOffset = viewerStruct.AddMatrix(nzsl::StructFieldType::Float1, 4, 4, true);
		viewerData.invViewMatrixOffset = viewerStruct.AddMatrix(nzsl::StructFieldType::Float1, 4, 4, true);
		viewerData.viewProjMatrixOffset = viewerStruct.AddMatrix(nzsl::StructFieldType::Float1, 4, 4, true);
		viewerData.invViewProjMatrixOffset = viewerStruct.AddMatrix(nzsl::StructFieldType::Float1, 4, 4, true);
		viewerData.targetSizeOffset = viewerStruct.AddField(nzsl::StructFieldType::Float2);
		viewerData.invTargetSizeOffset = viewerStruct.AddField(nzsl::StructFieldType::Float2);
		viewerData.eyePositionOffset = viewerStruct.AddField(nzsl::StructFieldType::Float3);

		viewerData.totalSize = viewerStruct.GetAlignedSize();

		return viewerData;
	}
}
