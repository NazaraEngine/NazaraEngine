// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/RenderQueueRegistry.hpp>
#include <NazaraUtils/Assert.hpp>

namespace Nz
{
	void RenderQueueRegistry::RegisterMaterialProxy(const MaterialProxy* materialProxy)
	{
		Register<MaterialProxy, &RenderQueueRegistry::m_materialProxyBitset, &RenderQueueRegistry::m_materialProxies>(materialProxy, materialProxy->OnMaterialProxyRelease);
	}

	void RenderQueueRegistry::RegisterPipeline(const RenderPipeline* pipeline)
	{
		Register<RenderPipeline, &RenderQueueRegistry::m_pipelineBitset, &RenderQueueRegistry::m_pipelines>(pipeline, pipeline->OnRenderPipelineRelease);
	}

	void RenderQueueRegistry::RegisterSkeleton(const Skeleton* skeleton)
	{
		Register<Skeleton, &RenderQueueRegistry::m_skeletonBitset, &RenderQueueRegistry::m_skeletons>(skeleton, skeleton->OnSkeletonRelease);
	}

	void RenderQueueRegistry::RegisterVertexBuffer(const GpuBuffer* vertexBuffer)
	{
		Register<GpuBuffer, &RenderQueueRegistry::m_vertexBufferBitset, &RenderQueueRegistry::m_vertexBuffers>(vertexBuffer, vertexBuffer->OnGpuBufferRelease);
	}

	void RenderQueueRegistry::RegisterVertexDeclaration(const VertexDeclaration* vertexDeclaration)
	{
		Register<VertexDeclaration, &RenderQueueRegistry::m_vertexDeclarationBitset, &RenderQueueRegistry::m_vertexDeclarations>(vertexDeclaration, vertexDeclaration->OnVertexDeclarationRelease);
	}

	template<typename T, auto BitsetMap, auto ObjectMap>
	void RenderQueueRegistry::Register(const T* ptr, Nz::Signal<T*>& onRelease)
	{
		constexpr std::size_t IndexGrowRate = 256;

		auto& objectMap = this->*ObjectMap;
		if (objectMap.contains(ptr))
			return;

		auto& bitset = this->*BitsetMap;

		std::size_t index = bitset.FindFirst();
		if (index == bitset.npos)
		{
			index = bitset.GetSize();
			bitset.Resize(index + IndexGrowRate, true);
		}

		bitset.Reset(index);

		auto& entry = objectMap[ptr];
		entry.index = index;
		entry.onRelease.Connect(onRelease, [this](T* object)
		{
			auto& bitset = this->*BitsetMap;
			auto& objectMap = this->*ObjectMap;

			auto it = objectMap.find(object);
			NazaraAssert(it != objectMap.end());
			bitset.UnboundedSet(it->second.index);

			objectMap.erase(it);
		});
	}
}
