// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_HPP
#define NAZARA_VULKANRENDERER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/RendererImpl.hpp>
#include <Nazara/VulkanRenderer/Config.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Device.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Instance.hpp>
#include <Nazara/VulkanRenderer/Wrapper/PhysicalDevice.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Surface.hpp>
#include <list>
#include <vector>

namespace Nz
{
	class NAZARA_VULKANRENDERER_API VulkanRenderer : public RendererImpl
	{
		public:
			VulkanRenderer() = default;
			~VulkanRenderer();

			std::unique_ptr<RenderSurface> CreateRenderSurfaceImpl() override;
			std::unique_ptr<RenderWindowImpl> CreateRenderWindowImpl() override;

			std::shared_ptr<RenderDevice> InstanciateRenderDevice(std::size_t deviceIndex) override;

			bool IsBetterThan(const RendererImpl* other) const override;

			RenderAPI QueryAPI() const override;
			String QueryAPIString() const override;
			UInt32 QueryAPIVersion() const override;
			std::vector<RenderDeviceInfo> QueryRenderDevices() const override;

			bool Prepare(const ParameterList& parameters) override;

			static constexpr UInt32 APIVersion = VK_API_VERSION_1_0;

		private:
			std::list<Vk::Device> m_devices;
			std::vector<Vk::PhysicalDevice> m_physDevices;
			ParameterList m_initializationParameters;
			Vk::Instance m_instance;
			UInt32 m_apiVersion;
	};
}

#include <Nazara/VulkanRenderer/VulkanRenderer.inl>

#endif // NAZARA_VULKANRENDERER_HPP
