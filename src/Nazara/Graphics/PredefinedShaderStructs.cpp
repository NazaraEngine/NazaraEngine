// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/PredefinedShaderStructs.hpp>
#include <Nazara/Utility/FieldOffsets.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	PredefinedLightData PredefinedLightData::GetOffset()
	{
		PredefinedLightData lightData;

		FieldOffsets lightStruct(StructLayout::Std140);
		lightData.innerOffsets.type = lightStruct.AddField(StructFieldType::Int1);
		lightData.innerOffsets.color = lightStruct.AddField(StructFieldType::Float4); 
		lightData.innerOffsets.factor = lightStruct.AddField(StructFieldType::Float2); 
		lightData.innerOffsets.parameter1 = lightStruct.AddField(StructFieldType::Float4);
		lightData.innerOffsets.parameter2 = lightStruct.AddField(StructFieldType::Float4);
		lightData.innerOffsets.parameter3 = lightStruct.AddField(StructFieldType::Float2);
		lightData.innerOffsets.shadowMappingFlag = lightStruct.AddField(StructFieldType::Bool1);

		lightData.innerOffsets.totalSize = lightStruct.GetAlignedSize();

		FieldOffsets lightDataStruct(StructLayout::Std140);
		for (std::size_t& lightOffset : lightData.lightArray)
			lightOffset = lightDataStruct.AddStruct(lightStruct);

		lightData.lightArraySize = lightDataStruct.GetAlignedSize();

		return lightData;
	}

	MaterialSettings::SharedUniformBlock PredefinedLightData::GetUniformBlock()
	{
		PredefinedLightData lightData = GetOffset();

		std::vector<MaterialSettings::UniformVariable> lightDataVariables;
		for (std::size_t i = 0; i < lightData.lightArray.size(); ++i)
		{
			lightDataVariables.push_back({
				"LightData[" + std::to_string(i) + "]",
				lightData.lightArray[i]
			});
		}

		MaterialSettings::SharedUniformBlock uniformBlock = {
			0, //< FIXME
			"Light",
			std::move(lightDataVariables)
		};

		return uniformBlock;
	}

	PredefinedInstanceData PredefinedInstanceData::GetOffsets()
	{
		FieldOffsets viewerStruct(StructLayout::Std140);

		PredefinedInstanceData instanceData;
		instanceData.worldMatrixOffset = viewerStruct.AddMatrix(StructFieldType::Float1, 4, 4, true);
		instanceData.invWorldMatrixOffset = viewerStruct.AddMatrix(StructFieldType::Float1, 4, 4, true);

		instanceData.totalSize = viewerStruct.GetAlignedSize();

		return instanceData;
	}

	PredefinedViewerData PredefinedViewerData::GetOffsets()
	{
		FieldOffsets viewerStruct(StructLayout::Std140);

		PredefinedViewerData viewerData;
		viewerData.projMatrixOffset = viewerStruct.AddMatrix(StructFieldType::Float1, 4, 4, true);
		viewerData.invProjMatrixOffset = viewerStruct.AddMatrix(StructFieldType::Float1, 4, 4, true);
		viewerData.viewMatrixOffset = viewerStruct.AddMatrix(StructFieldType::Float1, 4, 4, true);
		viewerData.invViewMatrixOffset = viewerStruct.AddMatrix(StructFieldType::Float1, 4, 4, true);
		viewerData.viewProjMatrixOffset = viewerStruct.AddMatrix(StructFieldType::Float1, 4, 4, true);
		viewerData.invViewProjMatrixOffset = viewerStruct.AddMatrix(StructFieldType::Float1, 4, 4, true);
		viewerData.targetSizeOffset = viewerStruct.AddField(StructFieldType::Float2);
		viewerData.invTargetSizeOffset = viewerStruct.AddField(StructFieldType::Float2);
		viewerData.eyePositionOffset = viewerStruct.AddField(StructFieldType::Float3);

		viewerData.totalSize = viewerStruct.GetAlignedSize();

		return viewerData;
	}
}
