// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERER_RENDERDEVICE_HPP
#define NAZARA_RENDERER_RENDERDEVICE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Renderer/Framebuffer.hpp>
#include <Nazara/Renderer/RenderBuffer.hpp>
#include <Nazara/Renderer/RenderDeviceInfo.hpp>
#include <Nazara/Renderer/RenderPass.hpp>
#include <Nazara/Renderer/RenderPipeline.hpp>
#include <Nazara/Renderer/RenderPipelineLayout.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Renderer/TextureSampler.hpp>
#include <Nazara/Shader/ShaderWriter.hpp>
#include <Nazara/Shader/Ast/Module.hpp>
#include <Nazara/Utility/PixelFormat.hpp>
#include <memory>
#include <string>

namespace Nz
{
	class CommandPool;
	class ShaderModule;

	class NAZARA_RENDERER_API RenderDevice
	{
		public:
			RenderDevice() = default;
			virtual ~RenderDevice();

			virtual const RenderDeviceInfo& GetDeviceInfo() const = 0;
			virtual const RenderDeviceFeatures& GetEnabledFeatures() const = 0;

			virtual std::shared_ptr<RenderBuffer> InstantiateBuffer(BufferType type, UInt64 size, BufferUsageFlags usageFlags, const void* initialData = nullptr) = 0;
			virtual std::shared_ptr<CommandPool> InstantiateCommandPool(QueueType queueType) = 0;
			virtual std::shared_ptr<Framebuffer> InstantiateFramebuffer(unsigned int width, unsigned int height, const std::shared_ptr<RenderPass>& renderPass, const std::vector<std::shared_ptr<Texture>>& attachments) = 0;
			virtual std::shared_ptr<RenderPass> InstantiateRenderPass(std::vector<RenderPass::Attachment> attachments, std::vector<RenderPass::SubpassDescription> subpassDescriptions, std::vector<RenderPass::SubpassDependency> subpassDependencies) = 0;
			virtual std::shared_ptr<RenderPipeline> InstantiateRenderPipeline(RenderPipelineInfo pipelineInfo) = 0;
			virtual std::shared_ptr<RenderPipelineLayout> InstantiateRenderPipelineLayout(RenderPipelineLayoutInfo pipelineLayoutInfo) = 0;
			virtual std::shared_ptr<ShaderModule> InstantiateShaderModule(ShaderStageTypeFlags shaderStages, const ShaderAst::Module& shaderModule, const ShaderWriter::States& states) = 0;
			virtual std::shared_ptr<ShaderModule> InstantiateShaderModule(ShaderStageTypeFlags shaderStages, ShaderLanguage lang, const void* source, std::size_t sourceSize, const ShaderWriter::States& states) = 0;
			std::shared_ptr<ShaderModule> InstantiateShaderModule(ShaderStageTypeFlags shaderStages, ShaderLanguage lang, const std::filesystem::path& sourcePath, const ShaderWriter::States& states);
			virtual std::shared_ptr<Texture> InstantiateTexture(const TextureInfo& params) = 0;
			virtual std::shared_ptr<TextureSampler> InstantiateTextureSampler(const TextureSamplerInfo& params) = 0;

			virtual bool IsTextureFormatSupported(PixelFormat format, TextureUsage usage) const = 0;

			static void ValidateFeatures(const RenderDeviceFeatures& supportedFeatures, RenderDeviceFeatures& enabledFeatures);
	};
}

#include <Nazara/Renderer/RenderDevice.inl>

#endif // NAZARA_RENDERER_RENDERDEVICE_HPP
