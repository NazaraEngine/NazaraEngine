// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_RENDERER_RENDERSTATES_HPP
#define NAZARA_RENDERER_RENDERSTATES_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/VertexDeclaration.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <memory>
#include <vector>

namespace Nz
{
	class ShaderModule;

	struct RenderStates
	{
		ColorComponentMask colorWriteMask = ColorComponentAll;
		FaceCulling faceCulling = FaceCulling::Back;
		FaceFilling faceFilling = FaceFilling::Fill;
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
		bool depthBias   = false;
		bool depthBuffer = false;
		bool depthClamp  = false;
		bool depthWrite  = true;
		bool scissorTest = false;
		bool stencilTest = false;

		float depthBiasConstantFactor = 0.f;
		float depthBiasSlopeFactor = 0.f;
		float lineWidth = 1.f;
		float pointSize = 1.f;
	};

	inline bool operator==(const RenderStates& lhs, const RenderStates& rhs);
}

#include <Nazara/Renderer/RenderStates.inl>

#endif // NAZARA_RENDERER_RENDERSTATES_HPP
