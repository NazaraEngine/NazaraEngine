// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ENUMS_RENDERER_HPP
#define NAZARA_ENUMS_RENDERER_HPP

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
	nzBlendFunc_Zero
};

enum nzFaceCulling
{
	nzFaceCulling_Back,
	nzFaceCulling_Front,
	nzFaceCulling_FrontAndBack
};

enum nzFaceFilling
{
	nzFaceFilling_Point,
	nzFaceFilling_Line,
	nzFaceFilling_Fill
};

enum nzMatrixType
{
	nzMatrixType_Projection,
	nzMatrixType_View,
	nzMatrixType_World,

	nzMatrixType_Max = nzMatrixType_World
};

enum nzPixelBufferType
{
	nzPixelBufferType_Pack,
	nzPixelBufferType_Unpack
};

enum nzRendererCap
{
	nzRendererCap_AnisotropicFilter,
	nzRendererCap_FP64,
	nzRendererCap_HardwareBuffer,
	nzRendererCap_MultipleRenderTargets,
	nzRendererCap_OcclusionQuery,
	nzRendererCap_PixelBufferObject,
	nzRendererCap_Texture3D,
	nzRendererCap_TextureCubemap,
	nzRendererCap_TextureMulti,
	nzRendererCap_TextureNPOT,

	nzRendererCap_Max = nzRendererCap_TextureNPOT
};

enum nzRendererClear
{
	nzRendererClear_Color = 0x01,
	nzRendererClear_Depth = 0x02,
	nzRendererClear_Stencil = 0x04
};

enum nzRendererComparison
{
	nzRendererComparison_Always,
	nzRendererComparison_Equal,
	nzRendererComparison_Greater,
	nzRendererComparison_GreaterOrEqual,
	nzRendererComparison_Less,
	nzRendererComparison_LessOrEqual,
	nzRendererComparison_Never
};

enum nzRendererParameter
{
	nzRendererParameter_Blend,
	nzRendererParameter_ColorWrite,
	nzRendererParameter_DepthTest,
	nzRendererParameter_DepthWrite,
	nzRendererParameter_FaceCulling,
	nzRendererParameter_Stencil
};

enum nzShaderLanguage
{
	nzShaderLanguage_Unknown,

	nzShaderLanguage_Cg,
	nzShaderLanguage_GLSL
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
	nzStencilOperation_Zero
};

enum nzTextureFilter
{
	nzTextureFilter_Bilinear,
	nzTextureFilter_Nearest,
	nzTextureFilter_Trilinear,
	nzTextureFilter_Unknown
};

enum nzTextureWrap
{
	nzTextureWrap_Clamp,
	nzTextureWrap_Repeat,
	nzTextureWrap_Unknown
};

#endif // NAZARA_ENUMS_RENDERER_HPP
