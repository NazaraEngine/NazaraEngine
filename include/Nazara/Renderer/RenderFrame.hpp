// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERER_RENDERFRAME_HPP
#define NAZARA_RENDERER_RENDERFRAME_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Renderer/RenderImage.hpp>
#include <Nazara/Utils/FunctionRef.hpp>
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
			inline explicit RenderFrame(RenderImage* renderImage, bool framebufferInvalidation, const Vector2ui& size, std::size_t framebufferIndex);
			RenderFrame(const RenderFrame&) = delete;
			RenderFrame(RenderFrame&&) = delete;
			~RenderFrame() = default;

			void Execute(const FunctionRef<void(CommandBufferBuilder& builder)>& callback, QueueTypeFlags queueTypeFlags);

			inline std::size_t GetFramebufferIndex() const;
			const Vector2ui& GetSize() const;
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
			std::size_t m_framebufferIndex;
			RenderImage* m_image;
			Vector2ui m_size;
			bool m_framebufferInvalidation;
	};
}

#include <Nazara/Renderer/RenderFrame.inl>

#endif // NAZARA_RENDERER_RENDERFRAME_HPP
