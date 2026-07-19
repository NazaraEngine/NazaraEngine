// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_RENDERER_GPURESOURCES_HPP
#define NAZARA_RENDERER_GPURESOURCES_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Renderer/Export.hpp>
#include <NazaraUtils/FunctionRef.hpp>
#include <concepts>
#include <type_traits>
#include <vector>

namespace Nz
{
	class GpuCommandBuffer;
	class GpuCommandBufferBuilder;
	class GpuDevice;
	class GpuUploadPool;

	class NAZARA_RENDERER_API GpuResources
	{
		public:
			class Releasable;
			class ReleasableCallback;
			template<typename T> class ReleasableData;
			template<std::invocable T> class ReleasableFunctor;

			virtual ~GpuResources();

			virtual void Execute(const FunctionRef<void(GpuCommandBufferBuilder& builder)>& callback, QueueTypeFlags queueTypeFlags) = 0;

			inline void FlushReleaseQueue();

			virtual UInt32 GetImageIndex() const = 0;
			inline GpuDevice& GetGpuDevice();
			virtual GpuUploadPool& GetUploadPool() = 0;

			template<typename T> void PushForRelease(const T& value) = delete;
			template<typename T> void PushForRelease(T&& value);
			template<typename F> void PushReleaseCallback(F&& callback);

			virtual void SubmitCommandBuffer(GpuCommandBuffer* commandBuffer, QueueTypeFlags queueTypeFlags) = 0;

		protected:
			inline GpuResources(GpuDevice& renderDvice);
			GpuResources(const GpuResources&) = delete;
			GpuResources(GpuResources&&) = delete;

		private:
			template<typename T> T* Allocate();
			inline void* Allocate(std::size_t size, std::size_t alignment);

			static constexpr std::size_t BlockSize = 4 * 1024 * 1024;

			using Block = std::vector<UInt8>;

			std::vector<ReleasableCallback*> m_callbackQueue;
			std::vector<Releasable*> m_releaseQueue;
			std::vector<Block> m_releaseMemoryPool;
			GpuDevice& m_renderDevice;
	};

	class NAZARA_RENDERER_API GpuResources::Releasable
	{
		public:
			virtual ~Releasable();
	};

	class GpuResources::ReleasableCallback : public Releasable
	{
		public:
			virtual void Release() = 0;
	};

	template<typename T>
	class GpuResources::ReleasableData : public Releasable
	{
		public:
			ReleasableData(T&& data);
			ReleasableData(const ReleasableData&) = delete;
			ReleasableData(ReleasableData&&) = delete;
			~ReleasableData() = default;

			ReleasableData& operator=(const ReleasableData&) = delete;
			ReleasableData& operator=(ReleasableData&&) = delete;

		private:
			T m_data;
	};

	template<std::invocable T>
	class GpuResources::ReleasableFunctor final : public ReleasableCallback
	{
		public:
			template<typename U> requires(std::constructible_from<T, U&&>) ReleasableFunctor(U&& lambda);
			ReleasableFunctor(const ReleasableFunctor&) = delete;
			ReleasableFunctor(ReleasableFunctor&&) = delete;
			~ReleasableFunctor() = default;

			void Release() override;

			ReleasableFunctor& operator=(const ReleasableFunctor&) = delete;
			ReleasableFunctor& operator=(ReleasableFunctor&&) = delete;

		private:
			T m_lambda;
	};
}

#include <Nazara/Renderer/GpuResources.inl>

#endif // NAZARA_RENDERER_GPURESOURCES_HPP
