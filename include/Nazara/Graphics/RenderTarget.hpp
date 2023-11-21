// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_RENDERTARGET_HPP
#define NAZARA_GRAPHICS_RENDERTARGET_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <NazaraUtils/Signal.hpp>

namespace Nz
{
	class BakedFrameGraph;
	class CommandBufferBuilder;
	class Framebuffer;
	class FrameGraph;
	class RenderPass;
	class Texture;
	class RenderResources;

	class NAZARA_GRAPHICS_API RenderTarget
	{
		public:
			RenderTarget() = default;
			virtual ~RenderTarget();

			virtual void OnBuildGraph(FrameGraph& frameGraph, std::size_t attachmentIndex) const = 0;
			virtual void OnRenderEnd(RenderResources& resources, const BakedFrameGraph& frameGraph, std::size_t finalAttachment) const = 0;

			virtual const Vector2ui& GetSize() const = 0;

			NazaraSignal(OnRenderTargetRelease, const RenderTarget* /*renderTarget*/);
			NazaraSignal(OnRenderTargetSizeChange, const RenderTarget* /*renderTarget*/, const Vector2ui& /*newSize*/);
	};
}

#include <Nazara/Graphics/RenderTarget.inl>

#endif // NAZARA_GRAPHICS_RENDERTARGET_HPP
