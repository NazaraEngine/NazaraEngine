// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERER_RENDERFRAME_HPP
#define NAZARA_RENDERER_RENDERFRAME_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Renderer/RenderImage.hpp>
#include <NazaraUtils/FunctionRef.hpp>
#include <functional>

namespace Nz
{
	class CommandBuffer;
	class CommandBufferBuilder;
	class RenderDevice;
	class UploadPool;

	class NAZARA_RENDERER_API RenderFrame
	{
		public:
			inline explicit RenderFrame();
			inline explicit RenderFrame(RenderImage* renderImage, bool framebufferInvalidation, const Vector2ui& size);
			RenderFrame(const RenderFrame&) = delete;
			RenderFrame(RenderFrame&&) = delete;
			~RenderFrame() = default;

			inline void Execute(const FunctionRef<void(CommandBufferBuilder& builder)>& callback, QueueTypeFlags queueTypeFlags);

			inline std::size_t GetImageIndex() const;
			const Vector2ui& GetSize() const;
			inline RenderDevice& GetRenderDevice();
			inline RenderResources& GetTransientResources();
			inline UploadPool& GetUploadPool();

			inline bool IsFramebufferInvalidated() const;

			template<typename T> void PushForRelease(T&& value);
			template<typename F> void PushReleaseCallback(F&& releaseCallback);

			void Present();

			void SubmitCommandBuffer(CommandBuffer* commandBuffer, QueueTypeFlags queueTypeFlags) ;

			inline explicit operator bool();
			inline operator RenderResources&();

			RenderFrame& operator=(const RenderFrame&) = delete;
			RenderFrame& operator=(RenderFrame&&) = delete;

		private:
			RenderImage* m_image;
			Vector2ui m_size;
			bool m_framebufferInvalidation;
	};
}

#include <Nazara/Renderer/RenderFrame.inl>

#endif // NAZARA_RENDERER_RENDERFRAME_HPP
