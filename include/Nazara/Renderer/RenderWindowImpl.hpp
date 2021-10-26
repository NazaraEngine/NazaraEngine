// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
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
#include <Nazara/Renderer/RenderFrame.hpp>
#include <Nazara/Renderer/RenderPass.hpp>
#include <Nazara/Renderer/RenderTarget.hpp>
#include <Nazara/Renderer/RenderWindowParameters.hpp>
#include <vector>

namespace Nz
{
	class CommandPool;
	class RendererImpl;
	class RenderDevice;
	class RenderSurface;

	class NAZARA_RENDERER_API RenderWindowImpl : public RenderTarget
	{
		public:
			RenderWindowImpl() = default;
			virtual ~RenderWindowImpl();

			virtual RenderFrame Acquire() = 0;

			virtual bool Create(RendererImpl* renderer, RenderSurface* surface, const RenderWindowParameters& parameters) = 0;
			virtual std::shared_ptr<CommandPool> CreateCommandPool(QueueType queueType) = 0;

		protected:
			static void BuildRenderPass(PixelFormat colorFormat, PixelFormat depthFormat, std::vector<RenderPass::Attachment>& attachments, std::vector<RenderPass::SubpassDescription>& subpassDescriptions, std::vector<RenderPass::SubpassDependency>& subpassDependencies);
	};
}

#endif // NAZARA_RENDERWINDOWIMPL_HPP
