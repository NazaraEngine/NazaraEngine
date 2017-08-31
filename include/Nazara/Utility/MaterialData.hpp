// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MATERIALDATA_HPP
#define NAZARA_MATERIALDATA_HPP

namespace Nz
{
	struct MaterialData
	{
		static constexpr const char* AlphaTest                = "MatAlphaTest";
		static constexpr const char* AlphaTexturePath         = "MatAlphaTexturePath";
		static constexpr const char* AlphaThreshold           = "MatAlphaThreshold";
		static constexpr const char* AmbientColor             = "MatAmbientColor";
		static constexpr const char* BackFaceStencilCompare   = "MatBackFaceStencilCompare";
		static constexpr const char* BackFaceStencilFail      = "MatBackFaceStencilFail";
		static constexpr const char* BackFaceStencilMask      = "MatBackFaceStencilMask";
		static constexpr const char* BackFaceStencilPass      = "MatBackFaceStencilPass";
		static constexpr const char* BackFaceStencilReference = "MatBackFaceStencilReference";
		static constexpr const char* BackFaceStencilZFail     = "MatBackFaceStencilZFail";
		static constexpr const char* Blending                 = "MatBlending";
		static constexpr const char* CullingSide              = "MatCullingSide";
		static constexpr const char* ColorWrite               = "MatColorWrite";
		static constexpr const char* DepthBuffer              = "MatDepthBuffer";
		static constexpr const char* DepthFunc                = "MatDepthfunc";
		static constexpr const char* DepthSorting             = "MatDepthSorting";
		static constexpr const char* DepthWrite               = "MatDepthWrite";
		static constexpr const char* DiffuseAnisotropyLevel   = "MatDiffuseAnisotropyLevel";
		static constexpr const char* DiffuseColor             = "MatDiffuseColor";
		static constexpr const char* DiffuseFilter            = "MatDiffuseFilter";
		static constexpr const char* DiffuseTexturePath       = "MatDiffuseTexturePath";
		static constexpr const char* DiffuseWrap              = "MatDiffuseWrap";
		static constexpr const char* DstBlend                 = "MatDstBlend";
		static constexpr const char* EmissiveTexturePath      = "MatEmissiveTexturePath";
		static constexpr const char* FaceCulling              = "MatFaceCulling";
		static constexpr const char* FaceFilling              = "MatFaceFilling";
		static constexpr const char* FilePath                 = "MatFilePath";
		static constexpr const char* HeightTexturePath        = "MatHeightTexturePath";
		static constexpr const char* Lighting                 = "MatLighting";
		static constexpr const char* LineWidth                = "MatLineWidth";
		static constexpr const char* Name                     = "MatName";
		static constexpr const char* NormalTexturePath        = "MatNormalTexturePath";
		static constexpr const char* PointSize                = "MatPointSize";
		static constexpr const char* ScissorTest              = "MatScissorTest";
		static constexpr const char* Shininess                = "MatShininess";
		static constexpr const char* SpecularAnisotropyLevel  = "MatSpecularAnisotropyLevel";
		static constexpr const char* SpecularColor            = "MatSpecularColor";
		static constexpr const char* SpecularFilter           = "MatSpecularFilter";
		static constexpr const char* SpecularTexturePath      = "MatSpecularTexturePath";
		static constexpr const char* SpecularWrap             = "MatSpecularWrap";
		static constexpr const char* SrcBlend                 = "MatSrcBlend";
		static constexpr const char* StencilCompare           = "MatStencilCompare";
		static constexpr const char* StencilFail              = "MatStencilFail";
		static constexpr const char* StencilMask              = "MatStencilMask";
		static constexpr const char* StencilPass              = "MatStencilPass";
		static constexpr const char* StencilReference         = "MatStencilReference";
		static constexpr const char* StencilTest              = "MatStencilTest";
		static constexpr const char* StencilZFail             = "MatStencilZFail";
		static constexpr const char* Transform                = "MatTransform";
	};
}

#endif // NAZARA_MATERIALDATA_HPP
