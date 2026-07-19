// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VULKANDEVICE_HPP
#define NAZARA_VULKANRENDERER_VULKANDEVICE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Renderer/GpuDevice.hpp>
#include <Nazara/VulkanRenderer/VulkanAsyncCommands.hpp>
#include <Nazara/VulkanRenderer/VulkanBuffer.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Device.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Fence.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_VULKANRENDERER_API VulkanDevice : public GpuDevice, public Vk::Device
	{
		public:
			inline VulkanDevice(Vk::Instance& instance, const GpuDeviceFeatures& enabledFeatures, GpuDeviceInfo renderDeviceInfo);
			VulkanDevice(const VulkanDevice&) = delete;
			VulkanDevice(VulkanDevice&&) = delete; ///TODO?
			~VulkanDevice();

			void Execute(const FunctionRef<void(GpuCommandBufferBuilder& builder)>& callback, QueueType queueType) override;

			const GpuDeviceInfo& GetDeviceInfo() const override;
			const GpuDeviceFeatures& GetEnabledFeatures() const override;

			std::unique_ptr<GpuAsyncCommands> InstantiateAsyncCommands(QueueType queueType) override;
			std::shared_ptr<GpuBuffer> InstantiateBuffer(UInt64 size, BufferUsageFlags usageFlags, const void* initialData = nullptr) override;
			std::shared_ptr<GpuCommandPool> InstantiateCommandPool(QueueType queueType) override;
			std::shared_ptr<GpuComputePipeline> InstantiateComputePipeline(GpuComputePipelineInfo pipelineInfo) override;
			std::shared_ptr<Framebuffer> InstantiateFramebuffer(UInt32 width, UInt32 height, const std::shared_ptr<GpuRenderPass>& renderPass, const std::vector<std::shared_ptr<Texture>>& attachments) override;
			std::shared_ptr<GpuRenderPass> InstantiateRenderPass(std::vector<GpuRenderPass::Attachment> attachments, std::vector<GpuRenderPass::SubpassDescription> subpassDescriptions, std::vector<GpuRenderPass::SubpassDependency> subpassDependencies) override;
			std::shared_ptr<GpuRenderPipeline> InstantiateRenderPipeline(RenderPipelineInfo pipelineInfo) override;
			std::shared_ptr<GpuPipelineLayout> InstantiateRenderPipelineLayout(GpuPipelineLayoutInfo pipelineLayoutInfo) override;
			std::shared_ptr<ShaderModule> InstantiateShaderModule(nzsl::ShaderStageTypeFlags stages, const nzsl::Ast::Module& shaderModule, const nzsl::BackendParameters& states) override;
			std::shared_ptr<ShaderModule> InstantiateShaderModule(nzsl::ShaderStageTypeFlags stages, ShaderLanguage lang, const void* source, std::size_t sourceSize, const nzsl::BackendParameters& states) override;
			std::shared_ptr<Swapchain> InstantiateSwapchain(WindowHandle windowHandle, const Vector2ui& windowSize, const SwapchainParameters& parameters) override;
			std::shared_ptr<Texture> InstantiateTexture(const TextureInfo& params) override;
			std::shared_ptr<TextureSampler> InstantiateTextureSampler(const TextureSamplerInfo& params) override;

			bool IsTextureFormatSupported(PixelFormat format, TextureUsage usage) const override;

			void SubmitAsyncCommands(std::unique_ptr<GpuAsyncCommands>&& transfer, bool waitForCompletion) override;
			void SubmitAsyncCommandsAndWait(VulkanAsyncCommands& transfer);

			void UpdateAsyncTransfer();

			void WaitForIdle() override;

			VulkanDevice& operator=(const VulkanDevice&) = delete;
			VulkanDevice& operator=(VulkanDevice&&) = delete; ///TODO?

		private:
			struct ActiveAsyncTransfer
			{
				std::unique_ptr<VulkanAsyncCommands> asyncTransfer;
				Vk::Fence completionFence;
			};

			std::vector<ActiveAsyncTransfer> m_activeAsyncTransfer;
			GpuDeviceFeatures m_enabledFeatures;
			GpuDeviceInfo m_renderDeviceInfo;
	};
}

#include <Nazara/VulkanRenderer/VulkanDevice.inl>

#endif // NAZARA_VULKANRENDERER_VULKANDEVICE_HPP
