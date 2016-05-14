// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERSTATES_HPP
#define NAZARA_RENDERSTATES_HPP

#include <Nazara/Utility/Enums.hpp>

namespace Nz
{
	struct RenderStates
	{
		RenderStates();
		RenderStates(const RenderStates& states);
		~RenderStates() = default;

		RenderStates& operator=(const RenderStates& states);

		struct Face
		{
			RendererComparison stencilCompare;
			StencilOperation stencilFail;
			StencilOperation stencilPass;
			StencilOperation stencilZFail;
			UInt32 stencilMask;
			unsigned int stencilReference;
		};

		Face backFace;
		Face frontFace;
		BlendFunc dstBlend;
		BlendFunc srcBlend;
		FaceFilling faceFilling;
		FaceSide faceCulling;
		RendererComparison depthFunc;
		bool parameters[RendererParameter_Max+1];
		float lineWidth;
		float pointSize;
	};
}

#include <Nazara/Renderer/RenderStates.inl>

#endif // NAZARA_RENDERSTATES_HPP
