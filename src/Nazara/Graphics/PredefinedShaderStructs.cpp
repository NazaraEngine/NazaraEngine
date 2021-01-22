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

		FieldOffsets lightStruct(StructLayout_Std140);
		lightData.innerOffsets.type = lightStruct.AddField(StructFieldType_Int1);
		lightData.innerOffsets.color = lightStruct.AddField(StructFieldType_Float4); 
		lightData.innerOffsets.factor = lightStruct.AddField(StructFieldType_Float2); 
		lightData.innerOffsets.parameter1 = lightStruct.AddField(StructFieldType_Float4);
		lightData.innerOffsets.parameter2 = lightStruct.AddField(StructFieldType_Float4);
		lightData.innerOffsets.parameter3 = lightStruct.AddField(StructFieldType_Float2);
		lightData.innerOffsets.shadowMappingFlag = lightStruct.AddField(StructFieldType_Bool1);

		lightData.innerOffsets.totalSize = lightStruct.GetAlignedSize();

		FieldOffsets lightDataStruct(StructLayout_Std140);
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
			"Light",
			"LightData",
			std::move(lightDataVariables)
		};

		return uniformBlock;
	}

	PredefinedInstanceData PredefinedInstanceData::GetOffsets()
	{
		FieldOffsets viewerStruct(StructLayout_Std140);

		PredefinedInstanceData instanceData;
		instanceData.worldMatrixOffset = viewerStruct.AddMatrix(StructFieldType_Float1, 4, 4, true);
		instanceData.invWorldMatrixOffset = viewerStruct.AddMatrix(StructFieldType_Float1, 4, 4, true);

		instanceData.totalSize = viewerStruct.GetAlignedSize();

		return instanceData;
	}

	MaterialSettings::SharedUniformBlock PredefinedInstanceData::GetUniformBlock()
	{
		PredefinedInstanceData instanceData = GetOffsets();

		std::vector<MaterialSettings::UniformVariable> instanceDataVariables;
		instanceDataVariables.assign({
			{
				"WorldMatrix",
				instanceData.worldMatrixOffset
			},
			{
				"InvWorldMatrix",
				instanceData.invWorldMatrixOffset
			},
		});

		MaterialSettings::SharedUniformBlock uniformBlock = {
			"Instance",
			"InstanceData",
			std::move(instanceDataVariables)
		};

		return uniformBlock;
	}

	PredefinedViewerData PredefinedViewerData::GetOffsets()
	{
		FieldOffsets viewerStruct(StructLayout_Std140);

		PredefinedViewerData viewerData;
		viewerData.projMatrixOffset = viewerStruct.AddMatrix(StructFieldType_Float1, 4, 4, true);
		viewerData.invProjMatrixOffset = viewerStruct.AddMatrix(StructFieldType_Float1, 4, 4, true);
		viewerData.viewMatrixOffset = viewerStruct.AddMatrix(StructFieldType_Float1, 4, 4, true);
		viewerData.invViewMatrixOffset = viewerStruct.AddMatrix(StructFieldType_Float1, 4, 4, true);
		viewerData.viewProjMatrixOffset = viewerStruct.AddMatrix(StructFieldType_Float1, 4, 4, true);
		viewerData.invViewProjMatrixOffset = viewerStruct.AddMatrix(StructFieldType_Float1, 4, 4, true);
		viewerData.targetSizeOffset = viewerStruct.AddField(StructFieldType_Float2);
		viewerData.invTargetSizeOffset = viewerStruct.AddField(StructFieldType_Float2);
		viewerData.eyePositionOffset = viewerStruct.AddField(StructFieldType_Float3);

		viewerData.totalSize = viewerStruct.GetAlignedSize();

		return viewerData;
	}

	MaterialSettings::SharedUniformBlock PredefinedViewerData::GetUniformBlock()
	{
		PredefinedViewerData viewerData = GetOffsets();

		std::vector<MaterialSettings::UniformVariable> viewerDataVariables;
		viewerDataVariables.assign({
			{
				"ProjMatrix",
				viewerData.projMatrixOffset
			},
			{
				"InvProjMatrix",
				viewerData.invProjMatrixOffset
			},
			{
				"ViewMatrix",
				viewerData.viewMatrixOffset
			},
			{
				"InvViewMatrix",
				viewerData.invViewMatrixOffset
			},
			{
				"ViewProjMatrix",
				viewerData.viewProjMatrixOffset
			},
			{
				"InvViewProjMatrix",
				viewerData.invViewProjMatrixOffset
			},
			{
				"TargetSize",
				viewerData.targetSizeOffset
			},
			{
				"InvTargetSize",
				viewerData.invTargetSizeOffset
			},
			{
				"EyePosition",
				viewerData.eyePositionOffset
			}
		});

		MaterialSettings::SharedUniformBlock uniformBlock = {
			"Viewer",
			"ViewerData",
			std::move(viewerDataVariables)
		};

		return uniformBlock;
	}
}
