// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/PredefinedShaderStructs.hpp>
#include <Nazara/Shader/FieldOffsets.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	PredefinedLightData PredefinedLightData::GetOffsets()
	{
		PredefinedLightData lightData;

		FieldOffsets lightStruct(StructLayout::Std140);
		lightData.lightMemberOffsets.type = lightStruct.AddField(StructFieldType::Int1);
		lightData.lightMemberOffsets.color = lightStruct.AddField(StructFieldType::Float4); 
		lightData.lightMemberOffsets.factor = lightStruct.AddField(StructFieldType::Float2); 
		lightData.lightMemberOffsets.parameter1 = lightStruct.AddField(StructFieldType::Float4);
		lightData.lightMemberOffsets.parameter2 = lightStruct.AddField(StructFieldType::Float4);
		lightData.lightMemberOffsets.parameter3 = lightStruct.AddField(StructFieldType::Float4);
		lightData.lightMemberOffsets.shadowMappingFlag = lightStruct.AddField(StructFieldType::Bool1);

		lightData.lightSize = lightStruct.GetAlignedSize();

		FieldOffsets lightDataStruct(StructLayout::Std140);
		lightData.lightsOffset = lightDataStruct.AddStructArray(lightStruct, MaxLightCount);
		lightData.lightCountOffset = lightDataStruct.AddField(StructFieldType::UInt1);

		lightData.totalSize = lightDataStruct.GetAlignedSize();

		return lightData;
	}

	MaterialSettings::SharedUniformBlock PredefinedLightData::GetUniformBlock(UInt32 bindingIndex, ShaderStageTypeFlags shaderStages)
	{
		PredefinedLightData lightData = GetOffsets();

		std::vector<MaterialSettings::UniformVariable> variables;
		auto& var = variables.emplace_back();
		var.name = "Lights";
		var.offset = lightData.lightsOffset;

		MaterialSettings::SharedUniformBlock uniformBlock = {
			bindingIndex,
			"LightData",
			std::move(variables),
			shaderStages
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

	MaterialSettings::SharedUniformBlock PredefinedInstanceData::GetUniformBlock(UInt32 bindingIndex, ShaderStageTypeFlags shaderStages)
	{
		PredefinedInstanceData instanceData = GetOffsets();

		std::vector<MaterialSettings::UniformVariable> variables = {
			{
				{ "WorldMatrix", instanceData.worldMatrixOffset },
				{ "InvWorldMatrix", instanceData.invWorldMatrixOffset }
			}
		};

		MaterialSettings::SharedUniformBlock uniformBlock = {
			bindingIndex,
			"InstanceData",
			std::move(variables),
			shaderStages
		};

		return uniformBlock;
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

	MaterialSettings::SharedUniformBlock PredefinedViewerData::GetUniformBlock(UInt32 bindingIndex, ShaderStageTypeFlags shaderStages)
	{
		PredefinedViewerData viewerData = GetOffsets();

		std::vector<MaterialSettings::UniformVariable> variables = {
			{
				{ "EyePosition", viewerData.eyePositionOffset },
				{ "InvProjMatrix", viewerData.invProjMatrixOffset },
				{ "InvTargetSize", viewerData.invTargetSizeOffset },
				{ "InvViewMatrix", viewerData.invViewMatrixOffset },
				{ "InvViewProjMatrix", viewerData.invViewProjMatrixOffset },
				{ "ProjMatrix", viewerData.projMatrixOffset },
				{ "TargetSize", viewerData.targetSizeOffset },
				{ "ViewMatrix", viewerData.viewMatrixOffset },
				{ "ViewProjMatrix", viewerData.viewProjMatrixOffset }
			}
		};

		MaterialSettings::SharedUniformBlock uniformBlock = {
			bindingIndex,
			"ViewerData",
			std::move(variables),
			shaderStages
		};

		return uniformBlock;
	}

}
