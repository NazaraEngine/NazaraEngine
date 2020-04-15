// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLDEVICE_HPP
#define NAZARA_OPENGLRENDERER_OPENGLDEVICE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/RenderDevice.hpp>
#include <Nazara/OpenGLRenderer/OpenGLBuffer.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Device.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_OPENGLRENDERER_API OpenGLDevice : public RenderDevice, public Vk::Device
	{
		public:
			using Device::Device;
			OpenGLDevice(const OpenGLDevice&) = delete;
			OpenGLDevice(OpenGLDevice&&) = delete; ///TODO?
			~OpenGLDevice();

			std::unique_ptr<AbstractBuffer> InstantiateBuffer(BufferType type) override;
			std::unique_ptr<CommandPool> InstantiateCommandPool(QueueType queueType) override;
			std::unique_ptr<RenderPipeline> InstantiateRenderPipeline(RenderPipelineInfo pipelineInfo) override;
			std::shared_ptr<RenderPipelineLayout> InstantiateRenderPipelineLayout(RenderPipelineLayoutInfo pipelineLayoutInfo) override;
			std::shared_ptr<ShaderStageImpl> InstantiateShaderStage(ShaderStageType type, ShaderLanguage lang, const void* source, std::size_t sourceSize) override;
			std::unique_ptr<Texture> InstantiateTexture(const TextureInfo& params) override;
			std::unique_ptr<TextureSampler> InstantiateTextureSampler(const TextureSamplerInfo& params) override;

			OpenGLDevice& operator=(const OpenGLDevice&) = delete;
			OpenGLDevice& operator=(OpenGLDevice&&) = delete; ///TODO?
	};
}

#include <Nazara/OpenGLRenderer/OpenGLDevice.inl>

#endif // NAZARA_OPENGLRENDERER_OPENGLDEVICE_HPP
