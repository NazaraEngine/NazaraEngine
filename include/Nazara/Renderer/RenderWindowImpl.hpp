// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERWINDOWIMPL_HPP
#define NAZARA_RENDERWINDOWIMPL_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Platform/WindowHandle.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/RenderDevice.hpp>
#include <Nazara/Renderer/RenderWindowParameters.hpp>

namespace Nz
{
	class CommandPool;
	class RendererImpl;
	class RenderImage;
	class RenderSurface;

	class NAZARA_RENDERER_API RenderWindowImpl
	{
		public:
			RenderWindowImpl() = default;
			virtual ~RenderWindowImpl();

			virtual RenderImage& Acquire() = 0;

			virtual bool Create(RendererImpl* renderer, RenderSurface* surface, const Vector2ui& size, const RenderWindowParameters& parameters) = 0;
			virtual std::unique_ptr<CommandPool> CreateCommandPool(QueueType queueType) = 0;

			virtual std::shared_ptr<RenderDevice> GetRenderDevice() = 0;
	};
}

#endif // NAZARA_RENDERWINDOWIMPL_HPP
