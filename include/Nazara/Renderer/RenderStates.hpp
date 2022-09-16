// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERER_RENDERSTATES_HPP
#define NAZARA_RENDERER_RENDERSTATES_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <Nazara/Utility/VertexDeclaration.hpp>
#include <memory>
#include <vector>

namespace Nz
{
	class ShaderModule;

	struct RenderStates
	{
		ColorComponentMask colorWriteMask = ColorComponentAll;
		FaceFilling faceFilling = FaceFilling::Fill;
		FaceSide cullingSide = FaceSide::Back;
		FrontFace frontFace = FrontFace::CounterClockwise;
		RendererComparison depthCompare = RendererComparison::LessOrEqual;
		PrimitiveMode primitiveMode = PrimitiveMode::TriangleList;

		struct 
		{
			BlendEquation modeAlpha = BlendEquation::Add;
			BlendEquation modeColor = BlendEquation::Add;
			BlendFunc dstAlpha = BlendFunc::Zero;
			BlendFunc dstColor = BlendFunc::Zero;
			BlendFunc srcAlpha = BlendFunc::One;
			BlendFunc srcColor = BlendFunc::One;
		} blend;

		struct
		{
			RendererComparison compare = RendererComparison::Always;
			StencilOperation depthFail = StencilOperation::Keep;
			StencilOperation fail = StencilOperation::Keep;
			StencilOperation pass = StencilOperation::Keep;
			UInt32 compareMask = 0xFFFFFFFF;
			UInt32 reference = 0;
			UInt32 writeMask = 0xFFFFFFFF;
		} stencilBack, stencilFront;

		bool blending    = false;
		bool depthBuffer = false;
		bool depthClamp  = false;
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

#endif // NAZARA_RENDERER_RENDERSTATES_HPP
