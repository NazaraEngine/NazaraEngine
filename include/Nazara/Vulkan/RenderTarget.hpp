// Copyright (C) 201 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERTARGET_HPP
#define NAZARA_RENDERTARGET_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Vulkan/Config.hpp>
#include <Nazara/Vulkan/VkFrameBuffer.hpp>
#include <Nazara/Vulkan/VkRenderPass.hpp>
#include <unordered_map>
#include <vulkan/vulkan.h>

namespace Nz
{
	class Renderer;

	class NAZARA_VULKAN_API RenderTarget
	{
		friend Renderer;

		public:
			RenderTarget() = default;
			RenderTarget(const RenderTarget&) = delete;
			RenderTarget(RenderTarget&&) = delete; ///TOOD?
			virtual ~RenderTarget();

			virtual bool Acquire(const Vk::Framebuffer** framebuffer) const = 0;

			const Vk::RenderPass& GetRenderPass() const { return m_renderPass; }

			virtual void Present() = 0;

			RenderTarget& operator=(const RenderTarget&) = delete;
			RenderTarget& operator=(RenderTarget&&) = delete; ///TOOD?

			// Signals:
			NazaraSignal(OnRenderTargetRelease,	const RenderTarget* /*renderTarget*/);
			NazaraSignal(OnRenderTargetSizeChange, const RenderTarget* /*renderTarget*/);

		protected:
			Vk::RenderPass m_renderPass;
	};
}

#endif // NAZARA_RENDERTARGET_HPP
