// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VULKANRENDERPASS_HPP
#define NAZARA_VULKANRENDERER_VULKANRENDERPASS_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/RenderPass.hpp>
#include <Nazara/VulkanRenderer/Config.hpp>
#include <Nazara/VulkanRenderer/Wrapper/RenderPass.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_VULKANRENDERER_API VulkanRenderPass final : public RenderPass
	{
		public:
			inline VulkanRenderPass(Vk::RenderPass renderPass, std::initializer_list<PixelFormat> formats); //< FIXME
			VulkanRenderPass(const VulkanRenderPass&) = delete;
			VulkanRenderPass(VulkanRenderPass&&) noexcept = default;
			~VulkanRenderPass() = default;

			inline PixelFormat GetAttachmentFormat(std::size_t attachmentIndex) const;
			inline Vk::RenderPass& GetRenderPass();
			inline const Vk::RenderPass& GetRenderPass() const;

			VulkanRenderPass& operator=(const VulkanRenderPass&) = delete;
			VulkanRenderPass& operator=(VulkanRenderPass&&) noexcept = default;

		private:
			std::vector<PixelFormat> m_formats;
			Vk::RenderPass m_renderPass;
	};
}

#include <Nazara/VulkanRenderer/VulkanRenderPass.inl>

#endif // NAZARA_VULKANRENDERER_VULKANRENDERPASS_HPP
