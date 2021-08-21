// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VULKANRENDERPASS_HPP
#define NAZARA_VULKANRENDERER_VULKANRENDERPASS_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Renderer/RenderPass.hpp>
#include <Nazara/VulkanRenderer/Config.hpp>
#include <Nazara/VulkanRenderer/Wrapper/RenderPass.hpp>
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

			VulkanRenderPass& operator=(const VulkanRenderPass&) = delete;
			VulkanRenderPass& operator=(VulkanRenderPass&&) noexcept = default;

			NazaraSignal(OnRenderPassRelease, const VulkanRenderPass* /*renderPass*/);

		private:
			Vk::RenderPass m_renderPass;
	};
}

#include <Nazara/VulkanRenderer/VulkanRenderPass.inl>

#endif // NAZARA_VULKANRENDERER_VULKANRENDERPASS_HPP
