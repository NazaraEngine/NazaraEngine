// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLDEVICE_HPP
#define NAZARA_OPENGLRENDERER_OPENGLDEVICE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Platform/WindowHandle.hpp>
#include <Nazara/OpenGLRenderer/Config.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Context.hpp>
#include <Nazara/Renderer/RenderDevice.hpp>
#include <unordered_set>
#include <vector>

namespace Nz
{
	class NAZARA_OPENGLRENDERER_API OpenGLDevice : public RenderDevice
	{
		friend GL::Context;

		public:
			OpenGLDevice(GL::Loader& loader);
			OpenGLDevice(const OpenGLDevice&) = delete;
			OpenGLDevice(OpenGLDevice&&) = delete; ///TODO?
			~OpenGLDevice();

			std::unique_ptr<GL::Context> CreateContext(const GL::ContextParams& params) const;
			std::unique_ptr<GL::Context> CreateContext(const GL::ContextParams& params, WindowHandle handle) const;

			inline const GL::Context& GetReferenceContext() const;

			std::unique_ptr<AbstractBuffer> InstantiateBuffer(BufferType type) override;
			std::unique_ptr<CommandPool> InstantiateCommandPool(QueueType queueType) override;
			std::unique_ptr<RenderPipeline> InstantiateRenderPipeline(RenderPipelineInfo pipelineInfo) override;
			std::shared_ptr<RenderPipelineLayout> InstantiateRenderPipelineLayout(RenderPipelineLayoutInfo pipelineLayoutInfo) override;
			std::shared_ptr<ShaderStageImpl> InstantiateShaderStage(ShaderStageType type, ShaderLanguage lang, const void* source, std::size_t sourceSize) override;
			std::unique_ptr<Texture> InstantiateTexture(const TextureInfo& params) override;
			std::unique_ptr<TextureSampler> InstantiateTextureSampler(const TextureSamplerInfo& params) override;

			inline void NotifyBufferDestruction(GLuint buffer) const;
			inline void NotifySamplerDestruction(GLuint sampler) const;
			inline void NotifyTextureDestruction(GLuint texture) const;

			OpenGLDevice& operator=(const OpenGLDevice&) = delete;
			OpenGLDevice& operator=(OpenGLDevice&&) = delete; ///TODO?

		private:
			inline void NotifyContextDestruction(const GL::Context& context) const;

			std::unique_ptr<GL::Context> m_referenceContext;
			mutable std::unordered_set<const GL::Context*> m_contexts;
			GL::Loader& m_loader;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLDevice.inl>

#endif // NAZARA_OPENGLRENDERER_OPENGLDEVICE_HPP
