// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VULKANRENDERPIPELINELAYOUT_HPP
#define NAZARA_VULKANRENDERER_VULKANRENDERPIPELINELAYOUT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Renderer/RenderPipelineLayout.hpp>
#include <Nazara/VulkanRenderer/Export.hpp>
#include <Nazara/VulkanRenderer/VulkanShaderBinding.hpp>
#include <Nazara/VulkanRenderer/Wrapper/DescriptorPool.hpp>
#include <Nazara/VulkanRenderer/Wrapper/DescriptorSet.hpp>
#include <Nazara/VulkanRenderer/Wrapper/DescriptorSetLayout.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Device.hpp>
#include <Nazara/VulkanRenderer/Wrapper/PipelineLayout.hpp>
#include <NazaraUtils/Bitset.hpp>
#include <memory>
#include <type_traits>
#include <vector>

namespace Nz
{
	class NAZARA_VULKANRENDERER_API VulkanRenderPipelineLayout : public RenderPipelineLayout
	{
		friend VulkanShaderBinding;

		public:
			VulkanRenderPipelineLayout() = default;
			~VulkanRenderPipelineLayout();

			ShaderBindingPtr AllocateShaderBinding(UInt32 setIndex) override;

			bool Create(Vk::Device& device, RenderPipelineLayoutInfo layoutInfo);

			inline Vk::Device* GetDevice() const;

			inline const Vk::PipelineLayout& GetPipelineLayout() const;

			void UpdateDebugName(std::string_view name) override;

		private:
			struct DescriptorPool;

			DescriptorPool& AllocatePool();
			ShaderBindingPtr AllocateFromPool(std::size_t poolIndex, UInt32 setIndex);
			void Release(ShaderBinding& binding);
			inline void TryToShrink();

			struct DescriptorPool
			{
				using BindingStorage = std::aligned_storage_t<sizeof(VulkanShaderBinding), alignof(VulkanShaderBinding)>;

				Bitset<UInt64> freeBindings;
				std::unique_ptr<Vk::DescriptorPool> descriptorPool;
				std::unique_ptr<BindingStorage[]> storage;
			};

			MovablePtr<Vk::Device> m_device;
			std::vector<DescriptorPool> m_descriptorPools;
			std::vector<const Vk::DescriptorSetLayout*> m_descriptorSetLayouts;
			Vk::PipelineLayout m_pipelineLayout;
			RenderPipelineLayoutInfo m_layoutInfo;
	};
}

#include <Nazara/VulkanRenderer/VulkanRenderPipelineLayout.inl>

#endif // NAZARA_VULKANRENDERER_VULKANRENDERPIPELINELAYOUT_HPP
