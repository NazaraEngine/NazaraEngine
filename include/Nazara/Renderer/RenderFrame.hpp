// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERFRAME_HPP
#define NAZARA_RENDERFRAME_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Renderer/RenderImage.hpp>
#include <functional>

namespace Nz
{
	class CommandBuffer;
	class CommandBufferBuilder;
	class UploadPool;

	class NAZARA_RENDERER_API RenderFrame
	{
		public:
			inline explicit RenderFrame();
			inline explicit RenderFrame(RenderImage* renderImage, bool framebufferInvalidation);
			RenderFrame(const RenderFrame&) = delete;
			RenderFrame(RenderFrame&&) = delete;
			~RenderFrame() = default;

			void Execute(const std::function<void(CommandBufferBuilder& builder)>& callback, QueueTypeFlags queueTypeFlags);

			UploadPool& GetUploadPool();

			inline bool IsFramebufferInvalidated() const;

			template<typename T> void PushForRelease(T&& value);
			template<typename F> void PushReleaseCallback(F&& releaseCallback);

			void Present();

			void SubmitCommandBuffer(CommandBuffer* commandBuffer, QueueTypeFlags queueTypeFlags) ;

			inline explicit operator bool();

			RenderFrame& operator=(const RenderFrame&) = delete;
			RenderFrame& operator=(RenderFrame&&) = delete;

		private:
			RenderImage* m_image;
			bool m_framebufferInvalidation;
	};
}

#include <Nazara/Renderer/RenderFrame.inl>

#endif
