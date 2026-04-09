// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/VulkanRenderer/VulkanDevice.hpp>
#include <Nazara/Platform/WindowHandle.hpp>
#include <Nazara/VulkanRenderer/VulkanAsyncCommands.hpp>
#include <Nazara/VulkanRenderer/VulkanCommandBufferBuilder.hpp>
#include <Nazara/VulkanRenderer/VulkanCommandPool.hpp>
#include <Nazara/VulkanRenderer/VulkanComputePipeline.hpp>
#include <Nazara/VulkanRenderer/VulkanRenderPass.hpp>
#include <Nazara/VulkanRenderer/VulkanRenderPipeline.hpp>
#include <Nazara/VulkanRenderer/VulkanRenderPipelineLayout.hpp>
#include <Nazara/VulkanRenderer/VulkanShaderModule.hpp>
#include <Nazara/VulkanRenderer/VulkanSwapchain.hpp>
#include <Nazara/VulkanRenderer/VulkanTexture.hpp>
#include <Nazara/VulkanRenderer/VulkanTextureFramebuffer.hpp>
#include <Nazara/VulkanRenderer/VulkanTextureSampler.hpp>
#include <Nazara/VulkanRenderer/Wrapper/QueueHandle.hpp>
#include <NZSL/Ast/Cloner.hpp>

namespace Nz
{
	VulkanDevice::~VulkanDevice()
	{
		OnRenderDeviceRelease(this);
	}

	void VulkanDevice::Execute(const FunctionRef<void(CommandBufferBuilder& builder)>& callback, QueueType queueType)
	{
		VulkanAsyncCommands asyncCommands(*this, queueType);
		asyncCommands.AddCommands(callback);

		SubmitAsyncCommandsAndWait(asyncCommands);
	}

	const RenderDeviceInfo& VulkanDevice::GetDeviceInfo() const
	{
		return m_renderDeviceInfo;
	}

	const RenderDeviceFeatures& VulkanDevice::GetEnabledFeatures() const
	{
		return m_enabledFeatures;
	}

	std::unique_ptr<AsyncRenderCommands> VulkanDevice::InstantiateAsyncCommands(QueueType queueType)
	{
		return std::make_unique<VulkanAsyncCommands>(*this, queueType);
	}

	std::shared_ptr<RenderBuffer> VulkanDevice::InstantiateBuffer(UInt64 size, BufferUsageFlags usageFlags, const void* initialData)
	{
		return std::make_shared<VulkanBuffer>(*this, size, usageFlags, initialData);
	}

	std::shared_ptr<CommandPool> VulkanDevice::InstantiateCommandPool(QueueType queueType)
	{
		return std::make_shared<VulkanCommandPool>(*this, queueType);
	}

	std::shared_ptr<ComputePipeline> VulkanDevice::InstantiateComputePipeline(ComputePipelineInfo pipelineInfo)
	{
		return std::make_shared<VulkanComputePipeline>(*this, std::move(pipelineInfo));
	}

	std::shared_ptr<Framebuffer> VulkanDevice::InstantiateFramebuffer(UInt32 width, UInt32 height, const std::shared_ptr<RenderPass>& renderPass, const std::vector<std::shared_ptr<Texture>>& attachments)
	{
		return std::make_shared<VulkanTextureFramebuffer>(*this, width, height, renderPass, attachments);
	}

	std::shared_ptr<RenderPass> VulkanDevice::InstantiateRenderPass(std::vector<RenderPass::Attachment> attachments, std::vector<RenderPass::SubpassDescription> subpassDescriptions, std::vector<RenderPass::SubpassDependency> subpassDependencies)
	{
		return std::make_shared<VulkanRenderPass>(*this, std::move(attachments), std::move(subpassDescriptions), std::move(subpassDependencies));
	}

	std::shared_ptr<RenderPipeline> VulkanDevice::InstantiateRenderPipeline(RenderPipelineInfo pipelineInfo)
	{
		return std::make_shared<VulkanRenderPipeline>(*this, std::move(pipelineInfo));
	}

	std::shared_ptr<RenderPipelineLayout> VulkanDevice::InstantiateRenderPipelineLayout(RenderPipelineLayoutInfo pipelineLayoutInfo)
	{
		auto pipelineLayout = std::make_shared<VulkanRenderPipelineLayout>();
		if (!pipelineLayout->Create(*this, std::move(pipelineLayoutInfo)))
			throw std::runtime_error("failed to instantiate vulkan render pipeline layout");

		return pipelineLayout;
	}

