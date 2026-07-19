// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/Algorithm.hpp>
#include <Nazara/Graphics/MaterialPass.hpp>

namespace Nz
{
	inline RenderSubmesh::RenderSubmesh(Int32 renderLayer, std::shared_ptr<MaterialProxy> materialProxy, MaterialPassFlags materialFlags, std::shared_ptr<GpuRenderPipeline> renderPipeline, UInt32 instanceIndex, const SkeletonInstance* skeletonInstance, std::size_t indexCount, IndexType indexType, std::shared_ptr<GpuBuffer> indexBuffer, std::shared_ptr<GpuBuffer> vertexBuffer, const Recti& scissorBox, const Spheref& boundingSphere, UInt32 renderMask) :
	RenderElement(BasicRenderElement::Submesh, instanceIndex, renderLayer, renderMask),
	m_indexBuffer(std::move(indexBuffer)),
	m_vertexBuffer(std::move(vertexBuffer)),
	m_materialProxy(std::move(materialProxy)),
	m_renderPipeline(std::move(renderPipeline)),
	m_indexCount(indexCount),
	m_skeletonInstance(skeletonInstance),
	m_indexType(indexType),
	m_materialFlags(materialFlags),
	m_scissorBox(scissorBox),
	m_boundingSphere(boundingSphere)
	{
	}

	inline const Spheref& RenderSubmesh::GetBoundingSphere() const
	{
		return m_boundingSphere;
	}

	inline const GpuBuffer* RenderSubmesh::GetIndexBuffer() const
	{
		return m_indexBuffer.get();
	}

	inline std::size_t RenderSubmesh::GetIndexCount() const
	{
		return m_indexCount;
	}

	inline IndexType RenderSubmesh::GetIndexType() const
	{
		return m_indexType;
	}

	inline const MaterialProxy& RenderSubmesh::GetMaterialProxy() const
	{
		return *m_materialProxy;
	}

	inline const GpuRenderPipeline* RenderSubmesh::GetRenderPipeline() const
	{
		return m_renderPipeline.get();
	}

	inline const Recti& RenderSubmesh::GetScissorBox() const
	{
		return m_scissorBox;
	}

	inline const SkeletonInstance* RenderSubmesh::GetSkeletonInstance() const
	{
		return m_skeletonInstance;
	}

	inline const GpuBuffer* RenderSubmesh::GetVertexBuffer() const
	{
		return m_vertexBuffer.get();
	}

	inline void RenderSubmesh::Register(RenderQueueRegistry& registry) const
	{
		registry.RegisterMaterialProxy(m_materialProxy.get());
		registry.RegisterPipeline(m_renderPipeline.get());
		registry.RegisterVertexBuffer(m_vertexBuffer.get());
		if (m_skeletonInstance)
			registry.RegisterSkeleton(m_skeletonInstance->GetSkeleton().get());
	}

	inline UInt64 RenderSubmesh::ComputeSortKey(const RenderQueueRegistry& registry) const
	{
		UInt64 elementType = GetElementType();

		UInt64 materialProxyIndex = registry.FetchMaterialProxyIndex(m_materialProxy.get());
		UInt64 pipelineIndex = registry.FetchPipelineIndex(m_renderPipeline.get());
		UInt64 vertexBufferIndex = registry.FetchVertexBuffer(m_vertexBuffer.get());

		UInt64 skeletonIndex = 0;
		if (m_skeletonInstance)
			skeletonIndex = registry.FetchSkeletonIndex(m_skeletonInstance->GetSkeleton().get());

		UInt64 layerIndex = 0;
		UInt64 matFlags = 0;

		// Opaque RQ index:
		// - Unused (previous render layer) (8bits)
		// - Unused (previously sorted by distance flag) (1bit)
		// - Element type (4bits)
		// - Pipeline (16bits)
		// - MaterialPass (16bits)
		// - VertexBuffer (8bits)
		// - Skeleton (8bits)
		// - Unused (3bits)

		return (layerIndex & 0xFF)              << 56 |
		       (matFlags)                       << 55 |
		       (elementType & 0xF)              << 51 |
		       (pipelineIndex & 0xFFFF)         << 35 |
		       (materialProxyIndex & 0xFFFF)    << 19 |
		       (vertexBufferIndex & 0xFF)       << 11 |
		       (skeletonIndex     & 0xFF)       << 3;
	}
}
