// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VULKANFRAMEBUFFER_HPP
#define NAZARA_VULKANRENDERER_VULKANFRAMEBUFFER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/Framebuffer.hpp>
#include <Nazara/VulkanRenderer/Config.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Framebuffer.hpp>

namespace Nz
{
	class NAZARA_VULKANRENDERER_API VulkanFramebuffer : public Framebuffer
	{
		public:
			using Framebuffer::Framebuffer;

			virtual const Vk::Framebuffer& GetFramebuffer() const = 0;
	};
}

#include <Nazara/VulkanRenderer/VulkanFramebuffer.inl>

#endif // NAZARA_VULKANRENDERER_VULKANFRAMEBUFFER_HPP
