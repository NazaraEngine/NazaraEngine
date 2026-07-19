// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLDEVICE_HPP
#define NAZARA_OPENGLRENDERER_OPENGLDEVICE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/Export.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Context.hpp>
#include <Nazara/Platform/WindowHandle.hpp>
#include <Nazara/Renderer/GpuDevice.hpp>
#include <Nazara/Renderer/GpuDeviceInfo.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <unordered_set>
#include <vector>

namespace Nz
{
	class OpenGLAsyncCommands;

	class NAZARA_OPENGLRENDERER_API OpenGLDevice : public GpuDevice
	{
		friend GL::Context;

		public:
			OpenGLDevice(GL::Loader& loader, const Renderer::Config& config);
			OpenGLDevice(const OpenGLDevice&) = delete;
			OpenGLDevice(OpenGLDevice&&) = delete; ///TODO?
			~OpenGLDevice();

			std::shared_ptr<GL::Context> CreateContext(GL::ContextParams params) const;
			std::shared_ptr<GL::Context> CreateContext(GL::ContextParams params, WindowHandle handle) const;

			void Execute(const FunctionRef<void(GpuCommandBufferBuilder& builder)>& callback, QueueType queueType) override;

			const GpuDeviceInfo& GetDeviceInfo() const override;
			const GpuDeviceFeatures& GetEnabledFeatures() const override;
			inline const GL::Context& GetReferenceContext() const;

			std::unique_ptr<GpuAsyncCommands> InstantiateAsyncCommands(QueueType queueType) override;
			std::shared_ptr<GpuBuffer> InstantiateBuffer(UInt64 size, BufferUsageFlags usageFlags, const void* initialData = nullptr) override;
			std::shared_ptr<GpuCommandPool> InstantiateCommandPool(QueueType queueType) override;
			std::shared_ptr<GpuComputePipeline> InstantiateComputePipeline(GpuComputePipelineInfo pipelineInfo) override;
			std::shared_ptr<GpuFramebuffer> InstantiateFramebuffer(UInt32 width, UInt32 height, const std::shared_ptr<GpuRenderPass>& renderPass, const std::vector<std::shared_ptr<Texture>>& attachments) override;
			std::shared_ptr<GpuRenderPass> InstantiateRenderPass(std::vector<GpuRenderPass::Attachment> attachments, std::vector<GpuRenderPass::SubpassDescription> subpassDescriptions, std::vector<GpuRenderPass::SubpassDependency> subpassDependencies) override;
			std::shared_ptr<GpuRenderPipeline> InstantiateRenderPipeline(RenderPipelineInfo pipelineInfo) override;
			std::shared_ptr<GpuPipelineLayout> InstantiateRenderPipelineLayout(GpuPipelineLayoutInfo pipelineLayoutInfo) override;
			std::shared_ptr<ShaderModule> InstantiateShaderModule(nzsl::ShaderStageTypeFlags shaderStages, const nzsl::Ast::Module& shaderModule, const nzsl::BackendParameters& states) override;
			std::shared_ptr<ShaderModule> InstantiateShaderModule(nzsl::ShaderStageTypeFlags shaderStages, ShaderLanguage lang, const void* source, std::size_t sourceSize, const nzsl::BackendParameters& states) override;
			std::shared_ptr<Swapchain> InstantiateSwapchain(WindowHandle windowHandle, const Vector2ui& windowSize, const SwapchainParameters& parameters) override;
			std::shared_ptr<Texture> InstantiateTexture(const TextureInfo& params) override;
			std::shared_ptr<TextureSampler> InstantiateTextureSampler(const TextureSamplerInfo& params) override;

			bool IsTextureFormatSupported(PixelFormat format, TextureUsage usage) const override;

			inline void NotifyBufferDestruction(GLuint buffer) const;
			inline void NotifyProgramDestruction(GLuint program) const;
			inline void NotifySamplerDestruction(GLuint sampler) const;
			inline void NotifyTextureDestruction(GLuint texture) const;

			void SubmitAsyncCommands(std::unique_ptr<GpuAsyncCommands>&& transfer, bool waitForCompletion = false) override;

			void UpdateAsyncTransfer();

			void WaitForIdle() override;

			OpenGLDevice& operator=(const OpenGLDevice&) = delete;
			OpenGLDevice& operator=(OpenGLDevice&&) = delete; ///TODO?

		private:
			inline void NotifyContextDestruction(const GL::Context& context) const;

			struct ActiveAsyncTransfer
			{
				std::unique_ptr<OpenGLAsyncCommands> asyncTransfer;
				GLsync completionFence;
			};

			std::shared_ptr<GL::Context> m_referenceContext;
			std::vector<ActiveAsyncTransfer> m_activeAsyncTransfer;
			mutable std::unordered_set<const GL::Context*> m_contexts;
			GpuDeviceInfo m_deviceInfo;
			GL::Loader& m_loader;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLDevice.inl>

#endif // NAZARA_OPENGLRENDERER_OPENGLDEVICE_HPP
