// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_RENDERER_RENDERDEVICE_HPP
#define NAZARA_RENDERER_RENDERDEVICE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/PixelFormat.hpp>
#include <Nazara/Renderer/ComputePipeline.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Renderer/Export.hpp>
#include <Nazara/Renderer/Framebuffer.hpp>
#include <Nazara/Renderer/RenderBuffer.hpp>
#include <Nazara/Renderer/RenderDeviceInfo.hpp>
#include <Nazara/Renderer/RenderPass.hpp>
#include <Nazara/Renderer/RenderPipeline.hpp>
#include <Nazara/Renderer/RenderPipelineLayout.hpp>
#include <Nazara/Renderer/Swapchain.hpp>
#include <Nazara/Renderer/SwapchainParameters.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Renderer/TextureSampler.hpp>
#include <NazaraUtils/FunctionRef.hpp>
#include <NZSL/ShaderWriter.hpp>
#include <NZSL/Ast/Module.hpp>
#include <memory>
#include <string>

namespace Nz
{
	class CommandBufferBuilder;
	class CommandPool;
	class ShaderModule;
	struct WindowHandle;

	class NAZARA_RENDERER_API RenderDevice
	{
		public:
			RenderDevice() = default;
			virtual ~RenderDevice();

			virtual const RenderDeviceInfo& GetDeviceInfo() const = 0;
			virtual const RenderDeviceFeatures& GetEnabledFeatures() const = 0;

			virtual std::shared_ptr<RenderBuffer> InstantiateBuffer(BufferType type, UInt64 size, BufferUsageFlags usageFlags, const void* initialData = nullptr) = 0;
			virtual std::shared_ptr<CommandPool> InstantiateCommandPool(QueueType queueType) = 0;
			virtual std::shared_ptr<ComputePipeline> InstantiateComputePipeline(ComputePipelineInfo pipelineInfo) = 0;
			virtual std::shared_ptr<Framebuffer> InstantiateFramebuffer(unsigned int width, unsigned int height, const std::shared_ptr<RenderPass>& renderPass, const std::vector<std::shared_ptr<Texture>>& attachments) = 0;
			virtual std::shared_ptr<RenderPass> InstantiateRenderPass(std::vector<RenderPass::Attachment> attachments, std::vector<RenderPass::SubpassDescription> subpassDescriptions, std::vector<RenderPass::SubpassDependency> subpassDependencies) = 0;
			virtual std::shared_ptr<RenderPipeline> InstantiateRenderPipeline(RenderPipelineInfo pipelineInfo) = 0;
			virtual std::shared_ptr<RenderPipelineLayout> InstantiateRenderPipelineLayout(RenderPipelineLayoutInfo pipelineLayoutInfo) = 0;
			virtual std::shared_ptr<ShaderModule> InstantiateShaderModule(nzsl::ShaderStageTypeFlags shaderStages, const nzsl::Ast::Module& shaderModule, const nzsl::ShaderWriter::States& states) = 0;
			virtual std::shared_ptr<ShaderModule> InstantiateShaderModule(nzsl::ShaderStageTypeFlags shaderStages, ShaderLanguage lang, const void* source, std::size_t sourceSize, const nzsl::ShaderWriter::States& states) = 0;
			std::shared_ptr<ShaderModule> InstantiateShaderModule(nzsl::ShaderStageTypeFlags shaderStages, ShaderLanguage lang, const std::filesystem::path& sourcePath, const nzsl::ShaderWriter::States& states);
			virtual std::shared_ptr<Swapchain> InstantiateSwapchain(WindowHandle windowHandle, const Vector2ui& windowSize, const SwapchainParameters& parameters) = 0;
			virtual std::shared_ptr<Texture> InstantiateTexture(const TextureInfo& params) = 0;
			virtual std::shared_ptr<Texture> InstantiateTexture(const TextureInfo& params, const void* initialData, bool buildMipmaps, unsigned int srcWidth = 0, unsigned int srcHeight = 0) = 0;
			virtual std::shared_ptr<TextureSampler> InstantiateTextureSampler(const TextureSamplerInfo& params) = 0;

			virtual bool IsTextureFormatSupported(PixelFormat format, TextureUsage usage) const = 0;

			virtual void WaitForIdle() = 0;

			static void ValidateFeatures(const RenderDeviceFeatures& supportedFeatures, RenderDeviceFeatures& enabledFeatures);
	};
}

#include <Nazara/Renderer/RenderDevice.inl>

#endif // NAZARA_RENDERER_RENDERDEVICE_HPP
