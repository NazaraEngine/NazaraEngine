// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_BUFFER_HPP
#define NAZARA_VULKANRENDERER_BUFFER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Utility/AbstractBuffer.hpp>
#include <Nazara/VulkanRenderer/Config.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Buffer.hpp>
#include <Nazara/VulkanRenderer/Wrapper/DeviceMemory.hpp>
#include <vector>

namespace Nz
{
	class Buffer;

	class NAZARA_VULKANRENDERER_API VulkanBuffer : public AbstractBuffer
	{
		public:
			inline VulkanBuffer(const Vk::DeviceHandle& device, Buffer* parent, BufferType type);
			VulkanBuffer(const VulkanBuffer&) = delete;
			VulkanBuffer(VulkanBuffer&&) = delete; ///TODO
			virtual ~VulkanBuffer();

			bool Fill(const void* data, UInt32 offset, UInt32 size) override;

			inline Nz::Vk::Buffer& GetBufferHandle();
			bool Initialize(UInt32 size, BufferUsageFlags usage) override;

			DataStorage GetStorage() const override;

			void* Map(BufferAccess access, UInt32 offset = 0, UInt32 size = 0) override;
			bool Unmap() override;

			VulkanBuffer& operator=(const VulkanBuffer&) = delete;
			VulkanBuffer& operator=(VulkanBuffer&&) = delete; ///TODO

		private:
			Buffer* m_parent;
			BufferType m_type;
			Nz::Vk::Buffer m_buffer;
			Nz::Vk::DeviceHandle m_device;
			Nz::Vk::DeviceMemory m_memory;
	};
}

#include <Nazara/VulkanRenderer/VulkanBuffer.inl>

#endif // NAZARA_VULKANRENDERER_BUFFER_HPP
