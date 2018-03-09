// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_SURFACE_HPP
#define NAZARA_VULKANRENDERER_SURFACE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/RenderSurface.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Surface.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Swapchain.hpp>

namespace Nz
{
	class NAZARA_VULKANRENDERER_API VulkanSurface : public RenderSurface
	{
		public:
			VulkanSurface();
			VulkanSurface(const VulkanSurface&) = delete;
			VulkanSurface(VulkanSurface&&) = delete; ///TODO
			virtual ~VulkanSurface();

			bool Create(WindowHandle handle) override;
			void Destroy() override;

			inline Vk::Surface& GetSurface();

			VulkanSurface& operator=(const VulkanSurface&) = delete;
			VulkanSurface& operator=(VulkanSurface&&) = delete; ///TODO

		private:
			Vk::Surface m_surface;
	};
}

#include <Nazara/VulkanRenderer/VulkanSurface.inl>

#endif // NAZARA_VULKANRENDERER_SURFACE_HPP
