// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_RENDERER_RENDERDEVICE_HPP
#define NAZARA_RENDERER_RENDERDEVICE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/PixelFormat.hpp>
#include <Nazara/Renderer/GpuAsyncCommands.hpp>
#include <Nazara/Renderer/GpuComputePipeline.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Renderer/Export.hpp>
#include <Nazara/Renderer/Framebuffer.hpp>
#include <Nazara/Renderer/GpuBuffer.hpp>
#include <Nazara/Renderer/GpuDeviceInfo.hpp>
#include <Nazara/Renderer/GpuRenderPass.hpp>
#include <Nazara/Renderer/GpuRenderPipeline.hpp>
#include <Nazara/Renderer/GpuPipelineLayout.hpp>
#include <Nazara/Renderer/Swapchain.hpp>
#include <Nazara/Renderer/SwapchainParameters.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Renderer/TextureSampler.hpp>
#include <NazaraUtils/FunctionRef.hpp>
#include <NazaraUtils/Signal.hpp>
#include <NZSL/BackendParameters.hpp>
#include <NZSL/Ast/Module.hpp>
#include <memory>
#include <string>

namespace Nz
{
	class GpuCommandBufferBuilder;
	class GpuCommandPool;
	class ShaderModule;
	struct WindowHandle;

	class NAZARA_RENDERER_API GpuDevice
	{
		public:
			GpuDevice() = default;
			virtual ~GpuDevice();

			virtual void Execute(const FunctionRef<void(GpuCommandBufferBuilder& builder)>& callback, QueueType queueType) = 0;

			virtual const GpuDeviceInfo& GetDeviceInfo() const = 0;
			virtual const GpuDeviceFeatures& GetEnabledFeatures() const = 0;

			virtual std::unique_ptr<GpuAsyncCommands> InstantiateAsyncCommands(QueueType queueType) = 0;
			virtual std::shared_ptr<GpuBuffer> InstantiateBuffer(UInt64 size, BufferUsageFlags usageFlags, const void* initialData = nullptr) = 0;
			virtual std::shared_ptr<GpuCommandPool> InstantiateCommandPool(QueueType queueType) = 0;
			virtual std::shared_ptr<GpuComputePipeline> InstantiateComputePipeline(GpuComputePipelineInfo pipelineInfo) = 0;
			virtual std::shared_ptr<Framebuffer> InstantiateFramebuffer(UInt32 width, UInt32 height, const std::shared_ptr<GpuRenderPass>& renderPass, const std::vector<std::shared_ptr<Texture>>& attachments) = 0;
			virtual std::shared_ptr<GpuRenderPass> InstantiateRenderPass(std::vector<GpuRenderPass::Attachment> attachments, std::vector<GpuRenderPass::SubpassDescription> subpassDescriptions, std::vector<GpuRenderPass::SubpassDependency> subpassDependencies) = 0;
			virtual std::shared_ptr<GpuRenderPipeline> InstantiateRenderPipeline(RenderPipelineInfo pipelineInfo) = 0;
			virtual std::shared_ptr<GpuPipelineLayout> InstantiateRenderPipelineLayout(GpuPipelineLayoutInfo pipelineLayoutInfo) = 0;
			virtual std::shared_ptr<ShaderModule> InstantiateShaderModule(nzsl::ShaderStageTypeFlags shaderStages, const nzsl::Ast::Module& shaderModule, const nzsl::BackendParameters& states) = 0;
			virtual std::shared_ptr<ShaderModule> InstantiateShaderModule(nzsl::ShaderStageTypeFlags shaderStages, ShaderLanguage lang, const void* source, std::size_t sourceSize, const nzsl::BackendParameters& states) = 0;
			std::shared_ptr<ShaderModule> InstantiateShaderModule(nzsl::ShaderStageTypeFlags shaderStages, ShaderLanguage lang, const std::filesystem::path& sourcePath, const nzsl::BackendParameters& states);
			virtual std::shared_ptr<Swapchain> InstantiateSwapchain(WindowHandle windowHandle, const Vector2ui& windowSize, const SwapchainParameters& parameters) = 0;
			virtual std::shared_ptr<Texture> InstantiateTexture(const TextureInfo& params) = 0;
			virtual std::shared_ptr<TextureSampler> InstantiateTextureSampler(const TextureSamplerInfo& params) = 0;

			virtual bool IsTextureFormatSupported(PixelFormat format, TextureUsage usage) const = 0;

			virtual void SubmitAsyncCommands(std::unique_ptr<GpuAsyncCommands>&& transfer, bool waitForCompletion = false) = 0;

			virtual void WaitForIdle() = 0;

			static void ValidateFeatures(const GpuDeviceFeatures& supportedFeatures, GpuDeviceFeatures& enabledFeatures);

			NazaraSignal(OnRenderDeviceRelease, GpuDevice* /*device*/);
	};
}

#include <Nazara/Renderer/GpuDevice.inl>

#endif // NAZARA_RENDERER_RENDERDEVICE_HPP
