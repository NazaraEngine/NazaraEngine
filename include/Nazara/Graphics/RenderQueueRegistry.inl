// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <cassert>

namespace Nz
{
	inline void RenderQueueRegistry::Clear()
	{
		m_materialProxies.clear();
		m_pipelines.clear();
		m_vertexBuffers.clear();
		m_vertexDeclarations.clear();
	}

	inline std::size_t RenderQueueRegistry::FetchMaterialProxyIndex(const MaterialProxy* materialPass) const
	{
		auto it = m_materialProxies.find(materialPass);
		assert(it != m_materialProxies.end());

		return it->second.index;
	}

	inline std::size_t RenderQueueRegistry::FetchPipelineIndex(const RenderPipeline* pipeline) const
	{
		auto it = m_pipelines.find(pipeline);
		assert(it != m_pipelines.end());

		return it->second.index;
	}

	inline std::size_t RenderQueueRegistry::FetchSkeletonIndex(const Skeleton* skeleton) const
	{
		auto it = m_skeletons.find(skeleton);
		assert(it != m_skeletons.end());

		return it->second.index;
	}

	inline std::size_t RenderQueueRegistry::FetchVertexBuffer(const RenderBuffer* vertexBuffer) const
	{
		auto it = m_vertexBuffers.find(vertexBuffer);
		assert(it != m_vertexBuffers.end());

		return it->second.index;
	}

	inline std::size_t RenderQueueRegistry::FetchVertexDeclaration(const VertexDeclaration* vertexDeclaration) const
	{
		auto it = m_vertexDeclarations.find(vertexDeclaration);
		assert(it != m_vertexDeclarations.end());

		return it->second.index;
	}
}
