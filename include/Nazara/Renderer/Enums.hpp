// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ENUMS_RENDERER_HPP
#define NAZARA_ENUMS_RENDERER_HPP

enum nzAttachmentPoint
{
	nzAttachmentPoint_Color,
	nzAttachmentPoint_Depth,
	nzAttachmentPoint_DepthStencil,
	nzAttachmentPoint_Stencil,

	nzAttachmentPoint_Max = nzAttachmentPoint_Stencil
};

enum nzBlendFunc
{
	nzBlendFunc_DestAlpha,
	nzBlendFunc_DestColor,
	nzBlendFunc_SrcAlpha,
	nzBlendFunc_SrcColor,
	nzBlendFunc_InvDestAlpha,
	nzBlendFunc_InvDestColor,
	nzBlendFunc_InvSrcAlpha,
	nzBlendFunc_InvSrcColor,
	nzBlendFunc_One,
	nzBlendFunc_Zero,

	nzBlendFunc_Max = nzBlendFunc_Zero
};

enum nzFaceCulling
{
	nzFaceCulling_Back,
	nzFaceCulling_Front,
	nzFaceCulling_FrontAndBack,

	nzFaceCulling_Max = nzFaceCulling_FrontAndBack
};

enum nzFaceFilling
{
	nzFaceFilling_Point,
	nzFaceFilling_Line,
	nzFaceFilling_Fill,

	nzFaceFilling_Max = nzFaceFilling_Fill
};

enum nzMatrixType
{
	// Matrices de base
	nzMatrixType_Projection,
	nzMatrixType_View,
	nzMatrixType_World,

	// Matrices combinées
	nzMatrixType_ViewProj,
	nzMatrixType_WorldView,
	nzMatrixType_WorldViewProj,

	nzMatrixType_Max = nzMatrixType_WorldViewProj
};

enum nzPixelBufferType
{
	nzPixelBufferType_Pack,
	nzPixelBufferType_Unpack,

	nzPixelBufferType_Max = nzPixelBufferType_Unpack
};

enum nzRendererCap
{
	nzRendererCap_AnisotropicFilter,
	nzRendererCap_FP64,
	nzRendererCap_HardwareBuffer,
	nzRendererCap_Instancing,
	nzRendererCap_MultipleRenderTargets,
	nzRendererCap_OcclusionQuery,
	nzRendererCap_PixelBufferObject,
	nzRendererCap_RenderTexture,
	nzRendererCap_Texture3D,
	nzRendererCap_TextureCubemap,
	nzRendererCap_TextureMulti,
	nzRendererCap_TextureNPOT,

	nzRendererCap_Max = nzRendererCap_TextureNPOT
};

enum nzRendererClearFlags
{
	nzRendererClear_Color   = 0x1,
	nzRendererClear_Depth   = 0x2,
	nzRendererClear_Stencil = 0x4,

	nzRendererClear_Max = nzRendererClear_Stencil*2-1
};

enum nzRendererComparison
{
	nzRendererComparison_Always,
	nzRendererComparison_Equal,
	nzRendererComparison_Greater,
	nzRendererComparison_GreaterOrEqual,
	nzRendererComparison_Less,
	nzRendererComparison_LessOrEqual,
	nzRendererComparison_Never,

	nzRendererComparison_Max = nzRendererComparison_Never
};

enum nzRendererParameter
{
	nzRendererParameter_Blend,
	nzRendererParameter_ColorWrite,
	nzRendererParameter_DepthBuffer,
	nzRendererParameter_DepthWrite,
	nzRendererParameter_FaceCulling,
	nzRendererParameter_ScissorTest,
	nzRendererParameter_StencilTest,

	nzRendererParameter_Max = nzRendererParameter_StencilTest
};

enum nzSamplerFilter
{
	nzSamplerFilter_Unknown = -1,

	nzSamplerFilter_Bilinear,
	nzSamplerFilter_Nearest,
	nzSamplerFilter_Trilinear,

	nzSamplerFilter_Default,

	nzSamplerFilter_Max = nzSamplerFilter_Default
};

enum nzSamplerWrap
{
	nzSamplerWrap_Unknown = -1,

	nzSamplerWrap_Clamp,
	nzSamplerWrap_MirroredRepeat,
	nzSamplerWrap_Repeat,

	nzSamplerWrap_Default,

	nzSamplerWrap_Max = nzSamplerWrap_Repeat
};

enum nzShaderFlags
{
	nzShaderFlags_None = 0,

	nzShaderFlags_Deferred   = 0x1,
	nzShaderFlags_FlipUVs    = 0x2,
	nzShaderFlags_Instancing = 0x4,

	nzShaderFlags_Max = nzShaderFlags_Instancing*2-1
};

enum nzShaderLanguage
{
	nzShaderLanguage_Unknown = -1,

	nzShaderLanguage_Cg,
	nzShaderLanguage_GLSL,

	nzShaderLanguage_Max = nzShaderLanguage_GLSL
};

enum nzShaderTarget
{
	nzShaderTarget_FullscreenQuad,
	nzShaderTarget_Model,
	nzShaderTarget_None,
	nzShaderTarget_Sprite,

	nzShaderTarget_Max = nzShaderTarget_Sprite
};

enum nzShaderUniform
{
	nzShaderUniform_EyePosition,
	nzShaderUniform_InvTargetSize,
	nzShaderUniform_MaterialAlphaMap,
	nzShaderUniform_MaterialAlphaThreshold,
	nzShaderUniform_MaterialAmbient,
	nzShaderUniform_MaterialDiffuse,
	nzShaderUniform_MaterialDiffuseMap,
	nzShaderUniform_MaterialEmissiveMap,
	nzShaderUniform_MaterialHeightMap,
	nzShaderUniform_MaterialNormalMap,
	nzShaderUniform_MaterialShininess,
	nzShaderUniform_MaterialSpecular,
	nzShaderUniform_MaterialSpecularMap,
	nzShaderUniform_ProjMatrix,
	nzShaderUniform_SceneAmbient,
	nzShaderUniform_TargetSize,
	nzShaderUniform_ViewMatrix,
	nzShaderUniform_ViewProjMatrix,
	nzShaderUniform_WorldMatrix,
	nzShaderUniform_WorldViewMatrix,
	nzShaderUniform_WorldViewProjMatrix,

	nzShaderUniform_Max = nzShaderUniform_WorldViewProjMatrix
};

enum nzShaderType
{
	nzShaderType_Fragment,
	nzShaderType_Geometry,
	nzShaderType_Vertex,

	nzShaderType_Max = nzShaderType_Vertex
};

enum nzStencilOperation
{
	nzStencilOperation_Decrement,
	nzStencilOperation_DecrementToSaturation,
	nzStencilOperation_Increment,
	nzStencilOperation_IncrementToSaturation,
	nzStencilOperation_Invert,
	nzStencilOperation_Keep,
	nzStencilOperation_Replace,
	nzStencilOperation_Zero,

	nzStencilOperation_Max = nzStencilOperation_Zero
};

#endif // NAZARA_ENUMS_RENDERER_HPP
