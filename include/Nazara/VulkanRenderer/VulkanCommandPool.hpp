// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VULKANCOMMANDPOOL_HPP
#define NAZARA_VULKANRENDERER_VULKANCOMMANDPOOL_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/CommandPool.hpp>
#include <Nazara/VulkanRenderer/Config.hpp>
#include <Nazara/VulkanRenderer/Wrapper/CommandPool.hpp>

namespace Nz
{
	class NAZARA_VULKANRENDERER_API VulkanCommandPool final : public CommandPool
	{
		public:
			inline VulkanCommandPool(Vk::Device& device, QueueType queueType);
			inline VulkanCommandPool(Vk::Device& device, UInt32 queueFamilyIndex);
			VulkanCommandPool(const VulkanCommandPool&) = delete;
			VulkanCommandPool(VulkanCommandPool&&) noexcept = default;
			~VulkanCommandPool() = default;

			std::unique_ptr<CommandBuffer> BuildCommandBuffer(const std::function<void(CommandBufferBuilder& builder)>& callback) override;

			VulkanCommandPool& operator=(const VulkanCommandPool&) = delete;
			VulkanCommandPool& operator=(VulkanCommandPool&&) = delete;

		private:
			Vk::CommandPool m_commandPool;
	};
}

#include <Nazara/VulkanRenderer/VulkanCommandPool.inl>

#endif // NAZARA_VULKANRENDERER_VULKANCOMMANDPOOL_HPP
