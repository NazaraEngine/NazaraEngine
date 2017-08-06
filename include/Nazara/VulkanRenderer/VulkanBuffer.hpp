// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_BUFFER_HPP
#define NAZARA_VULKANRENDERER_BUFFER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Utility/AbstractBuffer.hpp>
#include <Nazara/Utility/SoftwareBuffer.hpp>
#include <Nazara/VulkanRenderer/Config.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Buffer.hpp>
#include <vector>

namespace Nz
{
	//TODO: Move all the software stuff to the Renderer

	class NAZARA_VULKANRENDERER_API VulkanBuffer : public AbstractBuffer
	{
		public:
			inline VulkanBuffer(Buffer* parent, BufferType type);
			VulkanBuffer(const VulkanBuffer&) = delete;
			VulkanBuffer(VulkanBuffer&&) = delete; ///TODO
			virtual ~VulkanBuffer();

			bool Fill(const void* data, UInt32 offset, UInt32 size) override;

			bool Initialize(UInt32 size, BufferUsageFlags usage) override;

			DataStorage GetStorage() const override;

			void* Map(BufferAccess access, UInt32 offset = 0, UInt32 size = 0) override;
			bool Unmap() override;

			VulkanBuffer& operator=(const VulkanBuffer&) = delete;
			VulkanBuffer& operator=(VulkanBuffer&&) = delete; ///TODO

		private:
			BufferUsageFlags m_usage;
			SoftwareBuffer m_softwareData;
	};
}

#include <Nazara/VulkanRenderer/VulkanBuffer.inl>

#endif // NAZARA_VULKANRENDERER_BUFFER_HPP
