// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VULKANSHADERBINDING_HPP
#define NAZARA_VULKANRENDERER_VULKANSHADERBINDING_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/ShaderBinding.hpp>
#include <Nazara/VulkanRenderer/Wrapper/DescriptorSet.hpp>

namespace Nz
{
	class VulkanRenderPipelineLayout;

	class NAZARA_VULKANRENDERER_API VulkanShaderBinding : public ShaderBinding
	{
		public:
			inline VulkanShaderBinding(VulkanRenderPipelineLayout& owner, Vk::DescriptorSet descriptorSet);
			VulkanShaderBinding(const VulkanShaderBinding&) = default;
			VulkanShaderBinding(VulkanShaderBinding&&) noexcept = default;
			~VulkanShaderBinding() = default;

			inline Vk::DescriptorSet& GetDescriptorSet();

			void Update(std::initializer_list<Binding> bindings) override;

			VulkanShaderBinding& operator=(const VulkanShaderBinding&) = delete;
			VulkanShaderBinding& operator=(VulkanShaderBinding&&) = delete;

		private:
			Vk::AutoDescriptorSet m_descriptorSet;
			VulkanRenderPipelineLayout& m_owner;
	};
}

#include <Nazara/VulkanRenderer/VulkanShaderBinding.inl>

#endif // NAZARA_VULKANRENDERER_VULKANSHADERBINDING_HPP
