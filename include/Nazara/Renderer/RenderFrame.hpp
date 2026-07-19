// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_RENDERER_RENDERFRAME_HPP
#define NAZARA_RENDERER_RENDERFRAME_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Renderer/Export.hpp>
#include <Nazara/Renderer/RenderImage.hpp>
#include <NazaraUtils/FunctionRef.hpp>
#include <functional>

namespace Nz
{
	class GpuCommandBuffer;
	class GpuCommandBufferBuilder;
	class GpuDevice;
	class GpuUploadPool;

	class NAZARA_RENDERER_API RenderFrame
	{
		public:
			inline explicit RenderFrame();
			inline explicit RenderFrame(RenderImage* renderImage, bool framebufferInvalidation, const Vector2ui& size);
			RenderFrame(const RenderFrame&) = delete;
			RenderFrame(RenderFrame&&) = delete;
			~RenderFrame() = default;

			inline void Execute(const FunctionRef<void(GpuCommandBufferBuilder& builder)>& callback, QueueTypeFlags queueTypeFlags);

			inline std::size_t GetImageIndex() const;
			const Vector2ui& GetSize() const;
			inline GpuDevice& GetRenderDevice();
			inline GpuResources& GetTransientResources();
			inline GpuUploadPool& GetUploadPool();

			inline bool IsFramebufferInvalidated() const;

			template<typename T> void PushForRelease(T&& value);
			template<typename F> void PushReleaseCallback(F&& releaseCallback);

			void Present();

			void SubmitCommandBuffer(GpuCommandBuffer* commandBuffer, QueueTypeFlags queueTypeFlags) ;

			inline explicit operator bool();
			inline operator GpuResources&();

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