	std::shared_ptr<ShaderModule> VulkanDevice::InstantiateShaderModule(nzsl::ShaderStageTypeFlags stages, const nzsl::Ast::Module& shaderModule, const nzsl::BackendParameters& states)
	{
		auto stage = std::make_shared<VulkanShaderModule>();
		if (!stage->Create(*this, stages, std::move(*nzsl::Ast::Clone(shaderModule)), states))
			throw std::runtime_error("failed to instantiate vulkan shader module");

		return stage;
	}

	std::shared_ptr<ShaderModule> VulkanDevice::InstantiateShaderModule(nzsl::ShaderStageTypeFlags stages, ShaderLanguage lang, const void* source, std::size_t sourceSize, const nzsl::BackendParameters& states)
	{
		auto stage = std::make_shared<VulkanShaderModule>();
		if (!stage->Create(*this, stages, lang, source, sourceSize, states))
			throw std::runtime_error("failed to instantiate vulkan shader module");

		return stage;
	}

	std::shared_ptr<Swapchain> VulkanDevice::InstantiateSwapchain(WindowHandle windowHandle, const Vector2ui& windowSize, const SwapchainParameters& parameters)
	{
		return std::make_shared<VulkanSwapchain>(*this, windowHandle, windowSize, parameters);
	}

	std::shared_ptr<Texture> VulkanDevice::InstantiateTexture(const TextureInfo& params)
	{
		return std::make_shared<VulkanTexture>(*this, params);
	}

	std::shared_ptr<TextureSampler> VulkanDevice::InstantiateTextureSampler(const TextureSamplerInfo& params)
	{
		return std::make_shared<VulkanTextureSampler>(*this, params);
	}

	bool VulkanDevice::IsTextureFormatSupported(PixelFormat format, TextureUsage usage) const
	{
		VkFormat vulkanFormat = ToVulkan(format);
		if (vulkanFormat == VK_FORMAT_UNDEFINED)
			return false;

		VkFormatFeatureFlags flags = 0;
		switch (usage)
		{
			case TextureUsage::ColorAttachment:
				flags = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
				break;

			case TextureUsage::DepthStencilAttachment:
				flags = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
				break;

			case TextureUsage::InputAttachment:
			case TextureUsage::ShaderSampling:
				flags = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
				break;

			case TextureUsage::ShaderReadWrite:
				flags = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
				break;

			case TextureUsage::TransferSource:
				flags = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
				break;

			case TextureUsage::TransferDestination:
				flags = VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
				break;
		}

		VkFormatProperties formatProperties = GetInstance().GetPhysicalDeviceFormatProperties(GetPhysicalDevice(), vulkanFormat);
		return formatProperties.optimalTilingFeatures & flags; //< Assume optimal tiling
	}

	void VulkanDevice::SubmitAsyncCommands(std::unique_ptr<AsyncRenderCommands>&& transfer, bool waitForCompletion)
	{
		std::unique_ptr<VulkanAsyncCommands> asyncTransfer = StaticUniquePointerCast<VulkanAsyncCommands>(std::move(transfer));

		Vk::Fence completionFence;
		completionFence.Create(*this);

		Vk::QueueHandle queue = GetQueue(GetDefaultFamilyIndex(asyncTransfer->GetQueueType()), 0);
		queue.Submit(asyncTransfer->PrepareForSubmit(), completionFence); //< TODO: Handle error

		if (waitForCompletion)
		{
			completionFence.Wait();
			asyncTransfer->TriggerCallbacks();
		}
		else
		{
			ActiveAsyncTransfer& activeTransfer = m_activeAsyncTransfer.emplace_back();
			activeTransfer.asyncTransfer = std::move(asyncTransfer);
			activeTransfer.completionFence = std::move(completionFence);
		}
	}

	void VulkanDevice::SubmitAsyncCommandsAndWait(VulkanAsyncCommands& transfer)
	{
		Vk::QueueHandle queue = GetQueue(GetDefaultFamilyIndex(transfer.GetQueueType()), 0);

		Vk::Fence completionFence;
		completionFence.Create(*this);

		queue.Submit(transfer.PrepareForSubmit(), completionFence); //< TODO: Handle error

		completionFence.Wait();
		transfer.TriggerCallbacks();
	}

	void VulkanDevice::UpdateAsyncTransfer()
	{
		for (auto it = m_activeAsyncTransfer.begin(); it != m_activeAsyncTransfer.end();)
		{
			ActiveAsyncTransfer& transfer = *it;
			if (!transfer.completionFence.GetStatus())
			{
				++it;
				continue;
			}

			transfer.asyncTransfer->TriggerCallbacks();

			it = m_activeAsyncTransfer.erase(it);
		}
	}

	void VulkanDevice::WaitForIdle()
	{
		Device::WaitForIdle();
	}
}
