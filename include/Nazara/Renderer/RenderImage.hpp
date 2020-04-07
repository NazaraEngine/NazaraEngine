// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERIMAGE_HPP
#define NAZARA_RENDERIMAGE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <functional>

namespace Nz
{
	class CommandBuffer;
	class CommandBufferBuilder;
	class UploadPool;

	class NAZARA_RENDERER_API RenderImage
	{
		public:
			RenderImage() = default;
			virtual ~RenderImage();

			virtual void Execute(const std::function<void(CommandBufferBuilder& builder)>& callback, QueueTypeFlags queueTypeFlags) = 0;

			virtual UploadPool& GetUploadPool() = 0;

			virtual void SubmitCommandBuffer(CommandBuffer* commandBuffer, QueueTypeFlags queueTypeFlags) = 0;

			virtual void Present() = 0;

		protected:
			RenderImage(const RenderImage&) = delete;
			RenderImage(RenderImage&&) = default;
	};
}

#include <Nazara/Renderer/RenderImage.inl>

#endif // NAZARA_RENDERIMAGE_HPP
