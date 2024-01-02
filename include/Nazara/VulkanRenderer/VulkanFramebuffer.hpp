// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VULKANFRAMEBUFFER_HPP
#define NAZARA_VULKANRENDERER_VULKANFRAMEBUFFER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Renderer/Framebuffer.hpp>
#include <Nazara/VulkanRenderer/Config.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Framebuffer.hpp>

namespace Nz
{
	class NAZARA_VULKANRENDERER_API VulkanFramebuffer : public Framebuffer
	{
		public:
			using Framebuffer::Framebuffer;

			virtual Vk::Framebuffer& GetFramebuffer() = 0;
			virtual const Vk::Framebuffer& GetFramebuffer() const = 0;

			void UpdateDebugName(std::string_view name) override;
	};
}

#include <Nazara/VulkanRenderer/VulkanFramebuffer.inl>

#endif // NAZARA_VULKANRENDERER_VULKANFRAMEBUFFER_HPP
