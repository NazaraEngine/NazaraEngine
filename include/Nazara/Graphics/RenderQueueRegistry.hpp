// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_RENDERQUEUEREGISTRY_HPP
#define NAZARA_GRAPHICS_RENDERQUEUEREGISTRY_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Skeleton.hpp>
#include <Nazara/Graphics/MaterialProxy.hpp>
#include <Nazara/Graphics/Thirdparty/ankerl/unordered_dense.h>
#include <Nazara/Renderer/GpuBuffer.hpp>
#include <Nazara/Renderer/RenderPipeline.hpp>
#include <NazaraUtils/Bitset.hpp>

namespace Nz
{
	class MaterialProxy;
	class GpuBuffer;
	class RenderPipeline;
	class Skeleton;
	class VertexDeclaration;

	class NAZARA_GRAPHICS_API RenderQueueRegistry
	{
		public:
			RenderQueueRegistry() = default;
			RenderQueueRegistry(const RenderQueueRegistry&) = delete;
			RenderQueueRegistry(RenderQueueRegistry&&) = delete;
			~RenderQueueRegistry() = default;

			inline void Clear();

			inline std::size_t FetchMaterialProxyIndex(const MaterialProxy* materialProxy) const;
			inline std::size_t FetchPipelineIndex(const RenderPipeline* pipeline) const;
			inline std::size_t FetchSkeletonIndex(const Skeleton* skeleton) const;
			inline std::size_t FetchVertexBuffer(const GpuBuffer* vertexBuffer) const;
			inline std::size_t FetchVertexDeclaration(const VertexDeclaration* vertexDeclaration) const;

			void RegisterMaterialProxy(const MaterialProxy* materialProxy);
			void RegisterPipeline(const RenderPipeline* pipeline);
			void RegisterSkeleton(const Skeleton* skeleton);
			void RegisterVertexBuffer(const GpuBuffer* vertexBuffer);
			void RegisterVertexDeclaration(const VertexDeclaration* vertexDeclaration);

			RenderQueueRegistry& operator=(const RenderQueueRegistry&) = delete;
			RenderQueueRegistry& operator=(RenderQueueRegistry&&) = delete;

		private:
			template<typename T, auto BitsetMap, auto ObjectMap> void Register(const T* ptr, Nz::Signal<T*>& onRelease);

			struct MaterialProxyEntry
			{
				std::size_t index;

				NazaraSlot(MaterialProxy, OnMaterialProxyRelease, onRelease);
			};

			struct GpuBufferEntry
			{
				std::size_t index;

				NazaraSlot(GpuBuffer, OnGpuBufferRelease, onRelease);
			};

			struct RenderPipelineEntry
			{
				std::size_t index;

				NazaraSlot(RenderPipeline, OnRenderPipelineRelease, onRelease);
			};

			struct SkeletonEntry
			{
				std::size_t index;

				NazaraSlot(Skeleton, OnSkeletonRelease, onRelease);
			};

			struct VertexDeclarationEntry
			{
				std::size_t index;

				NazaraSlot(VertexDeclaration, OnVertexDeclarationRelease, onRelease);
			};

			ankerl::unordered_dense::map<const MaterialProxy*, MaterialProxyEntry> m_materialProxies;
			ankerl::unordered_dense::map<const RenderPipeline*, RenderPipelineEntry> m_pipelines;
			ankerl::unordered_dense::map<const GpuBuffer*, GpuBufferEntry> m_vertexBuffers;
			ankerl::unordered_dense::map<const Skeleton*, SkeletonEntry> m_skeletons;
			ankerl::unordered_dense::map<const VertexDeclaration*, VertexDeclarationEntry> m_vertexDeclarations;
			Bitset<UInt64> m_materialProxyBitset;
			Bitset<UInt64> m_pipelineBitset;
			Bitset<UInt64> m_vertexBufferBitset;
			Bitset<UInt64> m_vertexDeclarationBitset;
			Bitset<UInt64> m_skeletonBitset;
	};
}

#include <Nazara/Graphics/RenderQueueRegistry.inl>

#endif // NAZARA_GRAPHICS_RENDERQUEUEREGISTRY_HPP
