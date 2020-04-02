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

namespace Nz
{
	class NAZARA_VULKANRENDERER_API VulkanCommandBuffer final : public CommandBuffer
	{
		public:
			inline VulkanCommandBuffer(Vk::AutoCommandBuffer commandBuffer);
			VulkanCommandBuffer(const VulkanCommandBuffer&) = delete;
			VulkanCommandBuffer(VulkanCommandBuffer&&) noexcept = default;
			~VulkanCommandBuffer() = default;

			inline Vk::CommandBuffer& GetCommandBuffer();

			VulkanCommandBuffer& operator=(const VulkanCommandBuffer&) = delete;
			VulkanCommandBuffer& operator=(VulkanCommandBuffer&&) = delete;

		private:
			Vk::AutoCommandBuffer m_commandBuffer;
	};
}

#include <Nazara/VulkanRenderer/VulkanCommandBuffer.inl>

#endif // NAZARA_VULKANRENDERER_VULKANCOMMANDBUFFER_HPP
