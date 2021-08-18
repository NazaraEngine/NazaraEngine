// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VULKANCOMMANDBUFFER_HPP
#define NAZARA_VULKANRENDERER_VULKANCOMMANDBUFFER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/CommandBuffer.hpp>
#include <Nazara/VulkanRenderer/Config.hpp>
#include <Nazara/VulkanRenderer/Wrapper/CommandBuffer.hpp>
#include <vector>

namespace Nz
{
	class VulkanCommandPool;

	class NAZARA_VULKANRENDERER_API VulkanCommandBuffer final : public CommandBuffer
	{
		public:
			inline VulkanCommandBuffer(VulkanCommandPool& owner, std::size_t poolIndex, std::size_t bindingIndex, Vk::AutoCommandBuffer commandBuffer);
			VulkanCommandBuffer(const VulkanCommandBuffer&) = delete;
			VulkanCommandBuffer(VulkanCommandBuffer&&) = delete;
			~VulkanCommandBuffer() = default;

			inline std::size_t GetBindingIndex() const;
			inline const Vk::CommandBuffer& GetCommandBuffer() const;
			inline std::size_t GetPoolIndex() const;
			inline const VulkanCommandPool& GetOwner() const;

			VulkanCommandBuffer& operator=(const VulkanCommandBuffer&) = delete;
			VulkanCommandBuffer& operator=(VulkanCommandBuffer&&) = delete;

		private:
			inline VulkanCommandBuffer(VulkanCommandPool& owner, std::size_t poolIndex, std::size_t bindingIndex);

			void Release() override;

			std::size_t m_bindingIndex;
			std::size_t m_poolIndex;
			Vk::AutoCommandBuffer m_commandBuffer;
			VulkanCommandPool& m_owner;
	};
}

#include <Nazara/VulkanRenderer/VulkanCommandBuffer.inl>

#endif // NAZARA_VULKANRENDERER_VULKANCOMMANDBUFFER_HPP
