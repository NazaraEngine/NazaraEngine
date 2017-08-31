// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ENUMS_RENDERER_HPP
#define NAZARA_ENUMS_RENDERER_HPP

namespace Nz
{
	enum AttachmentPoint
	{
		AttachmentPoint_Color,
		AttachmentPoint_Depth,
		AttachmentPoint_DepthStencil,
		AttachmentPoint_Stencil,

		AttachmentPoint_Max = AttachmentPoint_Stencil
	};

	enum GpuQueryCondition
	{
		GpuQueryCondition_Region_NoWait,
		GpuQueryCondition_Region_Wait,
		GpuQueryCondition_NoWait,
		GpuQueryCondition_Wait,

		GpuQueryCondition_Max = GpuQueryCondition_Wait
	};

	enum GpuQueryMode
	{
		GpuQueryMode_AnySamplesPassed,
		GpuQueryMode_AnySamplesPassedConservative,
		GpuQueryMode_PrimitiveGenerated,
		GpuQueryMode_SamplesPassed,
		GpuQueryMode_TimeElapsed,
		GpuQueryMode_TransformFeedbackPrimitivesWritten,

		GpuQueryMode_Max = GpuQueryMode_TransformFeedbackPrimitivesWritten
	};

	enum MatrixType
	{
		// Matrices de base
		MatrixType_Projection,
		MatrixType_View,
		MatrixType_World,

		// Matrices combinées
		MatrixType_ViewProj,
		MatrixType_WorldView,
		MatrixType_WorldViewProj,

		// Matrice inversées
		MatrixType_InvProjection,
		MatrixType_InvView,
		MatrixType_InvViewProj,
		MatrixType_InvWorld,
		MatrixType_InvWorldView,
		MatrixType_InvWorldViewProj,

		MatrixType_Max = MatrixType_InvWorldViewProj
	};

	enum PixelBufferType
	{
		PixelBufferType_Pack,
		PixelBufferType_Unpack,

		PixelBufferType_Max = PixelBufferType_Unpack
	};

	enum RendererCap
	{
		RendererCap_AnisotropicFilter,
		RendererCap_FP64,
		RendererCap_Instancing,

		RendererCap_Max = RendererCap_Instancing
	};

	enum RendererBufferFlags
	{
		RendererBuffer_Color   = 0x1,
		RendererBuffer_Depth   = 0x2,
		RendererBuffer_Stencil = 0x4,

		RendererBuffer_Max = RendererBuffer_Stencil*2-1
	};

	enum ShaderUniform
	{
		ShaderUniform_InvProjMatrix,
		ShaderUniform_InvTargetSize,
		ShaderUniform_InvViewMatrix,
		ShaderUniform_InvViewProjMatrix,
		ShaderUniform_InvWorldMatrix,
		ShaderUniform_InvWorldViewMatrix,
		ShaderUniform_InvWorldViewProjMatrix,
		ShaderUniform_ProjMatrix,
		ShaderUniform_TargetSize,
		ShaderUniform_ViewMatrix,
		ShaderUniform_ViewProjMatrix,
		ShaderUniform_WorldMatrix,
		ShaderUniform_WorldViewMatrix,
		ShaderUniform_WorldViewProjMatrix,

		ShaderUniform_Max = ShaderUniform_WorldViewProjMatrix
	};

	enum ShaderStageType
	{
		ShaderStageType_Fragment,
		ShaderStageType_Geometry,
		ShaderStageType_Vertex,

		ShaderStageType_Max = ShaderStageType_Vertex
	};
}

#endif // NAZARA_ENUMS_RENDERER_HPP
