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
	class RenderResources;
	class Texture;

	class NAZARA_GRAPHICS_API RenderTarget
	{
		public:
			inline RenderTarget(Int32 renderOrder = 0);
			virtual ~RenderTarget();

			virtual std::size_t OnBuildGraph(FrameGraph& frameGraph, std::size_t attachmentIndex) const = 0;

			inline Int32 GetRenderOrder() const;
			virtual const Vector2ui& GetSize() const = 0;

			inline void UpdateRenderOrder(Int32 renderOrder);

			NazaraSignal(OnRenderTargetRelease, const RenderTarget* /*renderTarget*/);
			NazaraSignal(OnRenderTargetRenderOrderChange, const RenderTarget* /*renderTarget*/, Int32 /*newOrder*/);
			NazaraSignal(OnRenderTargetSizeChange, const RenderTarget* /*renderTarget*/, const Vector2ui& /*newSize*/);

		private:
			Int32 m_renderOrder;
	};
}

#include <Nazara/Graphics/RenderTarget.inl>

#endif // NAZARA_GRAPHICS_RENDERTARGET_HPP
