// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
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
			enum class Type
			{
				Multiple,
				Single
			};

			inline VulkanFramebuffer(Type type);
			VulkanFramebuffer(const VulkanFramebuffer&) = delete;
			VulkanFramebuffer(VulkanFramebuffer&&) noexcept = default;
			~VulkanFramebuffer() = default;

			inline Type GetType() const;

			VulkanFramebuffer& operator=(const VulkanFramebuffer&) = delete;
			VulkanFramebuffer& operator=(VulkanFramebuffer&&) noexcept = default;

		private:
			Type m_type;
	};
}

#include <Nazara/VulkanRenderer/VulkanFramebuffer.inl>

#endif // NAZARA_VULKANRENDERER_VULKANFRAMEBUFFER_HPP
