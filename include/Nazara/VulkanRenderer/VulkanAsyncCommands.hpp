// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VULKANASYNCCOMMANDS_HPP
#define NAZARA_VULKANRENDERER_VULKANASYNCCOMMANDS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Renderer/AsyncRenderCommands.hpp>
#include <Nazara/VulkanRenderer/Export.hpp>
#include <Nazara/VulkanRenderer/Wrapper/CommandBuffer.hpp>

namespace Nz
{
	class VulkanDevice;

	class NAZARA_VULKANRENDERER_API VulkanAsyncCommands : public AsyncRenderCommands
	{
		friend class VulkanDevice;

		public:
			VulkanAsyncCommands(VulkanDevice& device, QueueType queueType);
			VulkanAsyncCommands(const VulkanAsyncCommands&) = delete;
			VulkanAsyncCommands(VulkanAsyncCommands&&) = delete; ///TODO
			~VulkanAsyncCommands() = default;

			void AddCommands(Nz::FunctionRef<void(CommandBufferBuilder& builder)> callback) override;

			inline QueueType GetQueueType() const;

			VulkanAsyncCommands& operator=(const VulkanAsyncCommands&) = delete;
			VulkanAsyncCommands& operator=(VulkanAsyncCommands&&) = delete; ///TODO

		private:
			using AsyncRenderCommands::TriggerCallbacks;

			VkCommandBuffer PrepareForSubmit();

			Vk::AutoCommandBuffer m_commandBuffer;
			QueueType m_queueType;
	};
}

#include <Nazara/VulkanRenderer/VulkanAsyncCommands.inl>

#endif // NAZARA_VULKANRENDERER_VULKANASYNCCOMMANDS_HPP
