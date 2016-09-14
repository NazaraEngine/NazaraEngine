// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_HPP
#define NAZARA_VULKANRENDERER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Renderer/RendererImpl.hpp>
#include <Nazara/VulkanRenderer/Config.hpp>
#include <Nazara/VulkanRenderer/VkInstance.hpp>
#include <Nazara/VulkanRenderer/VkPhysicalDevice.hpp>

namespace Nz
{
	class NAZARA_VULKANRENDERER_API VulkanRenderer : public RendererImpl
	{
		public:
			VulkanRenderer() = default;
			~VulkanRenderer() = default;

			std::unique_ptr<RenderWindowImpl> CreateRenderWindowImpl() override;

			bool IsBetterThan(const RendererImpl* other) const override;

			RenderAPI QueryAPI() const override;
			String QueryAPIString() const override;
			UInt32 QueryAPIVersion() const override;
			std::vector<RenderDevice> QueryRenderDevices() const override;

			bool Prepare(const ParameterList& parameters) override;

			static constexpr UInt32 APIVersion = VK_API_VERSION_1_0;

		private:
			Vk::Instance m_instance;
			std::vector<Vk::PhysicalDevice> m_physDevices;
			UInt32 m_apiVersion;
	};
}

#endif // NAZARA_VULKANRENDERER_HPP
