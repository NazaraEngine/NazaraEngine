// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERSTATES_HPP
#define NAZARA_RENDERSTATES_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Utility/Enums.hpp>

namespace Nz
{
	struct RenderStates
	{
		BlendFunc dstBlend = BlendFunc_Zero;
		BlendFunc srcBlend = BlendFunc_One;
		FaceFilling faceFilling = FaceFilling_Fill;
		FaceSide cullingSide = FaceSide_Back;
		RendererComparison depthCompare = RendererComparison_Less;
		PrimitiveMode primitiveMode = PrimitiveMode_TriangleList;

		struct
		{
			RendererComparison compare = RendererComparison_Always;
			StencilOperation depthFail = StencilOperation_Keep;
			StencilOperation fail = StencilOperation_Keep;
			StencilOperation pass = StencilOperation_Keep;
			UInt32 compareMask = 0xFFFFFFFF;
			UInt32 reference = 0;
			UInt32 writeMask = 0xFFFFFFFF;
		} stencilBack, stencilFront;

		bool blending    = false;
		bool colorWrite  = true;
		bool depthBuffer = false;
		bool depthWrite  = true;
		bool faceCulling = false;
		bool scissorTest = false;
		bool stencilTest = false;

		float lineWidth = 1.f;
		float pointSize = 1.f;
	};

	inline bool operator==(const RenderStates& lhs, const RenderStates& rhs);
}

#include <Nazara/Renderer/RenderStates.inl>

#endif // NAZARA_RENDERSTATES_HPP
