// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VULKANRENDERPASS_HPP
#define NAZARA_VULKANRENDERER_VULKANRENDERPASS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Renderer/RenderPass.hpp>
#include <Nazara/VulkanRenderer/Export.hpp>
#include <Nazara/VulkanRenderer/Wrapper/RenderPass.hpp>
#include <NazaraUtils/Signal.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_VULKANRENDERER_API VulkanRenderPass final : public RenderPass
	{
		public:
			VulkanRenderPass(Vk::Device& device, std::vector<Attachment> attachments, std::vector<SubpassDescription> subpassDescriptions, std::vector<SubpassDependency> subpassDependencies);
			VulkanRenderPass(const VulkanRenderPass&) = delete;
			VulkanRenderPass(VulkanRenderPass&&) noexcept = default;
			~VulkanRenderPass();

			inline Vk::RenderPass& GetRenderPass();
			inline const Vk::RenderPass& GetRenderPass() const;

			void UpdateDebugName(std::string_view name) override;

			VulkanRenderPass& operator=(const VulkanRenderPass&) = delete;
			VulkanRenderPass& operator=(VulkanRenderPass&&) noexcept = default;

			NazaraSignal(OnRenderPassRelease, const VulkanRenderPass* /*renderPass*/);

		private:
			Vk::RenderPass m_renderPass;
	};
}

#include <Nazara/VulkanRenderer/VulkanRenderPass.inl>

#endif // NAZARA_VULKANRENDERER_VULKANRENDERPASS_HPP
