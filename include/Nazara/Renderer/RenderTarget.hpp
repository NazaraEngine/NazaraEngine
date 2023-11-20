// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERER_RENDERTARGET_HPP
#define NAZARA_RENDERER_RENDERTARGET_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <NazaraUtils/Signal.hpp>

namespace Nz
{
	class CommandBufferBuilder;
	class Framebuffer;
	class RenderFrame;
	class RenderPass;
	class Texture;

	class NAZARA_RENDERER_API RenderTarget
	{
		public:
			RenderTarget() = default;
			virtual ~RenderTarget();

			virtual void BlitTexture(RenderFrame& renderFrame, CommandBufferBuilder& builder, const Texture& texture) const = 0;

			virtual const Vector2ui& GetSize() const = 0;

			NazaraSignal(OnRenderTargetRelease, const RenderTarget* /*renderTarget*/);
			NazaraSignal(OnRenderTargetSizeChange, const RenderTarget* /*renderTarget*/, const Vector2ui& /*newSize*/);
	};
}

#include <Nazara/Renderer/RenderTarget.inl>

#endif // NAZARA_RENDERER_RENDERTARGET_HPP
