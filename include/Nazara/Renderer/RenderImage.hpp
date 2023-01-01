// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERER_RENDERIMAGE_HPP
#define NAZARA_RENDERER_RENDERIMAGE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Utils/FunctionRef.hpp>
#include <functional>
#include <vector>

namespace Nz
{
	class CommandBuffer;
	class CommandBufferBuilder;
	class UploadPool;

	class NAZARA_RENDERER_API RenderImage
	{
		public:
			class Releasable;
			template<typename T> class ReleasableLambda;

			virtual ~RenderImage();

			virtual void Execute(const FunctionRef<void(CommandBufferBuilder& builder)>& callback, QueueTypeFlags queueTypeFlags) = 0;

			inline void FlushReleaseQueue();

			virtual UploadPool& GetUploadPool() = 0;

			virtual void Present() = 0;

			template<typename F> void PushReleaseCallback(F&& callback);

			virtual void SubmitCommandBuffer(CommandBuffer* commandBuffer, QueueTypeFlags queueTypeFlags) = 0;

		protected:
			RenderImage() = default;
			RenderImage(const RenderImage&) = delete;
			RenderImage(RenderImage&&) = delete;

		private:
			static constexpr std::size_t BlockSize = 4 * 1024 * 1024;

			using Block = std::vector<UInt8>;

			std::vector<Releasable*> m_releaseQueue;
			std::vector<Block> m_releaseMemoryPool;
	};

	class NAZARA_RENDERER_API RenderImage::Releasable
	{
		public:
			virtual ~Releasable();

			virtual void Release() = 0;
	};

	template<typename T>
	class RenderImage::ReleasableLambda : public Releasable
	{
		public:
			template<typename U> ReleasableLambda(U&& lambda);
			ReleasableLambda(const ReleasableLambda&) = delete;
			ReleasableLambda(ReleasableLambda&&) = delete;
			~ReleasableLambda() = default;

			void Release() override;

			ReleasableLambda& operator=(const ReleasableLambda&) = delete;
			ReleasableLambda& operator=(ReleasableLambda&&) = delete;

		private:
			T m_lambda;
	};
}

#include <Nazara/Renderer/RenderImage.inl>

#endif // NAZARA_RENDERER_RENDERIMAGE_HPP
