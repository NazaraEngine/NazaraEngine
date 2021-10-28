// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VULKANSURFACE_HPP
#define NAZARA_VULKANRENDERER_VULKANSURFACE_HPP

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
			~VulkanSurface() = default;

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

#endif // NAZARA_VULKANRENDERER_VULKANSURFACE_HPP
