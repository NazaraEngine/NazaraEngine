// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UTILITY_MATERIALDATA_HPP
#define NAZARA_UTILITY_MATERIALDATA_HPP

namespace Nz
{
	struct MaterialData
	{
		static constexpr const char* AlphaTest                = "MatAlphaTest";
		static constexpr const char* AlphaTexturePath         = "MatAlphaTexturePath";
		static constexpr const char* AlphaWrap                = "MatAlphaWrap";
		static constexpr const char* AlphaThreshold           = "MatAlphaThreshold";
		static constexpr const char* AmbientColor             = "MatAmbientColor";
		static constexpr const char* BackFaceStencilCompare   = "MatBackFaceStencilCompare";
		static constexpr const char* BackFaceStencilFail      = "MatBackFaceStencilFail";
		static constexpr const char* BackFaceStencilMask      = "MatBackFaceStencilMask";
		static constexpr const char* BackFaceStencilPass      = "MatBackFaceStencilPass";
		static constexpr const char* BackFaceStencilReference = "MatBackFaceStencilReference";
		static constexpr const char* BackFaceStencilZFail     = "MatBackFaceStencilZFail";
		static constexpr const char* BaseColor                = "MatBaseColor";
		static constexpr const char* BaseColorTexturePath     = "MatBaseColorTexturePath";
		static constexpr const char* BaseColorWrap            = "MatBaseColorWrap";
		static constexpr const char* Blending                 = "MatBlending";
		static constexpr const char* BlendModeAlpha           = "MatBlendModeAlpha";
		static constexpr const char* BlendModeColor           = "MatBlendModeColor";
		static constexpr const char* BlendDstAlpha            = "MatBlendDstAlpha";
		static constexpr const char* BlendDstColor            = "MatBlendDstColor";
		static constexpr const char* BlendSrcAlpha            = "MatBlendSrcAlpha";
		static constexpr const char* BlendSrcColor            = "MatBlendSrcColor";
		static constexpr const char* CullingSide              = "MatCullingSide";
		static constexpr const char* ColorWrite               = "MatColorWrite";
		static constexpr const char* DepthBuffer              = "MatDepthBuffer";
		static constexpr const char* DepthFunc                = "MatDepthfunc";
		static constexpr const char* DepthSorting             = "MatDepthSorting";
		static constexpr const char* DepthWrite               = "MatDepthWrite";
		static constexpr const char* DiffuseAnisotropyLevel   = "MatDiffuseAnisotropyLevel";
		static constexpr const char* DiffuseFilter            = "MatDiffuseFilter";
		static constexpr const char* EmissiveTexturePath      = "MatEmissiveTexturePath";
		static constexpr const char* EmissiveWrap             = "MatEmissiveWrap";
		static constexpr const char* FaceCulling              = "MatFaceCulling";
		static constexpr const char* FaceFilling              = "MatFaceFilling";
		static constexpr const char* FilePath                 = "MatFilePath";
		static constexpr const char* HeightTexturePath        = "MatHeightTexturePath";
		static constexpr const char* HeightWrap               = "MatHeightWrap";
		static constexpr const char* Lighting                 = "MatLighting";
		static constexpr const char* LineWidth                = "MatLineWidth";
		static constexpr const char* MetallicTexturePath      = "MatMetallicTexturePath";
		static constexpr const char* MetallicWrap             = "MatMetallicWrap";
		static constexpr const char* Name                     = "MatName";
		static constexpr const char* NormalTexturePath        = "MatNormalTexturePath";
		static constexpr const char* NormalWrap               = "MatNormalTextureWrap";
		static constexpr const char* PointSize                = "MatPointSize";
		static constexpr const char* RoughnessTexturePath     = "MatRoughnessTexturePath";
		static constexpr const char* RoughnessWrap            = "MatRoughnessWrap";
		static constexpr const char* ScissorTest              = "MatScissorTest";
		static constexpr const char* Shininess                = "MatShininess";
		static constexpr const char* SpecularAnisotropyLevel  = "MatSpecularAnisotropyLevel";
		static constexpr const char* SpecularColor            = "MatSpecularColor";
		static constexpr const char* SpecularFilter           = "MatSpecularFilter";
		static constexpr const char* SpecularTexturePath      = "MatSpecularTexturePath";
		static constexpr const char* SpecularWrap             = "MatSpecularWrap";
		static constexpr const char* StencilCompare           = "MatStencilCompare";
		static constexpr const char* StencilFail              = "MatStencilFail";
		static constexpr const char* StencilMask              = "MatStencilMask";
		static constexpr const char* StencilPass              = "MatStencilPass";
		static constexpr const char* StencilReference         = "MatStencilReference";
		static constexpr const char* StencilTest              = "MatStencilTest";
		static constexpr const char* StencilZFail             = "MatStencilZFail";
		static constexpr const char* Transform                = "MatTransform";
		static constexpr const char* VertexColor              = "MatVertexColor";
	};
}

#endif // NAZARA_UTILITY_MATERIALDATA_HPP
